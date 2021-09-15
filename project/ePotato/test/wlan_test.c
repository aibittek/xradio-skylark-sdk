#include "wlan_test.h"
#include "kernel/os/os.h"
#include "lwip/sockets.h"
#include "wlan/wlan.h"
#include "wlan/wlan_defs.h"
#include "console/log.h"

#define PLAYER_THREAD_STACK_SIZE    (1024 * 2)

static OS_Thread_t wlan_thread;

static void wlan_test_thread(void *arg)
{
    while (1) {
        wlan_speed_test();
    }

    OS_ThreadDelete(&wlan_thread);
}

void wlan_speed_test(void)
{
    int sockfd;
    char buf[1024];
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

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

    // 连接服务器
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_len = sizeof(addr);
    addr.sin_port = htons(9900);
    inet_aton("192.168.3.55", &addr.sin_addr);
    // addr.sin_addr.s_addr = inet_addr("192.168.3.55");

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0 )) < 0)
    {
        LOG(EERROR, "sockfd error detail:%s", strerror(errno));
        return ;
    }

    if(connect(sockfd, (const struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        LOG(EERROR, "sockfd error detail:%s", strerror(errno));
        goto error;
    }

    // 循环发送数据
    memset(buf, 'f', sizeof(buf));
    while(1) {
        send(sockfd, buf, sizeof(buf), 0);
    }

error:
    close(sockfd);
}

void wlan_test(void)
{
    if (OS_ThreadCreate(&wlan_thread,
                        "wlan_test_thread",
                        wlan_test_thread,
                        NULL,
                        OS_THREAD_PRIO_APP,
                        PLAYER_THREAD_STACK_SIZE) != OS_OK) {
        printf("thread create fail.exit\n");
    }
}