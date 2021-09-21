#ifndef _WSCLIENT_H
#define _WSCLIENT_H

#include <stdint.h>
#include "EIHttpClient.h"

#define WEBSOCKET_SHAKE_KEY_LEN     16

bool bWSConnect(SEIHttpInfo_t *pstHttpInfo, fnWebsocketCallback cb, void *pvUserData);

/**
 * @brief 连接到websocket服务端
 * @param  c_pszUrl         websocket服务器的URL
 * @param  cb               websocket服务器的响应信息
 * @return true             连接成功
 * @return false            连接失败
 */
bool bWebsocketConnect(const char *c_pszUrl, fnWebsocketCallback cb, void *pvUserData);

/**
 * @brief 发送数据到websocket服务端
 * @param  pstSock          ws客户端对象
 * @param  pvData           发送数据
 * @param  iLen             发送数据长度
 * @param  eOpcode          发送数据类型操作码
 * @return int              发送数据量，错误返回负值错误码
 */
int iWSSend(SSockClient_t *pstSock, const void *pvData, int iLen, EEIWS_OPCODE eOpcode);

#endif
