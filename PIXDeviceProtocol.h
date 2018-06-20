//
//  PIXDeviceProtocol.h
//  PIXDeviceSDK
//
//  Created by xiaol on 15/6/16.
//  Copyright (c) 2015年 DFTX. All rights reserved.
//

#import <Foundation/Foundation.h>
//pppp status
#import "PIXVideoInfo.h"
#import "P2P_API_Define.h"
#import "PIXAdvanceSetting.h"
#import "PIXSensorInfo.h"
#import "PIXDevice.h"
#import "PIXEventInfo.h"
#import "PIXPushEventInfo.h"
#import "PIXStruct.h"


@protocol PIXDeviceProtocol <NSObject>
#pragma mark-- 局域网搜索回调

/*
 *devices 包含PIXDevice对象
 */
-(void)LanSearchResult:(NSArray*)devices;

#pragma mark--  摄像机状态回调
//- (void) PPPPStatus: (NSString*)strDID statusType:(PPPP_STATUS_TYPE)statusType status:(PPPP_STATUS_VALUE) status;
- (void) PPPPMsgNotify: (NSString*)strDID notifyType:(PPPP_MSG_NOTIFY_TYPE)notifyType status:(PPPP_STATUS_VALUE) status;

#pragma mark-- 视频数据回调
/*
 *视频数据回调，视频数据为H264数据流
 */
- (void) H264Data: (Byte*) h264Frame videoInfo:(PIXVideoInfo*)videInfo DID:(NSString *)did;

/**
 *  录像的视频数据回调
 *
 *  @param did      did
 *  @param h264Data h264Data
 *  @param bytes    bytes
 *  @param keyframe keyframe
 */
-(void) recordH264DataBack:(NSString *)did h264Data:(char *)h264Data byte:(long)bytes keyframe:(int)keyframe timezone:(int)timezone timestamp:(long long )timestamp;

/**
 *  录像的音频数据回调
 *
 *  @param did   did
 *  @param data  raw data,g726 data
 *  @param bytes bytes
 */
-(void)recordAudioDataBack:(NSString *)did audioData:(char *)data byte:(long)bytes length:(int)length;


#pragma mark--事件回放视频数据回调
-(void)playbackH264DataBack:(NSString *)did h264Data:(char *)h264Data length:(int)length timeStamp:(int)timeStamp;

//#pragma mark--音频数据
//-(void)AudioDataBack:(Byte*)data Length:(int)len;
//
//#pragma mark--对讲时麦克风采集的数据
//-(void)TalkMicDataBack:(Byte*)data length:(int)length;
//
//-(void)recordH264DataBack:(NSString *)did data:(char *)h264Data length:(int) length keyframe:( int) keyframe;
//


#pragma mark-- WIFI列表数据
/*
 *wifilist 包含PIXWIFInfo 对象
 */
-(void)WIFIListDataBack:(NSString*)strDID wifiList:(NSArray*)wifilist;

#pragma mark--高级设置信息返回
-(void)fetchAdvanceSettingDataBack:(NSString *)strDid setting:(PIXAdvanceSetting *)advanceSetting;

#pragma mark--设置音量回调-扬声器
// 0: success; otherwise: failed.
-(void)setSpeakerVolumeBack:(int)resultCode;

#pragma mark--设置音量回调-麦克风
// 0: success; otherwise: failed.
-(void)setMicrophoneVolumeBack:(int)resultCode;

#pragma mark--设置时区返回
//0-success;-1:failue
-(void)setGMTBack:(int)resultCode;

#pragma mark--设置时区返回  V1
-(void)setTimeZoneBack:(int)resultCode;


#pragma mark--对码
////endFlag 当为1时表示传完,0表示后面还有数据
-(void)matchCodeBack:(int)endFlag matchInfo:(PIXSensorInfo *)info codeNum:(MatchCodeNum)codeNum;

//传感器操作
#pragma mark--获取传感器列表回调
////endFlag 当为1时表示传完,0表示后面还有数据
-(void)fetchSensorListBack:(int) endFlag sensorInfo:(PIXSensorInfo *)sensorInfo;
#pragma mark--传感器修改、删除、添加回调
//0-success;-1:failue   command:(0、1、4、5、6);
-(void)modifySensorBack:(SensorEditType)command resultCode:(int)resultCode;


//防区操作
#pragma mark--获取防区列表回调
////endFlag 当为1时表示传完,0表示后面还有数据
-(void)fetchAlarmAreaInfoListBack:(int)endFlag alarmAreaInfo:(PIXAlarmAreaInfo *)areaInfo;
#pragma mark--操作防区：添加、修改、删除
-(void)modifyAlarmAreaInfoBack:(SensorEditType)command resultCode:(int)resultCode;


#pragma mark--防区操作  添加、删除传感器、删除防区
-(void)editAlarmAreaSensorListBack:(SensorEditType)command resultCode:(int)resultCode;

#pragma mark--获取防区布防、撤防状态回调
-(void)fetchGlobalAlarmEnableBack:(int)resultCode enable:(int)enable;
#pragma mark--设置防区布防、撤防状态回调   无论设置成功与否，都会返回防区状态
-(void)setGlobalAlarmEnableBack:(int)resultCode enable:(int)enable;

#pragma mark--设置红外报警回调
-(void)setInfraredAlertModeBack:(int)resultCode;

#pragma mark--设置图像翻转
//0-success;-1:failue
-(void)setVideoFlipModeBack:(int)resultCode;

#pragma makr--ir setting 设置红外模式
-(void)setIRModeBack:(int) resultCode;

#pragma mark--设置环境模式
-(void)setEnvModeBack:(int) resultCode;

#pragma mark-设置水印时钟
-(void)setOSDEnableBack:(int)resultCode;

#pragma mark--设置移动侦测
-(void)setMotionSensitivityBack:(int)resultCode;

#pragma mark--设置事件通知
-(void)setAlarmModeBack:(int)resultCode;

#pragma mark--设置录像模式
-(void)setRecordModeBack:(int)resultCode;

#pragma mark--格式化存储
-(void)formatExtStorageBack:(int)resultCode;

#pragma mark--设置wifi
//0: wifi connected; 1: failed to connect
-(void)setWifiBack:(int)resultCode;

#pragma mark--修改摄像机登录密码
//0: success; otherwise: failed.
-(void)setPasswordBack:(int)resultCode;

#pragma mark--获取回放事件回调
//endFlag = 1 means this package is the last one.
-(void)fetchEventListBack:(int)endFlag eventInfo:(PIXEventRespInfo *)info;
//event list one package complete.   endFlag = 1 means this package is the last one.
-(void)fetchEventPackageBack:(int)endFlag eventCountInPackage:(int)count;

#pragma mark--事件回放命令返回，包含各子命令
-(void)playbackCommandBack:(RECORD_PLAY_COMMAND)commandCode resultCode:(int)resultCode;

#pragma mark--设置红外灯模式
-(void)setIRLedModeBack:(int)resultCode;

#pragma mark--获取录像时间，timeSelectArray是bool值数组,是一个一维数组，存放顺序为1~7:(六五四三二一天的00:00~00:30)、9~15:(六五四三二一天的00:30~01:00)，以此类推
-(void)fetchRecordTimeBack:(int)recordType timeSelectArray:(NSArray *)timeSelectArray;

#pragma mark--设置录像时间返回
-(void)setRecordTimeBack:(int)resultCode;

#pragma mark--收到摄像机推送信息
-(void)receivePushEventReport:(PIXPushEventInfo *)pushEvent;

#pragma mark--收到传感器报警事件
-(void)receiveSensorAlarmEventReport:(PIXSensorAlarmInfo *)alarmInfo;
@end


@protocol PCM2G726EncodeDelegate <NSObject>
/**
 *  PCM格式数据压缩成G726格式数据
 *
 *  @param pcmData xx
 *  @param len
 *
 *  @return
 */
-(int) PCM2G726:(char *)pcmData dataSize:(unsigned int) len;
-(const unsigned char*)GetG726Data;
@end

@protocol G7262PCMDecodeDelegate <NSObject>
/**
 *  G726格式数据解压成PCM格式数据
 *
 *  @param g726Data
 *  @param len
 *
 *  @return
 */
-(int) G7262PCM:(const unsigned char *)g726Data dataSize:(unsigned int) len;

-(const unsigned char*)GetPCMData  ;
@end



