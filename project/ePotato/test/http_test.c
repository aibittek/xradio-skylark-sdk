#include "kernel/os/os.h"
#include "EILog.h"
#include "EIHttpClient.h"

#include "http_test.h"

#define PLAYER_THREAD_STACK_SIZE    (1024 * 2)

static OS_Thread_t httpThread;

void httpTestThread(void *arg)
{
    SEIHttpInfo_t stHttpInfo; 
    const char *pszUrl = "http://www.baidu.com";

    // 检测WiFi状态
    uint8_t ssid[] = "AIBIT"; 
    uint8_t ssid_len = strlen((char *)ssid);
    uint8_t psk[] = "75383573";
    wlan_sta_states_t state; 

    wlan_sta_set(ssid, ssid_len, psk);
    wlan_sta_enable();
    while(1) {
        wlan_sta_state(&state);
        if (WLAN_STA_STATE_CONNECTED == state) {
            LOG(EDEBUG, "wlan connected to %s", ssid);
            break;
        }
        OS_MSleep(1000);
    }
    
    // 连接HTTP服务器
    bConnectHttpServer(&stHttpInfo, pszUrl, NULL, NULL, 0);
    LOG(EDEBUG, "status:%d", stHttpInfo.stResponse.iStatus);
    LOG(EDEBUG, "body:%s", stHttpInfo.stResponse.pstBody->sBuffer);
    bHttpClose(&stHttpInfo);

    OS_ThreadDelete(&httpThread);
}

void httpTest(void)
{
    if (OS_ThreadCreate(&httpThread,
                        "httpTestThread",
                        httpTestThread,
                        NULL,
                        OS_THREAD_PRIO_APP,
                        PLAYER_THREAD_STACK_SIZE) != OS_OK) {
        LOG(EERROR, "thread create fail.exit\n");
    }
}