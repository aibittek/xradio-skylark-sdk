#ifndef _SIMPLE_THREAD_H
#define _SIMPLE_THREAD_H

#ifdef _WIN32  
	#include <windows.h>  
	#include <direct.h>  
	#include <io.h>  
	#include <process.h>
#elif defined(__unix__)
	#include <unistd.h>  
	#include <getopt.h>  
	#include <sys/types.h>  
	#include <pthread.h>
#define usleep Sleep
#elif defined(__CONFIG_CHIP_XR872) && defined(__CONFIG_OS_FREERTOS)
#include "kernel/os/os.h"
#endif

typedef struct ThreadFun ThreadFun;
#if defined(_WIN32)
typedef unsigned int 	thread_ret_t;
#elif defined(__unix__)
typedef void* 			thread_ret_t;
#elif defined(__CONFIG_CHIP_XR872) && defined(__CONFIG_OS_FREERTOS)
typedef void  			thread_ret_t;
#endif

struct ThreadFun{
	void* params;  							//线程函数的参数
#if defined(_WIN32)
	unsigned int (*fun)(void *params);   	//线程函数指针
#elif defined(__unix__)
	void* (*fun)(void *params);   			//线程函数指针
#elif defined(__CONFIG_CHIP_XR872) && defined(__CONFIG_OS_FREERTOS)
	void (*fun)(void *params);   			//线程函数指针
	OS_Thread_t handler;					//线程句柄
	char name[16];							//线程名称
	int stackSize;							//线程栈大小
#else
#error "not supported platform!"
#endif
};

/**
 * @brief 线程创建
 * @param  iThreadNum       线程创建的个数
 * @param  funArray         线程处理函数组
 */
void vStartThread(int iThreadNum, ThreadFun funArray[]);

#endif