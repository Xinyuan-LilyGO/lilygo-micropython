/*
 * This file is part of the MicroPython ESP32 project, https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 LoBo (https://github.com/loboris)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _LIBGSM_H_
#define _LIBGSM_H_

// #ifdef CONFIG_MICROPY_USE_GSM

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "netif/ppp/ppp.h"

#include <time.h>
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

#define GSM_STATE_DISCONNECTED 0
#define GSM_STATE_CONNECTED    1
#define GSM_STATE_IDLE         89
#define GSM_STATE_FIRSTINIT    98
#define GSM_MAX_NAME_LEN       32

#define SMS_SORT_NONE 0
#define SMS_SORT_ASC  1
#define SMS_SORT_DESC 2

#define SMS_LIST_ALL 0
#define SMS_LIST_NEW 1
#define SMS_LIST_OLD 2
#define SMS_LIST_ALL_STR "AT+CMGL=\"ALL\"\r\n"
#define SMS_LIST_NEW_STR "AT+CMGL=\"REC UNREAD\"\r\n"
#define SMS_LIST_OLD_STR "AT+CMGL=\"REC READ\"\r\n"

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

typedef enum teGSM_Status
{
    E_GSM_STATUS_INIT             = 0,
    E_GSM_STATUS_ACTIVE           = 1,
    E_GSM_STATUS_NO_DATA_LINK     = 2,
    E_GSM_STATUS_DATA_LINK_TO_PPP = 3,
} teGSM_Status;

typedef struct ts_GSM
{
    bool                  active;
    bool                  connected;
    volatile bool         clean_close;
    ppp_pcb              *pcb;
    SemaphoreHandle_t     inactiveWaitSem;
    volatile TaskHandle_t client_task_handle;
    struct netif          pppif;
    int32_t i32UartNum;
    uint32_t pppos_rx_count;
    uint32_t pppos_tx_count;
    teGSM_Status eStatus;
} ts_GSM;

typedef struct SMS_Msg
{
    int32_t idx;
    char *msg;
    char stat[32];
    char from[32];
    char time[32];
    time_t time_value;
    int32_t tz;
} SMS_Msg;

typedef struct SMS_indexes
{
    uint8_t idx[32];
    time_t time[32];
} SMS_indexes;

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

/**
 * Create GSM/PPPoS task if not already created
 * Initialize GSM and connect to Internet
 * Handle all PPPoS requests
 * Disconnect/Reconnect from/to Internet on user request
 * If 'wait' = 1, wait until connected
 * If 'doconn' = 0, only initialize the task, do not connect to Internet
 */
ts_GSM * ppposInit(int32_t i32UartNum,
                   int32_t i32TxPin,
                   int32_t i32RxPin,
                   int32_t i32RtsPin,
                   int32_t i32CtsPin,
                   int32_t i32BaudRate,
                   const char   *apn,
                   bool    bRoaming);

/**
 * @brief
 *
 * @param bActive When bActive is true, activate pppos;
 *                When bActive is false, close pppos.
 *
 * @return active status
 * -  0 disable
 * -  1 enable
 * - -1 error
 */
int32_t ppposActive(ts_GSM *psGSM, bool bActive);

/**
 * @brief Disconnect from Internet.
 */
int32_t ppposDisconnect(ts_GSM *psGSM);

/**
 * @brief Connect from Internet.
 *
 * @note If already connected, this function does nothing
 *
 * @param[in] authmode ppp auth mode \n
 *   @ref PPPAUTHTYPE_NONE None \n
 *   @ref PPPAUTHTYPE_PAP  Password Authentication Protocol \n
 *   @ref PPPAUTHTYPE_CHAP Challenge Handshake Authentication Protocol
 * @param[in] username user name
 * @param[in] password password
 *
 * @return connection result
 *   -  0 success
 *   - -1 auth mode error
 *   - -2 Failed to set default network interface
 *   - -3 ppp connection failed
 *   - -4 ppp receiving thread failed to start
 */
int32_t ppposConnect(ts_GSM *psGSM, int authmode, const char *username, const char *password);

/**
 * @brief Get transmitted and received bytes count.
 *
 * @param[out] u32RxBytes RX bytes
 * @param[out] u32TxBytes TX bytes
 */
void getRxTxCount(uint32_t *u32RxBytes, uint32_t *u32TxBytes);

/**
 * @brief Resets transmitted and received bytes counters.
 */
int32_t resetRxTxCount(void);

/**
 * @brief Get GSM/Task status
 *
 * @param[out] pu32IPAddress ipv4 address
 * @param[out] pu32Netmask   ipv4 netmask
 * @param[out] pu32Gateway   ipv4 gateway
 *
 * @return
 * - @ref GSM_STATE_DISCONNECTED Disconnected from Internet
 * - @ref GSM_STATE_CONNECTED    Connected to Internet
 * - @ref GSM_STATE_IDLE         Disconnected from Internet, Task idle, waiting for reconnect request
 * - @ref GSM_STATE_FIRSTINIT    Task started, initializing PPPoS
 */
int32_t ppposStatus(ts_GSM *psGSM, uint32_t *pu32IPAddress, uint32_t *pu32Netmask, uint32_t *pu32Gateway);

/**
 * @brief Turn GSM RF Off
 */
int32_t gsm_RFOff(void);

/**
 * @brief Turn GSM RF On
 */
int32_t gsm_RFOn(void);

/**
 * @brief Send SMS
 *
 * @param smsnum Pointer to phone number in international format (+<counry_code><gsm number>)
 * @param msg    Pointer to message text
 */
int32_t smsSend(char *smsnum, char *msg);

/**
 * @brief Get messages list
 *
 * @param rd_status check all, unread or read messages
 * @param sms_idx   return sms at index in msg
 * @param msg       SMS message structure pointer
 * @param indexes   pointer to indexes of the detected message
 * @param sort      sort the indexes
 */
int32_t getMessagesList(uint8_t rd_status, int32_t sms_idx, SMS_Msg *msg, SMS_indexes *indexes, uint8_t sort);

/**
 * return number of messages of given type
 * and, optionally the indexes of all new messages
 */
int32_t smsCount(uint8_t type, SMS_indexes *indexes, uint8_t sort);

/**
 * @brief Delete the message at GSM message index 'idx'
 */
int32_t smsDelete(int32_t idx);

int32_t setSMS_cb(void *cb_func, uint32_t interval);

void setDebug(uint8_t dbg);

int32_t at_Cmd(char *cmd, char *resp, char **buffer, int32_t buf_size, int32_t tmo, char *cmddata);

// #endif

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/