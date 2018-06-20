//
//  AudioPlayer.h
//  P4PCamLive
//
//  Created by Xiaol on 15/6/26.
//  Copyright (c) 2015年 Mutual Mobile. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "PIXDeviceManagement.h"
#import <AudioToolbox/AudioToolbox.h>
#import <AudioToolbox/AudioFile.h>
#include <AudioToolbox/AudioQueue.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include "CircleBuf.h"
#import "PIXDeviceProtocol.h"



#define AUDIO_PLAY_NUM_BUFFERS 16

class AudioPlayer
{

public:
    AudioPlayer();
    bool initialized();
    virtual ~AudioPlayer();
    void writeG726Data(unsigned char * g726Data ,int length);
    void StopPlay();
    
    bool StartPlayAudio();
//    void StartPlay();
    void SetG727DecoderHander(id<G7262PCMDecodeDelegate> hander);
  
private:
    
    NSCondition *m_lock;
    bool enqueueAudioData(unsigned char * AudiaData ,int length);

    void ProcessFillAudioBuf(AudioQueueRef inAQ, AudioQueueBufferRef buffer);
    
    int readPacketsIntoBuffer(AudioQueueBufferRef buffer);
    static void audioQueueOutputCallback(void *inClientData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer);
    int findQueueBuffer(AudioQueueBufferRef inBuffer);
    int findUnusedBuffer();
    void FillBuffer(AudioQueueRef queue,AudioQueueBufferRef buffer);
    static void* playThread(void* param);
    void PlayProcess();
    int StartThread();
    void StopThread();
private:
    AudioStreamBasicDescription m_dataFormat;
    AudioQueueRef m_queue;
    AudioStreamPacketDescription *m_packetDescs;
    AudioQueueBufferRef m_buffers[AUDIO_PLAY_NUM_BUFFERS];
    bool m_inUsed[AUDIO_PLAY_NUM_BUFFERS];
    CCircleBuf *                m_audioDataPuf;

    int m_bPlaying;
    int m_nPos;
    
    Byte *audioByte;
    long audioDataIndex;
    UInt32 audioDataLength;
    pthread_t m_threadID;
    bool m_bthreadRunning;
    id<G7262PCMDecodeDelegate> g726DecodeHander;

    
};
