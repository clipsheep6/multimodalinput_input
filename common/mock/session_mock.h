/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup Softbus
 * @{
 *
 * @brief Provides high-speed, secure communication between devices.
 *
 * This module implements unified distributed communication capability management between
 * nearby devices, and provides link-independent device discovery and transmission interfaces
 * to support service publishing and data transmission.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file session.h
 *
 * @brief Declares unified data transmission interfaces.
 *
 * This file provides data transmission capabilities, including creating and removing a session server,
 * opening and closing sessions, receiving data, and querying basic session information. \n
 * After multiple nearby devices are discovered and networked, these interfaces can be used to
 * transmit data across devices. \n
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef SESSION_MOCK_H
#define SESSION_MOCK_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief business type of session
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    TYPE_MESSAGE = 1,
    TYPE_BYTES,
    TYPE_FILE,
    TYPE_STREAM,
    TYPE_BUTT,
} SessionType;

typedef enum {
    INVALID = -1,
    /*
     * Send any segment of a frame each time.
     */
    RAW_STREAM,
    /*
     * Send a whole video frame each time.
     */
    COMMON_VIDEO_STREAM,
    /*
     * Send a whole audio frame each time.
     */
    COMMON_AUDIO_STREAM,
    /*
     * Slice frame mode.
     */
    VIDEO_SLICE_STREAM,
} StreamType;

typedef enum {
    LINK_TYPE_WIFI_WLAN_5G = 1,
    LINK_TYPE_WIFI_WLAN_2G = 2,
    LINK_TYPE_WIFI_P2P = 3,
    LINK_TYPE_BR = 4,
    LINK_TYPE_MAX = 4,
} LinkType;

typedef struct {
    /* @brief dataType{@link SessionType} */
    int dataType;
    int linkTypeNum;
    LinkType linkType[LINK_TYPE_MAX];
    union {
        struct StreamAttr {
            int streamType;
        } streamAttr;
    } attr;
} SessionAttribute;

typedef struct {
    char *buf;
    int bufLen;
} StreamData;

typedef struct {
    int type;
    int64_t value;
} TV;

typedef struct {
    int frameType;
    int64_t timeStamp;
    int seqNum;
    int seqSubNum;
    int level;
    int bitMap;
    int tvCount;
    TV *tvList;
} FrameInfo;

enum FrameType {
    NONE,
    VIDEO_I,
    VIDEO_P,
    VIDEO_MAX = 50,
    RADIO = VIDEO_MAX + 1,
    RADIO_MAX = 100,
};

// APP should update StreamFrameInfo each time.
struct StreamFrameInfo {
    uint32_t streamId = 0;
    uint32_t seqNum = 0;
    uint32_t level = 0;
    FrameType frameType = NONE;
    uint32_t timestamp = 0;
    uint32_t bitrate = 0;
};

constexpr uint32_t DEVICE_ID_SIZE_MAX = 65;
constexpr uint32_t CHAR_ARRAY_SIZE = 100;

typedef struct {
    int (*onSessionOpened)(int sessionId, int result);
    void (*onSessionClosed)(int sessionId);
    void (*onBytesReceived)(int sessionId, const void *data, unsigned int dataLen);
    void (*onMessageReceived)(int sessionId, const void *data, unsigned int dataLen);
    void (*onStreamReceived)(int sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *param);
} ISessionListener;

typedef struct {
    int (*onReceiveFileStarted)(int sessionId, const char *files, int fileCnt);
    int (*onReceiveFileProcess)(int sessionId, const char *firstFile, uint64_t bytesUpload, uint64_t bytesTotal);
    void (*onReceiveFileFinished)(int sessionId, const char *files, int fileCnt);
    void (*onFileTransError)(int sessionId);
} IFileReceiveListener;

typedef struct {
    int (*onSendFileProcess)(int sessionId, uint64_t bytesUpload, uint64_t bytesTotal);
    int (*onSendFileFinished)(int sessionId, const char *firstFile);
    void (*onFileTransError)(int sessionId);
} IFileSendListener;

int CreateSessionServer(const char *pkgName, const char *sessionName, const ISessionListener *listener);

int RemoveSessionServer(const char *pkgName, const char *sessionName);

int OpenSession(const char *mySessionName, const char *peerSessionName, const char *peerDeviceId, const char *groupId,
    const SessionAttribute *attr);
void OpenSessionResult(void);

void CloseSession(int sessionId);

int SendBytes(int sessionId, const void *data, unsigned int len);

int SendMessage(int sessionId, const void *data, unsigned int len);

int SendStream(int sessionId, const StreamData *data, const StreamData *ext, const FrameInfo *param);

int GetMySessionName(int sessionId, char *sessionName, unsigned int len);

int GetPeerSessionName(int sessionId, char *sessionName, unsigned int len);

int GetPeerDeviceId(int sessionId, char *devId, unsigned int len);

int GetSessionSide(int sessionId);

int SetFileReceiveListener(const char *pkgName, const char *sessionName, const IFileReceiveListener *recvListener,
    const char *rootDir);

int SetFileSendListener(const char *pkgName, const char *sessionName, const IFileSendListener *sendListener);

int SendFile(int sessionId, const char *sFileList[], const char *dFileList[], uint32_t fileCnt);

#ifdef __cplusplus
}
#endif
#endif // SESSION_MOCK_H