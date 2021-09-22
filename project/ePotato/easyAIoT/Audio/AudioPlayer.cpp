#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#include <file.h>
#include <cstring.h>
#include <Windows.h>
#include <stdio.h>
#pragma comment(lib, "winmm.lib")

void pcmPlay(const char *pathname)
{
    const int buf_size = 1024 * 1024 * 30;

    cstring_t *pFile = readFile(pathname);

    WAVEFORMATEX wfx = {0};
    wfx.wFormatTag = WAVE_FORMAT_PCM;   //设置波形声音的格式
    wfx.nChannels = 1;                  //设置音频文件的通道数量
    wfx.nSamplesPerSec = 16000;         //设置每个声道播放和记录时的样本频率
    wfx.wBitsPerSample = 16;            //每隔采样点所占的大小
 
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;
  
    HANDLE wait = CreateEvent(NULL, 0, 0, NULL);
    HWAVEOUT hwo;
    waveOutOpen(&hwo, WAVE_MAPPER, &wfx, (DWORD_PTR)wait, 0L, CALLBACK_EVENT); //打开一个给定的波形音频输出装置来进行回放
 
    int data_size = 20480;
    char* data_ptr = pFile->str;
    WAVEHDR wh;
 
    while (data_ptr - pFile->str < pFile->len)
    {
        //这一部分需要特别注意的是在循环回来之后不能花太长的时间去做读取数据之类的工作，不然在每个循环的间隙会有“哒哒”的噪音
        wh.lpData = data_ptr;
        wh.dwBufferLength = data_size;
        wh.dwFlags = 0L;
        wh.dwLoops = 1L;
 
        data_ptr += data_size;
 
        waveOutPrepareHeader(hwo, &wh, sizeof(WAVEHDR)); //准备一个波形数据块用于播放
        waveOutWrite(hwo, &wh, sizeof(WAVEHDR)); //在音频媒体中播放第二个函数wh指定的数据
 
        WaitForSingleObject(wait, INFINITE); //等待
    }
    waveOutClose(hwo);
    CloseHandle(wait);
    cstring_del(pFile);
}
#elif defined(LINUX) || defined(__unix__)
void pcmPlay(const char *pathname)
{
    const char *format = "aplay -r16000 -c1 -f S16_LE %s";
    int len = strlen(format) + strlen(pathname);
    char *cmd = (char *)malloc(len);
    if (!cmd) return;
    sprintf(cmd, format, pathname);
    system(cmd);
    if (cmd) free(cmd);
}
#elif defined(__CONFIG_CHIP_XR872) && defined(__CONFIG_OS_FREERTOS)
#include <EILog.h>
#include "kernel/os/os.h"
#include "common/framework/fs_ctrl.h"
#include "common/apps/player_app.h"
#include "fs/fatfs/ff.h"
#include "audio/pcm/audio_pcm.h"
#include "audio/manager/audio_manager.h"

#define PLAYER_THREAD_STACK_SIZE    (1024 * 4)
static OS_Thread_t play_thread;
static player_base *player;
static int isCompleted = 0;
static int play_pcm_music(const char *pathname)
{
    FIL fp;
    int ret;
    FRESULT result;
    unsigned int act_read;
    unsigned int pcm_buf_size;
    void *pcm_buf;
    struct pcm_config config;

    result = f_open(&fp, pathname, FA_OPEN_EXISTING | FA_READ);
    if (result != FR_OK) {
        printf("open pcm file fail\n");
        return -1;
    }

    config.channels = 1;
    config.format = PCM_FORMAT_S16_LE;
    config.period_count = 2;
    config.period_size = 1024;
    config.rate = 16000;
    ret = snd_pcm_open(AUDIO_SND_CARD_DEFAULT, PCM_OUT, &config);
    if (ret != 0) {
        goto err1;
    }

    pcm_buf_size = config.channels * config.period_count * config.period_size;
    pcm_buf = malloc(pcm_buf_size);
    if (pcm_buf == NULL) {
        goto err2;
    }

    while (1) {
        result = f_read(&fp, pcm_buf, pcm_buf_size, &act_read);
        if (result != FR_OK) {
            printf("read fail.\n");
            break;
        }

        snd_pcm_write(AUDIO_SND_CARD_DEFAULT, pcm_buf, act_read);

        if (act_read != pcm_buf_size) {
            printf("reach file end\n");
            break;
        }
    }

    free(pcm_buf);
    snd_pcm_flush(AUDIO_SND_CARD_DEFAULT);
    snd_pcm_close(AUDIO_SND_CARD_DEFAULT, PCM_OUT);
    f_close(&fp);

    return 0;

err2:
    snd_pcm_close(AUDIO_SND_CARD_DEFAULT, PCM_OUT);
err1:
    f_close(&fp);
    return -1;
}
static void play_task(void *arg)
{
    const char *pathname = (const char *)arg;

    if (fs_ctrl_mount(FS_MNT_DEV_TYPE_SDCARD, 0) != 0) {
        printf("mount fail\n");
        goto exit;
    }

    player = player_create();
    if (player == NULL) {
        printf("player create fail.\n");
        goto exit;
    }

    printf("player create success.\n");
    printf("you can use it to play, pause, resume, set volume and so on.\n");

    printf("player set volume to 8. valid volume value is from 0~31\n");
    player->setvol(player, 24);

    while (1) {
        LOG(EDEBUG, "===try to play pcm by audio driver===");
        play_pcm_music(pathname);
    }

    player_destroy(player);

exit:
    OS_ThreadDelete(&play_thread);
}
void pcmPlay(const char *pathname)
{
    if (OS_ThreadCreate(&play_thread,
                        "play_task",
                        play_task,
                        (void *)pathname,
                        OS_THREAD_PRIO_APP,
                        PLAYER_THREAD_STACK_SIZE) != OS_OK) {
        LOG(EERROR, "thread create fail.exit");
    }
}
#else
#error "pcm play not support on this platform."
#endif

#ifdef __cplusplus
}
#endif