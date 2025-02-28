#include <date.h>
#include <EILog.h>
#include <cstring.h>
#include <wsclient.h>
#include <SimpleThread.h>
#include <AudioRecorder.h>
#include <iFlyAuth.h>
#include <base64.h>
#include <cJSON_user_define.h>
#include <APPConfig.h>

#include "igr.h"

#define IGR_URL "ws://ws-api.xfyun.cn/v2/igr"

typedef enum EFRAME_TYPE
{
    EFRAME_FIRST = 0,
    EFRAME_CONTINE,
    EFRAME_LAST
} EFRAME_TYPE;

static void vFirstRequest(const char *pAudio, char *pszRequest, int iLen)
{
    JSON_SERIALIZE_CREATE_OBJECT_START(json_common_obj);
    JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_common_obj, "app_id", appconfig.appid);
    // JSON_SERIALIZE_CREATE_END(json_common_obj);

    JSON_SERIALIZE_CREATE_OBJECT_START(json_business_obj);
    JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_business_obj, "aue", "raw");
    JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_business_obj, "rate", "16000");
    // JSON_SERIALIZE_CREATE_END(json_business_obj);

    JSON_SERIALIZE_CREATE_OBJECT_START(json_data_obj);
    JSON_SERIALIZE_ADD_INT_TO_OBJECT(json_data_obj, "status", EFRAME_FIRST);
    JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_data_obj, "audio", pAudio);
    // JSON_SERIALIZE_CREATE_END(json_data_obj);

    JSON_SERIALIZE_CREATE_OBJECT_START(json_frame_obj);
    JSON_SERIALIZE_ADD_OBJECT_TO_OBJECT(json_frame_obj, "common", json_common_obj);
    JSON_SERIALIZE_ADD_OBJECT_TO_OBJECT(json_frame_obj, "business", json_business_obj);
    JSON_SERIALIZE_ADD_OBJECT_TO_OBJECT(json_frame_obj, "data", json_data_obj);
    JSON_SERIALIZE_STRING(json_frame_obj, pszRequest, iLen);
    JSON_SERIALIZE_CREATE_END(json_frame_obj);
}

static void vContinueRequest(const char *pAudio, char *pszRequest, int iLen)
{
    // 构造data中的字段
    JSON_SERIALIZE_CREATE_OBJECT_START(json_data_obj);
    JSON_SERIALIZE_ADD_INT_TO_OBJECT(json_data_obj, "status", EFRAME_CONTINE);
    // JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_data_obj, "format", "audio/L16;rate=16000");
    // JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_data_obj, "encoding", "raw");
    JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_data_obj, "audio", pAudio);

    // 把以上构造的对象json_data_obj放到data所表述的json_frame_obj中
    JSON_SERIALIZE_CREATE_OBJECT_START(json_frame_obj);
    JSON_SERIALIZE_ADD_OBJECT_TO_OBJECT(json_frame_obj, "data", json_data_obj);
    // 把json对象序列化到pszRequest数组中，用于后续的发送
    JSON_SERIALIZE_STRING(json_frame_obj, pszRequest, iLen);
    JSON_SERIALIZE_CREATE_END(json_frame_obj);
}

static void vLastRequest(const char *pAudio, char *pszRequest, int iLen)
{
    JSON_SERIALIZE_CREATE_OBJECT_START(json_data_obj);
    JSON_SERIALIZE_ADD_INT_TO_OBJECT(json_data_obj, "status", EFRAME_LAST);
    // JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_data_obj, "format", "audio/L16;rate=16000");
    // JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_data_obj, "encoding", "raw");
    JSON_SERIALIZE_ADD_STRING_TO_OBJECT(json_data_obj, "audio", pAudio);
    // JSON_SERIALIZE_CREATE_END(json_data_obj);

    JSON_SERIALIZE_CREATE_OBJECT_START(json_frame_obj);
    JSON_SERIALIZE_ADD_OBJECT_TO_OBJECT(json_frame_obj, "data", json_data_obj);
    JSON_SERIALIZE_STRING(json_frame_obj, pszRequest, iLen);
    JSON_SERIALIZE_CREATE_END(json_frame_obj);
}

static int iGetResponse(const char *pszData, int iSize)
{
    int ret, code;
    char message[1024];
    char sid[1024];
    int status = 0;
    char result[128];
    JSON_DESERIALIZE_START(json_root, pszData, ret);
        JSON_DESERIALIZE_GET_INT(json_root, "code", code, ret, JSON_CTRL_BREAK);
        JSON_DESERIALIZE_GET_STRING_COPY(json_root, "message", message, sizeof(message), ret, JSON_CTRL_NULL);
        JSON_DESERIALIZE_GET_STRING_COPY(json_root, "sid", sid, sizeof(sid), ret, JSON_CTRL_NULL);
        // 1. 解析data对象，不存在就break出去，不再解析
        JSON_DESERIALIZE_GET_OBJECT(json_root, "data", json_data, ret, JSON_CTRL_BREAK);
        // 2. 解析data中的result对象，不存在就break出去，不再解析
        JSON_DESERIALIZE_GET_OBJECT(json_data, "result", json_result, ret, JSON_CTRL_BREAK);
        // 3. 解析result中的age对象，不存在就break出去
        JSON_DESERIALIZE_GET_OBJECT(json_result, "age", json_age, ret, JSON_CTRL_BREAK);
        JSON_DESERIALIZE_GET_STRING_COPY(json_age, "age_type", result, sizeof(result), ret, JSON_CTRL_NULL);
        LOG(EDEBUG, "age_type:%s", result);
        JSON_DESERIALIZE_GET_STRING_COPY(json_age, "child", result, sizeof(result), ret, JSON_CTRL_NULL);
        LOG(EDEBUG, "child:%s", result);
        JSON_DESERIALIZE_GET_STRING_COPY(json_age, "middle", result, sizeof(result), ret, JSON_CTRL_NULL);
        LOG(EDEBUG, "middle:%s", result);
        JSON_DESERIALIZE_GET_STRING_COPY(json_age, "old", result, sizeof(result), ret, JSON_CTRL_NULL);
        LOG(EDEBUG, "old:%s", result);
        // 4. 解析ws数组中的gender对象，不存在就break出去
        JSON_DESERIALIZE_GET_OBJECT(json_result, "gender", json_gender, ret, JSON_CTRL_BREAK);
        JSON_DESERIALIZE_GET_STRING_COPY(json_gender, "female", result, sizeof(result), ret, JSON_CTRL_NULL);
        LOG(EDEBUG, "female:%s", result);
        JSON_DESERIALIZE_GET_STRING_COPY(json_gender, "gender_type", result, sizeof(result), ret, JSON_CTRL_NULL);
        LOG(EDEBUG, "gender_type:%s", result);
        JSON_DESERIALIZE_GET_STRING_COPY(json_gender, "male", result, sizeof(result), ret, JSON_CTRL_NULL);
        LOG(EDEBUG, "male:%s", result);
    JSON_DESERIALIZE_END(json_root, ret);

    LOG(ETRACE, "code:%d", code);

    if (ret < 0) return ret;
    return status;
}

static void RecordCB(void *pvHandle, int32_t iType, void *pvUserData, void *pvData, int32_t iLen)
{
    static char sBase64Buffer[8192] = {0};
    static char szRequest[8192] = {0};
    static int nTotalSize = 0;
    memset(sBase64Buffer, 0, sizeof(sBase64Buffer));
    memset(szRequest, 0, sizeof(szRequest));

    SSockClient_t *pstSock = (SSockClient_t *)pvUserData;
    if (!pstSock)
        return;
    static int iStatus = EFRAME_FIRST;
    switch (iType)
    {
    case AUDIO_OPEN:
        LOG(EDEBUG, "record device open success");
        break;
    case AUDIO_DATA:
        nTotalSize += iLen;
        iBase64Encode(pvData, sBase64Buffer, iLen);
        // LOG(EDEBUG, "get audio data, len:%d,%s", iLen, sBase64Buffer);
        switch (iStatus)
        {
        case EFRAME_FIRST:
            vFirstRequest(sBase64Buffer, szRequest, sizeof(szRequest) - 1);
            iStatus = EFRAME_CONTINE;
            break;
        case EFRAME_CONTINE:
            vContinueRequest(sBase64Buffer, szRequest, sizeof(szRequest) - 1);
            if (nTotalSize >= 5*32000) {
                nTotalSize = 0;
                iStatus = EFRAME_LAST;
            }
            break;
        case EFRAME_LAST:
            vLastRequest(sBase64Buffer, szRequest, sizeof(szRequest) - 1);
            Recorder.stop();
            break;
        default:
            break;
        }
        iWSSend(pstSock, szRequest, strlen(szRequest), EEIWS_OPCODE_TXTDATA);
        break;
    case AUDIO_CLOSE:
        Recorder.stop();
        break;
    default:
        break;
    }
}

static thread_ret_t pvRealRecordThread(void *params)
{
    // 录音开始，录音数据存放在ringbuffer，这些操作在回调函数RecordCB完成
    AudioConfig_t stAudioConfig = {16000, 16, 1, RecordCB, (void *)params};
    Recorder.open(&stAudioConfig);
    Recorder.start();

    // 释放资源
    Recorder.release();
    LOG(EDEBUG, "record thread exit");
}

static bool bWSCallback(SSockClient_t *pstSock, void* pUserData, EEIWS_MESSAGE eType, void *pvMessage, int iLen)
{
    int iRet = 0;
    bool bEnd = false;
    switch (eType)
    {
    case EEIWS_ON_HAND_SHAKE: // 收到了101消息，websocket服务握手成功
        LOG(EDEBUG, "websocket handshake success");
        ThreadFun funArr[1];
        funArr[0].fun = pvRealRecordThread;
        funArr[0].params = (void *)pstSock;
        vStartThread(1, funArr);

        break;
    case EEIWS_ON_MESSAGE: // 收到websocket消息
        iRet = iGetResponse(pvMessage, iLen);
        if (2 == iRet) {
            // LOG(EDEBUG, "response finish");
            // Recorder.close();
            // bEnd = true;
        } else if (iRet < 0) {
            LOG(EDEBUG, "len:%d, recv:%s", iLen, (char *)pvMessage);
            // Recorder.close();
            // bEnd = true;
        } else {
            // LOG(EDEBUG, "msg:%s", pvMessage);
        }
        break;
    case EEIWS_ON_CLOSE: // 断开websocket连接
        LOG(EDEBUG, "websocket close");
        break;
    case EEIWS_ON_ERROR: // websocket错误信息
        LOG(EDEBUG, "websocket error, errordetail:%s", (char *)pvMessage);
        break;
    default:
        break;
    }
    return bEnd;
}

void igr(const char *appid, const char *key, const char *secret)
{
    char szFullUrl[4096];
    char szAuth[1024], szDate[64];
    char *szBaseUrl = "ws://ws-api.xfyun.cn/v2/igr?host=%s&date=%s&authorization=%s";

    strcpy(appconfig.appid, appid);
    strcpy(appconfig.appkey, key);
    strcpy(appconfig.appsecret, secret);

    // 1. 构造websocket auth字段
    datetime.format("GMT", szDate, sizeof(szDate));
    vGetAuth(key, secret, "ws-api.xfyun.cn",
             "GET /v2/igr HTTP/1.1", szDate, szAuth, sizeof(szAuth));
    LOG(EDEBUG, "auth:%s\n", szAuth);

    // 2. 构造完整的URL
    snprintf(szFullUrl, sizeof(szFullUrl), szBaseUrl, "ws-api.xfyun.cn", szDate, szAuth);
    LOG(EDEBUG, "url:%s", szFullUrl);

    // 3. 连接服务器
    bWebsocketConnect(szFullUrl, bWSCallback, NULL);
}
