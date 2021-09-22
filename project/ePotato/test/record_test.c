#include "EILog.h"
#include "EIHttpClient.h"
#include "wlan/wlan.h"
#include "wlan/wlan_defs.h"
#include "common/framework/fs_ctrl.h"
#include "fs/fatfs/ff.h"
#include "common/apps/recorder_app.h"
#include "kernel/os/os_time.h"
#include "audio/pcm/audio_pcm.h"
#include "audio/manager/audio_manager.h"
#include "AudioRecorder.h"

#include "record_test.h"

#define PLAYER_THREAD_STACK_SIZE    (1024 * 2)

static OS_Thread_t recordThread;

void recordTestThread(void *arg)
{
    recorder_base *recorder;
    rec_cfg cfg;

    // 配置打开录音
    AudioConfig_t stAudioConfig = {16000, 16, 1, NULL, NULL};
    Recorder.open(&stAudioConfig);

    // 开始录音
    Recorder.start();
    OS_Sleep(15);

    // 停止录音
    Recorder.stop();
    // 释放资源
    Recorder.release();

    OS_ThreadDelete(&recordThread);
}

void recordTest()
{
    if (fs_ctrl_mount(FS_MNT_DEV_TYPE_SDCARD, 0) != 0) {
        LOG(EDEBUG, "mount fail");
        return ;
    }

    LOG(EDEBUG, "audio record start.");

    /* set record volume */
    audio_manager_handler(AUDIO_SND_CARD_DEFAULT, AUDIO_MANAGER_SET_VOLUME_LEVEL, AUDIO_IN_DEV_AMIC, 3);

    if (OS_ThreadCreate(&recordThread,
                        "recordTestThread",
                        recordTestThread,
                        NULL,
                        OS_THREAD_PRIO_APP,
                        PLAYER_THREAD_STACK_SIZE) != OS_OK) {
        LOG(EERROR, "thread create fail.exit");
    }
}