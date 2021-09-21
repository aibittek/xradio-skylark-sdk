#include <EILog.h>
#include "SimpleThread.h"

void vStartThread(int iThreadNum, ThreadFun funArray[])
{
#ifdef _WIN32
    int i;
    HANDLE handle[iThreadNum];
	for(i = 0; i < iThreadNum; i++) {
	   handle[i] = (HANDLE) _beginthreadex(NULL, 0, funArray[i].fun, funArray[i].params, 0, NULL);  
	}

    //等待线程结束
	//WaitForMultipleObjects(threadNum, handle, TRUE, INFINITE); 
    for(i = 0; i < iThreadNum; i++) {
		CloseHandle(handle[i]);    //关闭句柄 
	}
#elif defined(__unix__)

    int i;
    pthread_t pt[iThreadNum];
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    for(i = 0; i < iThreadNum; i++) {
    	pthread_create(&pt[i], &attr, funArray[i].fun, funArray[i].params); 
	}
    pthread_attr_destroy(&attr);

#elif defined(__CONFIG_CHIP_XR872) && defined(__CONFIG_OS_FREERTOS)
    int i;
    for(i = 0; i < iThreadNum; i++) {
        if (OS_ThreadCreate(&funArray[i].handler,
                            funArray[i].name,
                            funArray[i].fun,
                            funArray[i].params,
                            OS_THREAD_PRIO_APP,
                            funArray[i].stackSize) != OS_OK) {
            LOG(EDEBUG, "thread create fail.\n");
        }
	}

#else
#endif
}