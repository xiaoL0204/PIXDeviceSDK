

#ifndef _PPPP_CHANNEL_H_
#define _PPPP_CHANNEL_H_

#include <pthread.h>

#include "PPPP_API.h"
#include "defineutility.h"
#include "CircleBuf.h"





#import "Adpcm.h"
#import "CgiPacket.h"



#import "SDCardRecordFileSearchProtocol.h"


#import "PPPPObjectProtocol.h"
#import "AviManagement.h"
#include "PIXPO_Device_Protocol.h"
#include "AVIOCTRLDEFs.h"
#import "DeviceControlCommand.h"
#import "PIXSensorInfo.h"
#include "AudioRecorder.h"
#import "AudioPlayer.h"
#import "PIXDeviceProtocol.h"
#import "TimeZoneManager.h"

@class  PPPPChannelAdapter;
@class PIXEventReqInfo;

class CPPPPChannel:public  AudioRecorderProtocol
{
public:
    CPPPPChannel(CCircleBuf *pVideoBuf, CCircleBuf *pPlaybackVideoBuf, const char *DID, const char *user, const char *pwd);
    virtual ~CPPPPChannel();
    int Start();
    int Restart();
    void SetStop(); //外部强制停止设备
    int cgi_livestream(int bstart, int streamid); //视频流  bstart=1开启，bstart=0关闭
    
    int g726_audiostream(int bstart);
    
    /*发生视频播放命令
     *streamIndex 0 高清，1-标清
     */
    int StartVideoCommond(int streamIndex);
    int StopVideoCommond(int streamIndex);
    
    int cgi_get_common(char *cgi);
    int get_cgi(int cgi);
    int PTZ_Control(int command);
    int GetWIFIList();
    
    
    int CameraControl(int param, int value);
    int Snapshot();
    void ReconnectImmediately();
    int LoginDevice();
    int StartAudio(id<G7262PCMDecodeDelegate>g726Decode);
    int StopAudio();
    int StartTalk(id<PCM2G726EncodeDelegate>g726Encode);
    int StopTalk();
    int StopPlayback();
    int StartPlayback(char *szFilename, int offset);
    int EnqueueG726Data(const char*data, int len);
    int SetSystemParams(int type,char * msg,int len);
    void SetWifiParamsDelegate(id delegate);
    int SetWifi(int enable, char *szSSID, int channel, int mode, int authtype, int encrypt, int keyformat, int defkey, char *strKey1, char *strKey2, char *strKey3, char *strKey4, int key1_bits, int key2_bits, int key3_bits, int key4_bits, char *wpa_psk);
    int SendWifiSetting(char *msg, int len);
    int GetResult(char *pbuf, int len);

    void SetUserPwdParamsDelegate(id delegate);
    int SetUserPwd(char *user1,char *pwd1,char *user2,char *pwd2,char *user3,char *pwd3);
    

    void SetDateTimeParamsDelegate(id delegate);
    int SetDateTime(int now,int tz,int ntp_enable,char *ntp_svr);
    
    void SetFtpDelegate(id delegate);
    int SetFtp(char *szSvr, char *szUser, char *szPwd, char *dir, int port, int uploadinterval, int mode);
    
    void SetMailDelegate(id delegate);
    int SetMail(char *sender, char *smtp_svr, int smtp_port, int ssl, int auth, char *user,  char *pwd, char *recv1, char *recv2, char *recv3, char *recv4);
    
    void SetAlarmParamsDelegate(id delegate);
    int SetPlayAlarm(int motion_armed,int input_armed);
    int SetAlarm(  
                 int motion_armed,
                 int motion_sensitivity,
                 int input_armed,
                 int ioin_level,
                 int alarmpresetsit,
                 int iolinkage,
                 int ioout_level,
                 int mail,
                 int upload_interval,
                 int record,
                 int enable_alarm_audio
                 );
    
    void SetSDCardSearchDelegate(id delegate);
    void SetPlaybackDelegate(id delegate);
    
    
    void SetSDcardOver(int over);
    void ProcessResult(int result,int type);
    void SetResultDelegate(id delegate);
    void SetAlarmLogsDelegate(id delegate);
    void SetServer(NSString *server);
    
    void startRecordAVI(char *path,char *format,int width,int height);
    void stopRecordAVI();
    
    
    /**
     *  获取摄像头的高级设置信息
     */
    int fetchAdvanceSettingsCommond();
    int setSpeakerVolumeCommand(int volume);      //设置扬声器音量
    int setMicrophoneVolumeCommand(int volume);   //设置麦克风音量
    int setGMTCommand(int gmt);                   //时区
    int setTimeZoneCommand(TimeZoneInfo *info,NSInteger index);   //设置时区
   
    
//    int sensorEditCommand(ENUM_EDIT_COMMOND editCommand,int savedIndex);                  //传感器删除、获取列表
//    int sensorModifyCommand(ENUM_EDIT_COMMOND editCommand,PIXSensorInfo *sensorInfo);  //传感器设置名称、添加
    
    int matchCodeCommand();                             //对码
    
    //传感器操作     获取传感器列表、或者某个传感器的信息，editCommand有效值:2、3
    int fetchSensorInfoCommand(SensorEditType editCommand,int sensorIndex);
    //传感器操作     设置名称、删除，editCommand有效值:0、1、4、5
    int modifySensorInfoCommand(SensorEditType editCommand,PIXSensorInfo *sensorInfo);
    //操作传感器     当areaInfo.savedIndex==-1时，表示是新建的防区   (发的命令是USER_EDIT_PARAMS_CMD_ADD_ONE)
    int addSensorCommand(NSString *sensorName,PIXAlarmAreaInfo *areaInfo);
    
    
    //防区操作      获取防区列表、或者某个防区的信息，editCommand有效值:2、3
    int fetchAlarmAreaInfoCommand(ENUM_EDIT_COMMOND editCommand,int alarmAreaIndex);
    //防区操作      防区修改名称、添加、删除，editCommand有效值:0、1、4、5、6
    int modifyAlarmAreaInfoCommand(SensorEditType editCommand,PIXAlarmAreaInfo *info);
    
    //编辑防区内的传感器  添加、删除传感器、删除防区
    int editAlarmAreaSensorListCommand(SensorEditType editCommand,int sensorIndex,int areaIndex);
    
    //获取防区布防、撤防
    int fetchGlobalAlarmEnableCommand();
    //设置防区布防、撤防   alarmEnable:1布防，0撤防    //无论设置成功与否，都会返回防区状态
    int setGlobalAlarmEnableCommand(BOOL alarmEnable);
    
    int setInfraredAlertModeCommand(int alertMode);     //是否红外报警
    int setVideoFlipModeCommand(int flipMode);          //设置图像翻转 0:no move, 1: vertical, 2: horizon, 3:vertical and horizon
    int setIRModeCommand(int raylightMode);       //设置红外模式
    int setEnvModeCommand(int envMode);           //环境模式
    int setOSDEnableCommand(int osdEnable);       //水印时钟
    int setMotionSensitivityCommand(int sensitivity,int enable);  //移动侦测  1:enable,0:disable
    int setAlarmModeCommand(int alarmMode);       //事件通知
    int setRecordModeCommand(int recordMode);     //设置录像模式
    int formatExtStorageCommand(int storageIndex);       //格式化外设存储
    int setWifiCommand(PIXWIFInfo *wifiInfo);
    int setPasswordCommand(NSString *oldPassword,NSString *nowPassword);   //修改摄像机登录密码
    int fetchEventListCommand(PIXEventReqInfo *info);                      //事件回放列表
    //开始播放事件，播放前不需要调用停止命令。事件相同时，摄像头不理会start命令。事件不同时，摄像头会自动停止上次播放，并开始本次播放。
    int startPlaybackCommand(long long timeDay);
    int stopPlaybackCommand(long long timeDay);     //stop,and clear buffer   //完全停止播放
    int pauseOrRestorePlaybackCommand(long long timeDay,int pauseOrRestore);    //1:pause , 2:restore(暂停和恢复播放都是pause命令，但是暂停需要停止播放线程，恢复需要开启线程)
    int seekPlaybackCommand(long long timeDay,int seekTime);
    
    int setIRLedModeCommand(int irLedMode);      //红外灯设置
    int fetchRecordTimeCommand(int recordType);    //获取录像时间
    int setRecordTimeCommand(int recordType,unsigned char time[48]);  //设置录像时间
    
    
    
public:
    void RecoredDataCallback(const void * g726Data ,int length);

public:
    //这个delegate是用来给cameraviewcontroller通知pppp的状态的
    id <PPPPObjectProtocol> m_PPPPStatusDelegate;
    id <PPPPObjectProtocol> m_CameraViewSnapshotDelegate;
    id <PPPPObjectProtocol> m_IOCMDDelegate;

    void SetPlayViewPPPPStatusDelegate(id delegate);
    void SetPlayViewParamNotifyDelegate(id delegate);
    void SetPlayViewImageNotifyDelegate(id delegate);
    void SetPlaybackImageNotifyDelegate(id delegate);

private:
    int SscanfInt(const char *pszBuffer , const char *reg , int &nResult);
    int PPPP_IndeedRead(UCHAR channel, CHAR *buf, int len);    
    void CommandRecvProcess();
    void DataProcess();
    void TalkProcess();
    void PlaybackProcess();
    void AlarmProcess();  
    void Stop();
    int SendTalk(char * pbuf,int len);
    
    int StartCommandChannel();
    int StartDataChannel();
    int StartPlaybackChannel();
    int StartAudioChannel();
    int StartTalkChannel();
    int StartAlarmChannel();
    int StartCommandRecvThread();
    
    void StartPlaybackVideoPlayer();
    void StopPlaybackVideoPlayer();
    
    static void* CommandRecvThread(void* param);
    static void* CommandThread(void* param);       
    static void* DataThread(void* param); 
    static void* TalkThread(void* param);      
    static void* PlaybackThread(void* param);      
    static void* AlarmThread(void* param);
    static void* AudioThread(void* param);
    static void* PlaybackVideoPlayerThread(void* param);
    void PlaybackVideoPlayerProcess();
    
    void CommandProcess(); 
    void AudioProcess();
    void ProcessCommand(int cmd, char *pbuf, int len);
    void ProcessCameraParams(char *pbuf, int len);   
    int AddCommand(void* data, int len); 
    void StopOtherThread();
    void MsgNotify(int MsgType, int Param);
    void MainWindowNotify(int MsgType, int Param);
    void PlayWindowNotify(int MsgType, int Param);
    //void AVNofity(char *buf, int len);
    void PlayViewParamNotify(int paramType, void* param);    
    void StartVideoPlay();
    void StopVideoPlay();
    
    void StartAudioPlay();   //开启读音频数据纯种
    void StopAudioPlay();   //关闭音频
    
    void ProcessSnapshot(char *pbuf, int len);    
    void PPPPClose();
    void ImageNotify(UIImage *image, unsigned int timestamp);
    void ImageData(unsigned char* buf,int len,unsigned int timestamp);
    void YUVNotify(unsigned char* yuv, int len, int width, int height, unsigned int timestamp);
    
    static void* PlayThread(void* param);
    static void* PlayAudioThread(void* param);
    static void* SendAudioThread(void* param);
    void PlayProcess();
    void PlayAudioProcess();
    void SendAudioProcess();
    
    void G726DataNofify(char* g726Data,int length);
    void H264DataNotify(unsigned char *h264Data, PIXVideoInfo *videoinfo);
    void MicDataNofify(unsigned char *pcmData,int length);   //对讲的时候麦克风采集的数据
    void RecordH264DataNotify(char *did, char *h264Data, long bytes, int keyframe,int timezone,long long timestamp);  //录像时视频数据
    void RecordAudioDataNotify(char *did, char *data, long bytes);   //录像时音频数据

    void playbackH264DataNotify(char *did, char *h264Data,int length,int timeStamp);   //事件回放视频数据,length:视频数据的长度，timeStamp:时间戳，当前播放的时间
    
    void PlaybackYUVNotify(unsigned char *yuv, int len, int width, int height, unsigned int timestamp);
    void PlaybackImageNotify(UIImage *image, unsigned int timestamp);
    
    void ProcessWifiScanResult(STRU_WIFI_SEARCH_RESULT_LIST wifiSearchResultList);
    void ProcessWifiParam(STRU_WIFI_PARAMS wifiParams);

    void ProcessUserInfo(STRU_USER_INFO userInfo);
    void ProcessDatetimeParams(STRU_DATETIME_PARAMS datetime);
    void ProcessAlaramParams(STRU_ALARM_PARAMS alarm);
    void ProcessFtpParams(STRU_FTP_PARAMS ftpParam);
    void ProcessMailParams(STRU_MAIL_PARAMS mailParam);
    void ProcessRecordFile(STRU_RECORD_FILE_LIST recordFileList);
    void ProcessRecordParam(STRU_SD_RECORD_PARAM recordFileList);
    void ProcessCheckUser(char *pbuf, int len);
    const char * getCMDEncrptedStr(const char *plainTxt,int step);
    static void* startRecordAVIChannel(void *param);
    void startRecordAVIProcess();
//    void ProcessCommandEx(PP2P_OJT_RESP_HEAD_INFO_STRUCT pCmdhead, const char * dataBuffer, const int dataSize);
    void ProcessCommandEx(PP2P_OJT_RESP_HEAD_INFO_STRUCT pCmdhead, char * dataBuffer, const int dataSize);    
    
private:
    /*命令返回处理函数*/
    void ProcessWIFIList(const SMsgAVIoctrlListWifiApResp  * pRsp);
    
//    void handleDeviceAdvanceInfoReceiveInfo(const SMsgAVIoctrlGetAdvanceSettingResp *dataBuffer);
    void receivePushEventReport(SMsgAVIoctrlEvent1 *pRsp);      //收到推送
    
    void handleDeviceAdvanceInfoReceiveInfo(const char *dataBuffer);
//    void setMicophoneVolumeCommandBack(SMsgAVIoctrlSetVolumeResp *pRsp);
//    void setSpeakerVolumeCommandBack(SMsgAVIoctrlSetVolumeResp *pRsp);
    
    void setMicophoneVolumeCommandBack(int resultCode);
    void setSpeakerVolumeCommandBack(int resultCode);
    void setGMTCommandBack(int resultCode);
    void setTimeZoneCommandBack(int resultCode);
    
    void matchCodeCommandBack(int endFlag,int resultCode,const char *dataBuffer);    //0表示成功
    
    
//    void fetchSensorListCommandBack(int endFlag,int allCount,const char *dataBuf);   //endFlag==1时结束,endFlag为头数据   获取传感器列表
//    void modifySensorBack(SensorEditType command,int resultCode);    //传感器删除、修改
    
    //传感器操作  获取传感器列表、某个传感器信息
    void fetchSensorInfoCommandBack(int endFlag,int allCount,const char *dataBuf);
    //传感器操作  修改、增加、删除传感器
    void modifySensorInfoCommandBack(SensorEditType command,int resultCode);
    
    
    
    //防区操作   获取防区列表、某个防区信息
    void fetchAlarmAreaInfoCommandBack(int endFlag,int allCount,const char *dataBuf);
    //防区操作   修改、增加、删除防区
    void modifyAlarmAreaInfoCommandBack(int resultCode,SensorEditType command);
    //编辑防区内的传感器
    void editAlarmAreaSensorListCommandBack(int resultCode,SensorEditType command);
    
    void fetchGlobalAlarmEnableCommandBack(int resultCode,const char *dataBuf);    //获取防区状态
    void setGlobalAlarmEnableCommandBack(int resultCode,const char *dataBuf);      //无论设置成功与否，都会返回防区状态
    
    void setInfraredAlertModeCommandBack(int resultCode);   //红外报警
    void setVideoFlipModeCommandBack(int resultCode);       //图像翻转
    void setIRModeCommandBack(int resultCode);        //红外模式
    void setEnvModeCommandBack(int resultCode);       //环境模式
    void setOSDEnableCommandBack(int resultCode);     //水印时钟
    void setMotionSensitivityCommandBack(int resultCode);  //移动侦测
    void setAlarmModeCommandBack(int resultCode);     //事件通知
    void setRecordModeCommandBack(int resultCode);    //录像模式
    void formatExtStorageCommandBack(int resultCode);     //格式化SD卡
    void setWifiCommandBack(int resultCode);
    void setPasswordCommandBack(int resultCode);  //修改登录密码     0:success , otherwise:fail
//    void fetchEventListCommandBack(SMsgAVIoctrlListEventResp *pRsp);
    void fetchEventListCommandBack(char *dataBuf);
    
    void playbackCommandBack(SMsgAVIoctrlPlayRecordResp *pRsp);                  //事件回放大命令返回，会返回小命令
    void setIRLedModeCommandBack(int resultCode);
    void fetchRecordTimeCommandBack(SMsgAVIoctrlGetRecordResq1 *pRsp);
    void setRecordTimeCommandBack(int resultCode);
    
private:
    int m_bOnline;    
    int m_bOnConnecting;
   
    id <PPPPObjectProtocol> m_PlayViewPPPPStatusDelegate;
    id <PPPPObjectProtocol> m_PlayViewParamNotifyDelegate;
    id <PPPPObjectProtocol> m_PlayViewImageNotifyDelegate;

//    id <PPPPObjectProtocol> m_PlaybackViewImageNotifyDelegate;
    
    id  m_PlaybackViewImageNotifyDelegate;
    
    id <PPPPObjectProtocol> m_WifiParamsDelegate;
    
    
    id <PPPPObjectProtocol> m_UserPwdParamsDelegate;
    
    
    id <PPPPObjectProtocol> m_DateTimeParamsDelegate;
    
    id <PPPPObjectProtocol> m_AlarmParamsDelegate;
    
    id <SDCardRecordFileSearchProtocol> m_SDCardSearchDelegate;
   
    id <PPPPObjectProtocol> m_FtpDelegate;
   
    id <PPPPObjectProtocol> m_MailDelegate;
   
    id<PPPPObjectProtocol>m_SDcardScheduleDelegate;
    
   
    id<PPPPObjectProtocol>m_SetResultDelegate;
    
    id<PPPPObjectProtocol>m_AlarmLogsDelegate;
    
    int m_bConnectThreadRuning;
    int m_bCommandThreadRuning;
    int m_bCommandRecvThreadRuning;
    int m_bDataThreadRuning;
    int m_bTalkThreadRuning;
    int m_bPlaybackThreadRuning;
    int m_bAlarmThreadRuning;
    int m_bAudioThreadRuning;    
    int m_bRead;
    NSString *strServer;
    
    pthread_t m_CommandThreadID;
    pthread_t m_DataThreadID;
    pthread_t m_TalkThreadID;
    pthread_t m_PlaybackThreadID;
    pthread_t m_AlarmThreadID;
    pthread_t m_CommandRecvThreadID;
    pthread_t m_AudioThreadID;    
    
    
    ENUM_VIDEO_MODE_B m_EnumVideoMode;
    int m_bFindIFrame;
    CCircleBuf *m_pVideoBuf;    
    
    
	pthread_t m_PlayThreadID;    
	int m_bPlayThreadRuning;
    
    pthread_t m_PlayAudioThreadID;
    int m_bPlayAudioThreadRuning;
    
    pthread_t m_PlaybackVideoPlayerThreadID;    
	int m_bPlaybackVideoPlayerThreadRuning; 
    
    char szDID[64];
    char szUser[64];
    char szPwd[64];    
    
    CCircleBuf *m_pCommandBuffer;    
    
    int m_bReconnectImmediately;
    
//    CCircleBuf *m_pAudioBuf;
    CCircleBuf *m_pTalkAudioBuf;
    
    CAdpcm *m_pAudioAdpcm;
    CAdpcm *m_pTalkAdpcm;

    int m_bAudioStarted;  
    int m_bTalkStarted;

    CCgiPacket m_CgiPacket;
    
    ENUM_VIDEO_MODE_B m_EnumPlayBackVideoMode;
    int m_bPlayBackFindIFrame;
    CCircleBuf *m_pPlayBackVideoBuf;
    //CVideoPlayer *m_pPlayBackVideoPlayer;
    int m_bPlayBackStreamOK;
    BOOL m_bPlaybackStarted;
    
    
    int playbackwriteNum;
    int playbackreadNum;
   
    int sdcardOver;
    
    
    //录制AVI格式视频
    CAviManagement *aviRecordMgt;
    pthread_t m_AVIThreadID;
    int m_bAVIRecordThreadRunning;
    CCircleBuf *m_BufRecordAVIVideo;
    CCircleBuf *m_BufRecordAVIAudio;
    
    NSCondition *playVideoLock;
    NSCondition *playBackVideoLock;
    AudioRecorder *m_recorder;
    AudioPlayer *m_audioPlayer;
//    NSMutableArray *sensorArray;   //fetch sensor list 
    
public:
    CCircleBuf *m_pAudioBuf;
    INT32 m_hSessionHandle;
    

};



#endif
