//
//  AudioPlayer.m
//  P4PCamLive
//
//  Created by Xiaol on 15/6/26.
//  Copyright (c) 2015年 Mutual Mobile. All rights reserved.
//


#include "AudioPlayer.h"
#import "PIXDeviceManagement.h"
#import "G726Decoder.h"
#include "defineutility.h"
#include <pthread.h>

#define bufferByteSize 320*2
#define AUDIO_DATA_BUFF_SIZE bufferByteSize*1024

AudioPlayer::AudioPlayer(){
    m_lock = [[NSCondition alloc] init];
    m_bthreadRunning = 0;
    g726DecodeHander = nil;
    m_audioDataPuf = new CCircleBuf();

}

AudioPlayer::~AudioPlayer()
{
    StopPlay();
    delete m_audioDataPuf;
}



bool AudioPlayer::initialized()
{
   
    for (int i = 0; i<AUDIO_PLAY_NUM_BUFFERS; ++i) {
        m_inUsed[i] = false;
    }

    m_nPos = 0;
    
    m_dataFormat.mSampleRate=8000;
    m_dataFormat.mFormatID=kAudioFormatLinearPCM;
    m_dataFormat.mFormatFlags=kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    m_dataFormat.mBytesPerFrame=2;
    m_dataFormat.mBytesPerPacket=2;
    m_dataFormat.mFramesPerPacket=1;//
    m_dataFormat.mChannelsPerFrame=1;//
    m_dataFormat.mBitsPerChannel=16;//
    m_dataFormat.mReserved=0;
    
    
    OSStatus err = AudioQueueNewOutput(&m_dataFormat, audioQueueOutputCallback, this, NULL, NULL, 0, &m_queue);
    if (err) {
        return false;
    }
    
    for (int i=0; i<AUDIO_PLAY_NUM_BUFFERS; i++) {
        AudioQueueAllocateBuffer(m_queue, bufferByteSize, &m_buffers[i]);
        //readPacketsIntoBuffer(m_buffers[i]);
        
    }
    
    return true;
}
int AudioPlayer::findQueueBuffer(AudioQueueBufferRef inBuffer)
{

    
    for (unsigned int i = 0; i < AUDIO_PLAY_NUM_BUFFERS; ++i) {
        if (inBuffer == m_buffers[i]) {
            return i;
        }
    }
    return -1;
}

int AudioPlayer::findUnusedBuffer(){
    
    int index = m_nPos % AUDIO_PLAY_NUM_BUFFERS;
    if (!m_inUsed[index]) {
        
        m_nPos = index+1;
        if (index == (AUDIO_PLAY_NUM_BUFFERS/2-1)) { //在将最后一个音频队列缓冲区入队之后调用，来确保所有缓存过的数据，也包括处理的中间数据，得到录制或播放
            
            if (m_queue != NULL) {
                AudioQueueFlush(m_queue);
            }
            
        }

        return index;
    }
    if (m_queue != NULL) {
        AudioQueueFlush(m_queue);
    }
    return -1;
    
}


void AudioPlayer::audioQueueOutputCallback(void *inClientData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer)
{

    AudioPlayer *audioPlay = static_cast<AudioPlayer*>(inClientData);
    [audioPlay->m_lock lock];
    int bufIndex = audioPlay->findQueueBuffer(inBuffer);
    if (bufIndex < 0 || bufIndex >= AUDIO_PLAY_NUM_BUFFERS) {
        [audioPlay->m_lock unlock];
        return;
    }
    audioPlay->m_inUsed[bufIndex] = false;

    [audioPlay->m_lock unlock];

}

bool AudioPlayer::enqueueAudioData(unsigned char * audiaData ,int length){
    [this->m_lock lock];

    int buffIndex = findUnusedBuffer();
    if (buffIndex < 0 || buffIndex >= AUDIO_PLAY_NUM_BUFFERS) {
        PLogError(@"音频数据加入队列失败");
        [this->m_lock unlock];

        return false;
    }
    AudioQueueBufferRef buffer = m_buffers[buffIndex];
    
    if (length <= 0) {
        memset(buffer->mAudioData, 0, bufferByteSize);
        buffer->mAudioDataByteSize = bufferByteSize;
    }else {
        memcpy(buffer->mAudioData, audiaData, length);
        buffer->mAudioDataByteSize = length;
    }
//    buffer->mAudioDataByteSize = length;
    
    // 完成给队列配置缓存的处理
   OSStatus err = AudioQueueEnqueueBuffer(m_queue, buffer, 0, NULL);
    if (err) {
        [this->m_lock unlock];
        return false;
    }
    m_inUsed[buffIndex] = YES;
    [this->m_lock unlock];

    return true;
}


bool AudioPlayer::StartPlayAudio()
{
    if (m_bPlaying == 1) {
        return true;
    }
    
    if (!initialized()) {
        PLogInfo(@"initialzed failed");
        return false;
    }
    //直接调用AudioQueueStart会自动开始解码
   OSStatus err =  AudioQueueStart(m_queue, NULL);
    if (err) {
        return false;
    }
    m_bPlaying = 1;
    /*为了保证播放流畅性可以用线程*/
    //StartThread();
    return true;
}
 

/*
void AudioPlayer::StartPlay()
{
    if (m_bPlaying == 1) {
        return;
    }
    //直接调用AudioQueueStart会自动开始解码
    AudioQueueStart(m_queue, NULL);
    m_bPlaying = 1;
}
 */

void AudioPlayer::StopPlay()
{
    if (m_bPlaying == 0) {
        return;
    }
    
    if (m_queue !=NULL) {
        AudioQueueFlush(m_queue);

        AudioQueueStop(m_queue, YES);
        for (int i=0; i<AUDIO_PLAY_NUM_BUFFERS; i++) {
            AudioQueueFreeBuffer(m_queue, m_buffers[i]);
        }
        
        AudioQueueDispose(m_queue, YES);
    }
    StopThread();
    m_bPlaying = 0;
}

//填充静音数据
int AudioPlayer::readPacketsIntoBuffer(AudioQueueBufferRef buffer)
{
    UInt32 numBytes;
    numBytes = bufferByteSize;
    AudioQueueBufferRef outBufferRef = buffer;
    memset(outBufferRef->mAudioData, 0, numBytes);
    outBufferRef->mAudioDataByteSize = numBytes;
    AudioQueueEnqueueBuffer(m_queue, outBufferRef, 0, nil);
    return 1;
}


void AudioPlayer::writeG726Data(unsigned char * g726Data ,int length){
    if (g726DecodeHander == nil) {
        PLogError(@"faild: dont set g726Dncoder....");
        return;
    }
    int size = [g726DecodeHander G7262PCM:(const unsigned char *)g726Data dataSize:length];
//    PLogInfo(@"play audio data %d", size);
    enqueueAudioData((unsigned char *)[g726DecodeHander GetPCMData], size);

    //m_audioDataPuf->Write((void*)[g726DecodeHander GetData], size);
}


/*
 *为了保证播放的流畅性，用缓存进行数据播放
 */
void AudioPlayer::PlayProcess(){
    //从缓冲区取音频数据
    char *pcmData =   new char[bufferByteSize+1];
    PLogInfo(@"AudioPlayer thread started..");
    while(m_bthreadRunning)
    {
        
        if (m_audioDataPuf->GetStock() < bufferByteSize) {
            usleep(10000);
            continue;
            
        }
        /*获取PCM数据*/
        m_audioDataPuf->Read(pcmData, bufferByteSize);
        enqueueAudioData((unsigned char *)pcmData, bufferByteSize);

        
    }
    delete[] pcmData;
    
    PLogInfo(@"AudioPlayer thread end..");
}


void* AudioPlayer::playThread(void* param){
    AudioPlayer *player = (AudioPlayer*)param;
    player->PlayProcess();
    return NULL;
}

int AudioPlayer::StartThread()
{
    m_bthreadRunning = 1;
    m_audioDataPuf->Create(AUDIO_DATA_BUFF_SIZE);
    pthread_create(&m_threadID, NULL, playThread, (void*)this);
    return 1;
    
}
void AudioPlayer::StopThread()
{
    m_bthreadRunning=0;
    if (m_threadID != NULL) {
        pthread_join(m_threadID, NULL);
        m_threadID = NULL;
        m_audioDataPuf->Release();
        
    }
}

void AudioPlayer::SetG727DecoderHander(id<G7262PCMDecodeDelegate> hander){
    [((NSObject*)hander) retain];
    if(g726DecodeHander != nil){
        [g726DecodeHander release];
    }
    g726DecodeHander = hander;
    
}



