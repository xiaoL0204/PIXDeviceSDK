
#import <UIKit/UIKit.h>

#include <pthread.h>
#include "string.h"
#include "defineutility.h"
#include "PPPPDefine.h"
#include "PPPPChannel.h"
#include "cmdhead.h"
#include "moto.h"

#include "P2P_API_Define.h"
//#include "H264Decoder.h"
#include "PIXPO_Device_Protocol.h"
#include "AVIOCTRLDEFs.h"
#import "PIXDeviceProtocol.h"
#import "PIXWIFInfo.h"
#import "PIXAdvanceSetting.h"
#import "PIXSensorInfo.h"
#import "DeviceControlCommand.h"
#import "PIXStruct.h"
#import "PIXDevice.h"
#import "PIXEventInfo.h"
#import "PIXPushEventInfo.h"
#import "SENSORDEFs.h"

//extern "C"
//{
//#include "H264iPhone.h"
//}

//#import "libH264Dec.h"
CPPPPChannel::CPPPPChannel(CCircleBuf *pVideoBuf, CCircleBuf *pPlaybackVideoBuf, const char *DID, const char *user, const char *pwd)
{ 
    memset(szDID, 0, sizeof(szDID));
    strcpy(szDID, DID);

    memset(szUser, 0, sizeof(szUser));
    strcpy(szUser, user);

    memset(szPwd, 0, sizeof(szPwd));
    strcpy(szPwd, pwd);     
    m_bRead=1;
    //init
    m_bConnectThreadRuning=0;
    m_bAlarmThreadRuning = 0;
    m_bCommandThreadRuning = 0;
    m_bDataThreadRuning = 0;
    m_bPlaybackThreadRuning = 0;
    m_bTalkThreadRuning = 0;
    m_bCommandRecvThreadRuning = 0;
    m_bAudioThreadRuning = 0;
    m_CommandRecvThreadID = NULL;
    m_CommandThreadID = NULL;
    m_DataThreadID = NULL;
    m_PlaybackThreadID = NULL;
    m_TalkThreadID = NULL;
    m_AlarmThreadID = NULL;
    m_AudioThreadID = NULL;
    m_pCommandBuffer = NULL;
    m_bFindIFrame = 0;
    m_pVideoBuf = pVideoBuf;
    m_pPlayBackVideoBuf = pPlaybackVideoBuf;
    m_hSessionHandle = -1;
    m_bOnline = 0;
    m_IOCMDDelegate = nil;
    m_PPPPStatusDelegate = nil;   
    m_CameraViewSnapshotDelegate = nil;
    m_PlayViewPPPPStatusDelegate = nil;
    //m_PlayViewAVDataDelegate = nil;    
    m_PlayViewParamNotifyDelegate = nil;
    m_PlayViewImageNotifyDelegate = nil;
    m_WifiParamsDelegate = nil;
    m_UserPwdParamsDelegate = nil;
    m_DateTimeParamsDelegate = nil;
    m_AlarmParamsDelegate = nil;
    m_SDCardSearchDelegate = nil;
    m_FtpDelegate = nil;
    m_MailDelegate = nil;
    m_PlaybackViewImageNotifyDelegate = nil;
    m_AlarmLogsDelegate=nil;
    

    m_bPlayThreadRuning = 0;
    m_PlayThreadID = NULL;
    
    m_bPlayAudioThreadRuning = 0;
    m_PlayAudioThreadID = NULL;
    
    m_EnumVideoMode = ENUM_VIDEO_MODE_UNKNOWN;    
    m_bReconnectImmediately = 0;    
    m_bAudioStarted = 0;
    m_bTalkStarted = 0;
    
    m_pTalkAdpcm = NULL;
    m_pAudioBuf = NULL;
    m_pTalkAudioBuf = NULL;
    m_pAudioAdpcm = NULL;    
    
    m_bPlayBackFindIFrame = 0;
    m_bPlayBackStreamOK = NULL;
    m_bPlaybackStarted = NO;
    m_EnumPlayBackVideoMode = ENUM_VIDEO_MODE_UNKNOWN;
    m_PlaybackVideoPlayerThreadID = NULL;
    playbackreadNum=0;
    playbackwriteNum=0;
    
    sdcardOver=0;
    
    //AVI Record
    aviRecordMgt=new CAviManagement();
    m_AVIThreadID=NULL;
    m_bAVIRecordThreadRunning=0;
    m_BufRecordAVIAudio=NULL;
    m_BufRecordAVIVideo=NULL;
    
    playVideoLock=[[NSCondition alloc]init];
    playBackVideoLock=[[NSCondition alloc]init];
    m_audioPlayer = new AudioPlayer();
}

CPPPPChannel::~CPPPPChannel()
{
    m_IOCMDDelegate = nil;
    m_PPPPStatusDelegate = nil;
    m_CameraViewSnapshotDelegate = nil;
    m_PlayViewPPPPStatusDelegate = nil;
    //m_PlayViewAVDataDelegate = nil;
    m_PlayViewParamNotifyDelegate = nil;
    m_PlayViewImageNotifyDelegate = nil;
    m_WifiParamsDelegate = nil;
    m_UserPwdParamsDelegate = nil;
    m_DateTimeParamsDelegate = nil;
    m_AlarmParamsDelegate = nil;
    m_SDCardSearchDelegate = nil;
    m_FtpDelegate = nil;
    m_MailDelegate = nil;
    m_PlaybackViewImageNotifyDelegate = nil;
    m_AlarmLogsDelegate=nil;
    
    [playBackVideoLock release];
    playBackVideoLock=nil;
    [playVideoLock release];
    playVideoLock=nil;
    Stop();
    SAFE_DELETE(m_audioPlayer);
    
    
    
}



void CPPPPChannel::CommandRecvProcess()
{
    //F_LOG;
    
    while(m_bCommandRecvThreadRuning)
    {
        
        //read head
        P2P_OJT_RESP_HEAD_INFO_STRUCT cmdhead;
        memset(&cmdhead, 0, sizeof(cmdhead));
        INT32 res = PPPP_IndeedRead(P2P_CMDCHANNEL, (CHAR*)&cmdhead, sizeof(cmdhead));
        
        PLogInfo(@"CommandRecvProcess did:%s 消息返回结果，读取头部信息 ：RespCmd =%x   , EditCmd =%d  ,  DataSize = %d , AllCounts = %d , SaveIndex = %d  EndFlag = %d, iRet = %d",szDID,
              cmdhead.RespCmd,cmdhead.EditCmd,cmdhead.DataSize, cmdhead.AllCounts, cmdhead.SaveIndex, cmdhead.EndFlag, cmdhead.iRet);
        if(res < 0)
        {
            return ;
        }
        
        
        if(cmdhead.DataSize == 0)
        {
            ProcessCommandEx(&cmdhead, NULL, 0);
            
            continue;
        }
        
        char *pbuf = new char[cmdhead.DataSize];
        //read data
        res = PPPP_IndeedRead(P2P_CMDCHANNEL, (CHAR *)pbuf, cmdhead.DataSize);
        
        if(res < 0)
        {
            SAFE_DELETE(pbuf);
            return;
        }
        
        //Process command
        ProcessCommandEx(&cmdhead, pbuf, cmdhead.DataSize);
        
        SAFE_DELETE(pbuf);       
        
    }
}

#pragma mark command process

void CPPPPChannel::ProcessCommandEx(PP2P_OJT_RESP_HEAD_INFO_STRUCT pCmdhead, char * dataBuffer, const int dataSize){
    PLogInfo(@"ProcessCommandEx commond RespCmd:0X%x  dataSize:%d",pCmdhead->RespCmd,dataSize);
    
    
    switch (pCmdhead->RespCmd) {
        
        case IOTYPE_USER_IPCAM_EVENT_REPORT1:    //push by P2P
            receivePushEventReport((SMsgAVIoctrlEvent1 *)dataBuffer);
            break;
        case IOTYPE_USER_IPCAM_SET_TIMEZONE_RESP:      //timeZone
            setGMTCommandBack(pCmdhead->iRet);
            break;
        case IOTYPE_USER_IPCAM_SET_TIMEZONE_RESP1:     //set timeZone V1
            setTimeZoneCommandBack(pCmdhead->iRet);
            break;
        case IOTYPE_USER_IPCAM_EDIT_SENSOR_LIST_RSP:   //sensor control     大命令
        {
            //小命令
            if (pCmdhead->EditCmd == USER_EDIT_PARAMS_CMD_GET_ALL || pCmdhead->EditCmd == USER_EDIT_PARAMS_CMD_GET_ONE) {       //all sensors ,fetch sensor list
//                fetchSensorListCommandBack(pCmdhead->EndFlag,pCmdhead->AllCounts,dataBuffer);
                fetchSensorInfoCommandBack(pCmdhead->EndFlag, pCmdhead->AllCounts, dataBuffer);
            }else if (pCmdhead->EditCmd == USER_EDIT_PARAMS_CMD_DEL_ONE || pCmdhead->EditCmd == USER_EDIT_PARAMS_CMD_DEL_ALL || pCmdhead->EditCmd == USER_EDIT_PARAMS_CMD_SET_ONE || pCmdhead->EditCmd == USER_EDIT_PARAMS_CMD_SET_ALL || pCmdhead->EditCmd == USER_EDIT_PARAMS_CMD_ADD_ONE){  //delete sensor   //change sensor name  //add sensor(with alarm area)
                modifySensorInfoCommandBack((SensorEditType)pCmdhead->EditCmd, pCmdhead->iRet);
            }
            break;
        }
            
//        case IOTYPE_USER_IPCAM_SENSOR_ALARM_EVENT_REPORT:    //是摄像机对码后主动的通信
        case IOTYPE_USER_IPCAM_SENSOR_REPORT:    //摄像机收到对码指令后，立即返回IOTYPE_USER_IPCAM_EDIT_ALARM_STATUS_RSP，之后按对码的结果来推送消息
        {
            if (pCmdhead->EditCmd == SENSOR_ALARM_CMD_ENABLE_MATCH_CODE) {  //对码
                matchCodeCommandBack(pCmdhead->EndFlag,pCmdhead->iRet,dataBuffer);
            }
            break;
        }
        
        default:
            break;
    }
    

}


#define N 8
#define M 1<<(N-1)
void printBit(BYTE bit){
    for (int i = 0; i <N ; i++) {
        if (i%8 == 0) printf("    \n");
            putchar(((bit&M)==0)?'0':'1');
            bit<<=1;
        printf(",");
    }
}

void CPPPPChannel::fetchRecordTimeCommandBack(SMsgAVIoctrlGetRecordResq1 *pRsp){
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
    
    sTimeTableType timeTable = pRsp->sRecordPara.sTimePara;
    unsigned int recordType = pRsp->recordType;
    PLogInfo(@"recordType:%d",recordType);
    
    //48个字节，每1个字节的高位舍弃掉，最低位代表星期天，高一位代表星期一，一直到星期六
    //48个字节，每个字节最低位为星期天(下标为7、15、23...)
    //每个字节高一位为星期一(下标为6、14、22...)
    NSMutableArray *timeArray = [NSMutableArray array];
    for (int i = 0; i < TIME_TABLE_NS; i++) {
        unsigned char bit = timeTable.fgTimeEnTable[i];
//        printBit(bit);
        for (int j = 0; j<N; j++) {
            [timeArray addObject:[NSNumber numberWithBool:((bit&M)==0) ? NO : YES]];
            bit<<=1;
        }
    }
    
    if (m_IOCMDDelegate && [m_IOCMDDelegate respondsToSelector:@selector(fetchRecordTimeBack:timeSelectArray:)]) {
        [m_IOCMDDelegate fetchRecordTimeBack:recordType timeSelectArray:timeArray];
    }
    [pool drain];
}




//收到推送
void CPPPPChannel::receivePushEventReport(SMsgAVIoctrlEvent1 *pRsp){
    if (pRsp) {
        
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc]init];
        PIXPushEventInfo *event = [[PIXPushEventInfo alloc]init];
        event.uid = [NSString stringWithUTF8String:szDID];
        event.utcTime = pRsp->time;
        event.alarmMode = (ENUM_ALARM_MODE)pRsp->alarm_mode;
        event.eventType = (ENUM_PUSH_EVENT_TYPE)pRsp->event;
        
//        NSLog(@"-----receivePushEventReport:%@    %@   %d   %x",event.uid,@(event.utcTime),event.alarmMode,event.eventType);
        if (m_IOCMDDelegate &&
            [m_IOCMDDelegate respondsToSelector:@selector(receivePushEventReport:)]) {
            [m_IOCMDDelegate receivePushEventReport:event];
        }
        [pool drain];
    }
}



void CPPPPChannel::PlayProcess()
{
    
#if 0
    CH264Decoder *pH264Decoder = new CH264Decoder();//创建h264的解码库
#endif
    PLogInfo(@"video play thread started!");
    while(m_bPlayThreadRuning)
    {
        if (m_EnumVideoMode == ENUM_VIDEO_MODE_UNKNOWN) {
            usleep(100000);
            continue;
        }
        
        if(m_pVideoBuf->GetStock() == 0)
        {
            //PLogInfo(@"实时视频buf中没有数据。。。等待10毫秒");
            usleep(10000);
            continue;
        }        
        
        char *pbuf = NULL;
        int videoLen = 0;
        
        VIDEO_BUF_HEAD videohead;
        memset(&videohead, 0, sizeof(videohead));
        pbuf = m_pVideoBuf->ReadOneFrame1(videoLen, videohead) ;
        if(NULL == pbuf)
        {
            PLogInfo(@"read NULL,wating 10 ms");
            usleep(10000);
            continue;
        }
        
        unsigned int untimestamp = videohead.timestamp;
        
        //PLogInfo(@"get one frame");
        if(ENUM_VIDEO_MODE_H264 == m_EnumVideoMode)
        {

#if 0
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            int yuvlen = 0;
            int nWidth = 0;
            int nHeight = 0;
            //PLogInfo(@"PPPPPChannel  下面解码的");

             [g_DecoderLock lock];
        
            if (pH264Decoder->DecoderFrame((uint8_t*)pbuf, videoLen, nWidth, nHeight)) {
                //PLogInfo(@"nWidth=%d nHeight=%d",nWidth,nHeight);
                yuvlen=nWidth*nHeight*3/2;
                uint8_t *pYUVBuffer = new uint8_t[yuvlen];
                if (pYUVBuffer != NULL) {
                    int nRec=pH264Decoder->GetYUVBuffer(pYUVBuffer, yuvlen);
                    
                    if (nRec>0) {
                         YUVNotify(pYUVBuffer, yuvlen, nWidth, nHeight, untimestamp);
                    }
                    delete pYUVBuffer;
                    pYUVBuffer = NULL;
                }
            }

            [g_DecoderLock unlock];
            [pool release];

#endif
            PIXVideoInfo * videInfo =[[PIXVideoInfo alloc] init];
            videInfo.frametype = videohead.frametype;
            videInfo.onlinenum = videohead.onlinenum;
            videInfo.length = videoLen;
            H264DataNotify((unsigned char*)pbuf, videInfo);
            [videInfo release];
            SAFE_DELETE(pbuf)
            
        }      
        else /* JPEG */
        {
            [playVideoLock lock];
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
//            PLogInfo(@"image.length=%d",videoLen);
            NSData *image = [[NSData alloc] initWithBytes:pbuf length:videoLen];
            UIImage *img = [[UIImage alloc] initWithData:image];
            
            ImageNotify(img, untimestamp);
            ImageData((unsigned char*)pbuf, videoLen,untimestamp);
            [img release];
            [image release];            
            [pool release];
            [playVideoLock unlock];
        }
        //PLogInfo(@"往上显示一帧");
        SAFE_DELETE(pbuf) ;
        
    }
    
    
    PLogInfo(@"video play thread end。。。");

#if 0
    delete pH264Decoder;
    pH264Decoder=NULL;
#endif
}




