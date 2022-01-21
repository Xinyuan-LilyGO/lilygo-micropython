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

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "sdkconfig.h"

// #ifdef CONFIG_MICROPY_USE_GSM

#include <string.h>

#include "esp_system.h"
#include "esp_log.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/uart.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/uart.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/uart.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/uart.h"
#endif

#include "driver/uart.h"
#include "driver/gpio.h"
#include "tcpip_adapter.h"
#include "netif/ppp/pppos.h"

#include "netif/ppp/pppapi.h"
//#include "lwip/port/lwipopts.h"
#include "lwip/opt.h"
#include "lwip/dns.h"

#include "libGSM.h"
#include "py/runtime.h"
#include "mphalport.h"

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

#define BUF_SIZE (1508)
#define GSM_OK_STRING "OK"
#define PPPOSMUTEX_TIMEOUT 5000 / portTICK_RATE_MS

#define PPPOS_CLIENT_STACK_SIZE 1024 * 3

#define GSM_InitCmdsSize (sizeof(GSM_Init) / sizeof(GSM_Cmd *))

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

typedef struct GSM_Cmd
{
    char    *cmd;
    int16_t  cmdSize;
    char    *cmdResponseOnOk;
    uint16_t timeoutMs;
    uint16_t delayMs;
    uint8_t  skip;
} GSM_Cmd;

/******************************************************************************/
/***        local function prototypes                                       ***/
/******************************************************************************/

/**
 * PPP status callback
 */
static void ppp_status_cb(ppp_pcb *pcb, int32_t err_code, void *ctx);

/**
 * Handle sending data to GSM modem
 */
static uint32_t ppp_output_callback(ppp_pcb *pcb, u8_t *data, uint32_t len, void *ctx);

static void infoCommand(char *cmd, int32_t cmdSize, char *info);

static int32_t atCmd_waitResponse(char *cmd, char *rsp, char *rsp1, int32_t cmdSize, int32_t timeout, char **response, int32_t size, char *cmddata);

static void _disconnect(uint8_t rfOff);

static void enableAllInitCmd(void);

static void checkSMS(void);

/**
 * PPPoS TASK
 * Handles GSM initialization, disconnects and GSM modem responses
 */
static void pppos_client_task(void *args);

static int32_t sms_ready();

time_t sms_time(char *msg_time, int32_t *tz);

/**
 * @brief Parse message in buffer to message structure
 */
static int32_t getSMS(char *msgstart, SMS_Msg *msg, uint8_t msgalloc);

/**
 * @brief Get message index and time
 */
static int32_t getSMSindex(char *msgstart, time_t *msgtime);

static int32_t checkMessages(uint8_t rd_status, int32_t sms_idx, SMS_Msg *msg, SMS_indexes *indexes, uint8_t sort);

static int32_t user_uart_init(uart_port_t uart_num, int32_t tx, int32_t rx, int32_t rts, int32_t cts, int32_t bdr);

static int32_t gsm_init(void);

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        local variables                                                 ***/
/******************************************************************************/

// shared variables, use mutex to access them
static uint8_t gsm_status = GSM_STATE_FIRSTINIT;
static uint32_t pppos_rx_count;
static uint32_t pppos_tx_count;

static void *New_SMS_cb = NULL;
static uint32_t SMS_check_interval = 0;
static uint8_t debug = 1;
static uint8_t doCheckSMS = 1;

// local variables
static QueueHandle_t pppos_mutex = NULL;
static char PPP_User[GSM_MAX_NAME_LEN] = {0};
static char PPP_Pass[GSM_MAX_NAME_LEN] = {0};
static char GSM_APN[GSM_MAX_NAME_LEN] = {0};
static int32_t uart_num = UART_NUM_1;

static uint32_t sms_timer = 0;
static bool allow_roaming = false;

static const char *TAG = "[PPPOS CLIENT]";

static GSM_Cmd cmd_AT =
{
    .cmd = "AT\r\n",
    .cmdSize = sizeof("AT\r\n") - 1,
    .cmdResponseOnOk = GSM_OK_STRING,
    .timeoutMs = 300,
    .delayMs = 0,
    .skip = 0,
};

static GSM_Cmd cmd_NoSMSInd =
{
    .cmd = "AT+CNMI=0,0,0,0,0\r\n",
    .cmdSize = -1,
    .cmdResponseOnOk = GSM_OK_STRING,
    .timeoutMs = 1000,
    .delayMs = 0,
    .skip = 0,
};

static GSM_Cmd cmd_Reset =
{
    .cmd = "ATZ\r\n",
    .cmdSize = -1,
    .cmdResponseOnOk = GSM_OK_STRING,
    .timeoutMs = 300,
    .delayMs = 0,
    .skip = 0,
};

static GSM_Cmd cmd_RFOn =
{
    .cmd = "AT+CFUN=1\r\n",
    .cmdSize = -1,
    .cmdResponseOnOk = GSM_OK_STRING,
    .timeoutMs = 10000,
    .delayMs = 1000,
    .skip = 0,
};

static GSM_Cmd cmd_EchoOff =
{
    .cmd = "ATE0\r\n",
    .cmdSize = -1,
    .cmdResponseOnOk = GSM_OK_STRING,
    .timeoutMs = 300,
    .delayMs = 0,
    .skip = 0,
};

static GSM_Cmd cmd_Pin =
{
    .cmd = "AT+CPIN?\r\n",
    .cmdSize = -1,
    .cmdResponseOnOk = "CPIN: READY",
    .timeoutMs = 5000,
    .delayMs = 0,
    .skip = 0,
};

static GSM_Cmd cmd_Reg =
{
    .cmd = "AT+CREG?\r\n",
    .cmdSize = -1,
    .cmdResponseOnOk = "CREG: 0,2",
    .timeoutMs = 3000,
    .delayMs = 2000,
    .skip = 0,
};

static GSM_Cmd cmd_APN =
{
    .cmd = NULL,
    .cmdSize = 0,
    .cmdResponseOnOk = GSM_OK_STRING,
    .timeoutMs = 8000,
    .delayMs = 0,
    .skip = 0,
};

static GSM_Cmd cmd_Connect =
{
    .cmd = "AT+CGDATA=\"PPP\",1\r\n",
    //.cmd = "ATDT*99***1#\r\n",
    .cmdSize = -1,
    .cmdResponseOnOk = "CONNECT",
    .timeoutMs = 30000,
    .delayMs = 1000,
    .skip = 0,
};

static GSM_Cmd *GSM_Init[] =
{
    &cmd_AT,
    &cmd_Reset,
    &cmd_EchoOff,
    &cmd_RFOn,
    &cmd_Pin,
    &cmd_Reg,
    &cmd_NoSMSInd,
    &cmd_APN,
    &cmd_Connect,
};


/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

ts_GSM * ppposInit(int32_t i32UartNum,
                   int32_t i32TxPin,
                   int32_t i32RxPin,
                   int32_t i32RtsPin,
                   int32_t i32CtsPin,
                   int32_t i32BaudRate,
                   const char   *apn,
                   bool    bRoaming)
{
    char    PPP_ApnATReq[64] = { 0 };
    ts_GSM *psGSM            = NULL;

    tcpip_adapter_init();
    user_uart_init(i32UartNum, i32TxPin, i32RxPin, i32RtsPin, i32CtsPin, i32BaudRate);

    // Set APN from config
    sprintf(PPP_ApnATReq, "AT+CGDCONT=1,\"IP\",\"%s\"\r\n", apn);
    cmd_APN.cmd = PPP_ApnATReq;
    cmd_APN.cmdSize = strlen(PPP_ApnATReq);
    enableAllInitCmd();
    if (!gsm_init())
    {
        psGSM = calloc(1, sizeof(ts_GSM));
        if (!psGSM) return NULL;
        psGSM->inactiveWaitSem = xSemaphoreCreateMutex();
        psGSM->active = false;
        psGSM->connected = false;
        psGSM->clean_close = false;
        psGSM->client_task_handle = NULL;
        psGSM->i32UartNum = i32UartNum;
        psGSM->eStatus = E_GSM_STATUS_INIT;
        return psGSM;
    }
    else
    {
        return NULL;
    }
}


int ppposActive(ts_GSM *psGSM, bool bActive)
{
    if (bActive)
    {
        if (psGSM->active) return 1;

        psGSM->pcb = pppapi_pppos_create(&psGSM->pppif, ppp_output_callback, ppp_status_cb, psGSM);
        if (psGSM->pcb)
        {
            psGSM->active = true;
            psGSM->eStatus = E_GSM_STATUS_ACTIVE;
            if (debug)
            {
                ESP_LOGI(TAG, "PPPoS control block created");
            }
            return 1;
        }
        else
        {
            if (debug)
            {
                ESP_LOGE(TAG, "Error initializing PPPoS");
            }
            return -1;
        }
    }
    else
    {
        if (!psGSM->active) return 0;

        if (psGSM->client_task_handle != NULL) // is connecting or connected?
        {
            // Wait for PPPERR_USER, with timeout
            pppapi_close(psGSM->pcb, 0);
            while (!psGSM->clean_close) ;

            // Shutdown task
            xTaskNotifyGive(psGSM->client_task_handle);
            while (psGSM->client_task_handle != NULL);
        }

        pppapi_free(psGSM->pcb);
        psGSM->pcb = NULL;
        psGSM->active = false;
        psGSM->connected = false;
        psGSM->clean_close = false;
        psGSM->eStatus = E_GSM_STATUS_INIT;
        return 0;
    }
}


int32_t ppposConnect(ts_GSM *psGSM, int authmode, const char *username, const char *password)
{
    if (psGSM->connected) return 0;

    if (authmode != PPPAUTHTYPE_NONE && \
        authmode != PPPAUTHTYPE_PAP && \
        authmode != PPPAUTHTYPE_CHAP)
    {
        return -1;
    }

    if (authmode != PPPAUTHTYPE_NONE)
    {
        pppapi_set_auth(psGSM->pcb, authmode, username, password);
    }

    if (pppapi_set_default(psGSM->pcb) != ESP_OK)
    {
        return -2;
    }

    ppp_set_usepeerdns(psGSM->pcb, true);

    if (pppapi_connect(psGSM->pcb, 0) != ESP_OK)
    {
        return -3;
    }

    if (xTaskCreatePinnedToCore(&pppos_client_task,
                                "ppp",
                                4096,
                                psGSM,
                                8,
                                (TaskHandle_t *)&psGSM->client_task_handle,
                                0) != pdPASS)
    {
        pppapi_close(psGSM->pcb, 0);
        return -4;
    }

    psGSM->connected = true;
    psGSM->eStatus = E_GSM_STATUS_NO_DATA_LINK;
    return 0;
}


int32_t ppposDisconnect(ts_GSM *psGSM)
{
    if (!psGSM->connected) return 0;

    if (psGSM->client_task_handle != NULL) // is connecting or connected?
    {
        // Wait for PPPERR_USER, with timeout
        pppapi_close(psGSM->pcb, 0);
        while (!psGSM->clean_close) ;

        // Shutdown task
        xTaskNotifyGive(psGSM->client_task_handle);
        while (psGSM->client_task_handle != NULL);
    }

    uart_flush(psGSM->i32UartNum);
    psGSM->connected = false;
    psGSM->eStatus = E_GSM_STATUS_NO_DATA_LINK;
    return 0;
}


int32_t ppposStatus(ts_GSM *psGSM, uint32_t *pu32IPAddress, uint32_t *pu32Netmask, uint32_t *pu32Gateway)
{
    int32_t gstat = E_GSM_STATUS_INIT;
    struct netif *pppif = ppp_netif(psGSM->pcb);

    if (psGSM->inactiveWaitSem == NULL) return gstat;

    xSemaphoreTake(psGSM->inactiveWaitSem, PPPOSMUTEX_TIMEOUT);
    gstat = psGSM->eStatus;
    if (pu32IPAddress)
        *pu32IPAddress = pppif->ip_addr.u_addr.ip4.addr;
    if (pu32Netmask)
        *pu32Netmask = pppif->netmask.u_addr.ip4.addr;
    if (pu32Gateway)
        *pu32Gateway = pppif->gw.u_addr.ip4.addr;
    xSemaphoreGive(psGSM->inactiveWaitSem);

    return gstat;
}


void getRxTxCount(uint32_t *u32RxBytes, uint32_t *u32TxBytes)
{
    if (pppos_mutex == NULL)
    {
        if (u32RxBytes) *u32RxBytes = 0;
        if (u32TxBytes) *u32TxBytes = 0;
    }

    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    if (u32RxBytes) *u32RxBytes = pppos_rx_count;
    if (u32TxBytes) *u32TxBytes = pppos_tx_count;
    xSemaphoreGive(pppos_mutex);
}


int32_t resetRxTxCount(void)
{
    if (pppos_mutex == NULL) return -1;
    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    pppos_rx_count = 0;
    pppos_tx_count = 0;
    xSemaphoreGive(pppos_mutex);
    return 0;
}


int32_t gsm_RFOff(void)
{
    if (pppos_mutex == NULL)
        return 1;
    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    int32_t gstat = gsm_status;
    xSemaphoreGive(pppos_mutex);

    if (gstat != GSM_STATE_IDLE)
        return 0;

    uint8_t f = 1;
    char buf[64] = {'\0'};
    char *pbuf = buf;
    int32_t res = atCmd_waitResponse("AT+CFUN?\r\n", NULL, NULL, -1, 2000, &pbuf, 63, NULL);
    if (res > 0)
    {
        if (strstr(buf, "+CFUN: 4"))
            f = 0;
    }

    if (f)
    {
        cmd_Reg.timeoutMs = 500;
        return atCmd_waitResponse("AT+CFUN=4\r\n", GSM_OK_STRING, NULL, 11, 10000, NULL, 0, NULL); // disable RF function
    }
    return 1;
}


int32_t gsm_RFOn(void)
{
    if (pppos_mutex == NULL)
        return 1;
    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    int32_t gstat = gsm_status;
    xSemaphoreGive(pppos_mutex);

    if (gstat != GSM_STATE_IDLE)
        return 0;

    uint8_t f = 1;
    char buf[64] = {'\0'};
    char *pbuf = buf;
    int32_t res = atCmd_waitResponse("AT+CFUN?\r\n", NULL, NULL, -1, 2000, &pbuf, 63, NULL);
    if (res > 0)
    {
        if (strstr(buf, "+CFUN: 1"))
            f = 0;
    }

    if (f)
    {
        cmd_Reg.timeoutMs = 0;
        return atCmd_waitResponse("AT+CFUN=1\r\n", GSM_OK_STRING, NULL, 11, 10000, NULL, 0, NULL); // disable RF function
    }
    return 1;
}


int32_t smsSend(char *smsnum, char *msg)
{
    if (sms_ready() == 0)
        return 0;

    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 0;
    xSemaphoreGive(pppos_mutex);

    char *msgbuf = NULL;
    int32_t res = 0;
    char buf[64];
    int32_t len = strlen(msg);

    sprintf(buf, "AT+CMGS=\"%s\"\r\n", smsnum);
    res = atCmd_waitResponse(buf, "> ", NULL, -1, 1000, NULL, 0, NULL);
    if (res != 1)
    {
        atCmd_waitResponse("\x1B", GSM_OK_STRING, NULL, 1, 1000, NULL, 0, NULL);
        res = 0;
        goto exit;
    }

    msgbuf = malloc(len + 2);
    if (msgbuf == NULL)
    {
        res = 0;
        goto exit;
    }

    sprintf(msgbuf, "%s\x1A", msg);
    res = atCmd_waitResponse(msgbuf, "+CMGS: ", "ERROR", len + 1, 40000, NULL, 0, NULL);
    if (res != 1)
    {
        res = atCmd_waitResponse("\x1B", GSM_OK_STRING, NULL, 1, 1000, NULL, 0, NULL);
        res = 0;
    }
exit:
    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 1;
    xSemaphoreGive(pppos_mutex);

    if (msgbuf)
        free(msgbuf);
    return res;
}


int32_t smsCount(uint8_t type, SMS_indexes *indexes, uint8_t sort)
{
    if (sms_ready() == 0)
        return -1;

    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 0;
    xSemaphoreGive(pppos_mutex);

    int32_t res = checkMessages(type, 0, NULL, indexes, sort);

    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 1;
    xSemaphoreGive(pppos_mutex);

    return res;
}


int32_t getMessagesList(uint8_t rd_status, int32_t sms_idx, SMS_Msg *msg, SMS_indexes *indexes, uint8_t sort)
{
    if (sms_ready() == 0)
        return -1;

    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 0;
    xSemaphoreGive(pppos_mutex);

    int32_t res = checkMessages(rd_status, sms_idx, msg, indexes, sort);

    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 1;
    xSemaphoreGive(pppos_mutex);

    return res;
}


int32_t smsDelete(int32_t idx)
{
    if (sms_ready() == 0)
        return 0;

    char buf[64];

    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 0;
    xSemaphoreGive(pppos_mutex);

    sprintf(buf, "AT+CMGD=%d\r\n", idx);

    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 1;
    xSemaphoreGive(pppos_mutex);

    return atCmd_waitResponse(buf, GSM_OK_STRING, NULL, -1, 5000, NULL, 0, NULL);
}


int32_t setSMS_cb(void *cb_func, uint32_t interval)
{
    if (pppos_mutex == NULL)
        return 0;
    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    New_SMS_cb = cb_func;
    SMS_check_interval = interval;
    xSemaphoreGive(pppos_mutex);
    return 1;
}


void setDebug(uint8_t dbg)
{
    if (pppos_mutex != NULL)
        xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    debug = dbg;
    if (debug)
        esp_log_level_set(TAG, ESP_LOG_DEBUG);
    else
        esp_log_level_set(TAG, ESP_LOG_NONE);
    if (pppos_mutex != NULL)
        xSemaphoreGive(pppos_mutex);
}


int32_t at_Cmd(char *cmd, char *resp, char **buffer, int32_t buf_size, int32_t tmo, char *cmddata)
{
    if (ppposStatus(NULL, NULL, NULL, NULL) != GSM_STATE_IDLE)
        return 0;
    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);

    int32_t res = atCmd_waitResponse(cmd, resp, NULL, -1, tmo, buffer, buf_size, cmddata);

    xSemaphoreGive(pppos_mutex);
    return res;
}


/******************************************************************************/
/***        local functions                                                 ***/
/******************************************************************************/

static void ppp_status_cb(ppp_pcb *pcb, int32_t err_code, void *ctx)
{
    ts_GSM *psGSM = ctx;
    struct netif *pppif = ppp_netif(pcb);

    switch (err_code)
    {
        case PPPERR_NONE:
        {
            if (debug)
            {
                ESP_LOGI(TAG, "status_cb: Connected");
#if PPP_IPV4_SUPPORT
                ESP_LOGI(TAG, "    ipaddr    = %s", ipaddr_ntoa(&pppif->ip_addr));
                ESP_LOGI(TAG, "    gateway   = %s", ipaddr_ntoa(&pppif->gw));
                ESP_LOGI(TAG, "    netmask   = %s", ipaddr_ntoa(&pppif->netmask));
#endif
#if PPP_IPV6_SUPPORT
                ESP_LOGI(TAG, "    ip6addr   = %s", ip6addr_ntoa(netif_ip6_addr(pppif, 0)));
#endif
            }
            psGSM->connected = (pppif->ip_addr.u_addr.ip4.addr != 0);
            psGSM->eStatus = E_GSM_STATUS_DATA_LINK_TO_PPP;
        }
        break;

        case PPPERR_PARAM:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Invalid parameter");
            }
        }
        break;

        case PPPERR_OPEN:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Unable to open PPP session");
            }
        }
        break;

        case PPPERR_DEVICE:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Invalid I/O device for PPP");
            }
        }
        break;

        case PPPERR_ALLOC:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Unable to allocate resources");
            }
        }
        break;

        case PPPERR_USER:
        {
            /* ppp_free(); -- can be called here */
            if (debug)
            {
                ESP_LOGW(TAG, "status_cb: User interrupt (disconnected)");
            }
            psGSM->clean_close = true;
            psGSM->eStatus = E_GSM_STATUS_NO_DATA_LINK;
        }
        break;

        case PPPERR_CONNECT:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Connection lost");
            }
            psGSM->connected = false;
        }
        break;

        case PPPERR_AUTHFAIL:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Failed authentication challenge");
            }
        }
        break;

        case PPPERR_PROTOCOL:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Failed to meet protocol");
            }
        }
        break;

        case PPPERR_PEERDEAD:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Connection timeout");
            }
        }
        break;

        case PPPERR_IDLETIMEOUT:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Idle Timeout");
            }
        }
        break;

        case PPPERR_CONNECTTIME:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Max connect time reached");
            }
        }
        break;

        case PPPERR_LOOPBACK:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Loopback detected");
            }
        }
        break;

        default:
        {
            if (debug)
            {
                ESP_LOGE(TAG, "status_cb: Unknown error code %d", err_code);
            }
        }
        break;
    }
}


static uint32_t ppp_output_callback(ppp_pcb *pcb, u8_t *data, uint32_t len, void *ctx)
{
    ts_GSM *psGSM = ctx;
    uint32_t ret = uart_write_bytes(psGSM->i32UartNum, (const char *)data, len);
    uart_wait_tx_done(psGSM->i32UartNum, 10 / portTICK_RATE_MS);
    // if (ret > 0)
    // {
    //     xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    //     pppos_rx_count += ret;
    //     xSemaphoreGive(pppos_mutex);
    // }
    return ret;
}


static void infoCommand(char *cmd, int32_t cmdSize, char *info)
{
    char buf[cmdSize + 2];
    memset(buf, 0, cmdSize + 2);

    for (int32_t i = 0; i < cmdSize; i++)
    {
        if ((cmd[i] != 0x00) && ((cmd[i] < 0x20) || (cmd[i] > 0x7F)))
            buf[i] = '.';
        else
            buf[i] = cmd[i];
        if (buf[i] == '\0')
            break;
    }
    ESP_LOGI(TAG, "%s [%s]", info, buf);
}


static int32_t atCmd_waitResponse(char *cmd, char *rsp, char *rsp1, int32_t cmdSize, int32_t timeout, char **response, int32_t size, char *cmddata)
{
    char data[256] = { 0 };
    int32_t len, res = 0, tot = 0, timeoutCnt = 0;
    size_t blen = 0;

    // ** Send command to GSM
    vTaskDelay(100 / portTICK_PERIOD_MS);
    uart_flush(uart_num);

    if (cmd != NULL)
    {
        if (debug)
        {
            infoCommand(cmd, strlen(cmd), "AT COMMAND:");
        }
        uart_write_bytes(uart_num, (const char *)cmd, strlen(cmd));
        uart_wait_tx_done(uart_num, 100 / portTICK_RATE_MS);
    }

    if (response != NULL)
    {
        // === Read GSM response into buffer ===
        char *pbuf = *response;
        // wait for first response data
        while (blen == 0)
        {
            uart_get_buffered_data_len(uart_num, &blen);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            timeoutCnt += 10;
            if (timeoutCnt > timeout)
                break;
        }
        len = uart_read_bytes(uart_num, (uint8_t *)data, 256, 50 / portTICK_RATE_MS);
        // Add response to buffer
        while (len > 0)
        {
            if ((tot + len) >= size)
            {
                // Need to expand the buffer
                char *ptemp = realloc(pbuf, size + 512);
                if (ptemp == NULL)
                {
                    if (debug)
                    {
                        ESP_LOGE(TAG, "AT RESPONSE (to buffer): Error reallocating buffer of size %d", size + 512);
                    }
                    // Ignore any new data sent by modem
                    while (len > 0)
                    {
                        len = uart_read_bytes(uart_num, (uint8_t *)data, 256, 100 / portTICK_RATE_MS);
                    }
                    return tot; // return success with received bytes
                }
                else if (debug)
                {
                    ESP_LOGD(TAG, "AT RESPONSE (to buffer): buffer reallocated, new size: %d", size + 512);
                }
                size += 512;
                pbuf = ptemp;
            }
            memcpy(pbuf + tot, data, len); // append response to the buffer
            tot += len;                    // increase total received count
            pbuf[tot] = '\0';              // terminate string
            if (rsp != NULL)
            {
                // Check terminating string
                if (strstr(pbuf, rsp))
                {
                    if (debug)
                    {
                        ESP_LOGI(TAG, "RESPONSE terminator detected");
                    }
                    if (cmddata)
                    {
                        if (debug)
                        {
                            ESP_LOGI(TAG, "Sending data");
                        }
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                        // Send data after response
                        uart_write_bytes(uart_num, (const char *)cmddata, strlen(cmddata));
                        uart_wait_tx_done(uart_num, 1000 / portTICK_RATE_MS);
                        // Read the response after the data was sent
                        rsp = NULL;
                        // wait for first response data
                        timeoutCnt = 0;
                        blen = 0;
                        while (blen == 0)
                        {
                            uart_get_buffered_data_len(uart_num, &blen);
                            vTaskDelay(10 / portTICK_PERIOD_MS);
                            timeoutCnt += 10;
                            if (timeoutCnt > timeout)
                                break;
                        }
                        len = uart_read_bytes(uart_num, (uint8_t *)data, 256, 50 / portTICK_RATE_MS);
                        continue;
                    }
                    // Ignore any new data sent by modem
                    while (len > 0)
                    {
                        len = uart_read_bytes(uart_num, (uint8_t *)data, 256, 100 / portTICK_RATE_MS);
                    }
                    break;
                }
            }
            len = uart_read_bytes(uart_num, (uint8_t *)data, 256, 100 / portTICK_RATE_MS);
        }
        *response = pbuf;
        if (debug)
        {
            ESP_LOGI(TAG, "AT RESPONSE (to buffer): len=%d", tot);
        }
        return tot;
    }

    // === Receive response to temporary buffer, wait for and check the response ===
    char sresp[256] = { 0 };
    int32_t idx = 0;
    while (1)
    {
        memset(data, 0, 256);
        len = 0;
        len = uart_read_bytes(uart_num, (uint8_t *)data, 256, 20 / portTICK_RATE_MS);
        ESP_LOGI(TAG, "len: [%d]", len);
        if (len > 0)
        {
            for (int32_t i = 0; i < len; i++)
            {
                if (idx < 256)
                {
                    if ((data[i] >= 0x20) && (data[i] < 0x80))
                        sresp[idx++] = data[i];
                    else
                        sresp[idx++] = 0x2e;
                }
            }
            tot += len;
        }
        else
        {
            if (tot > 0)
            {
                // Check the response
                if (rsp && strstr(sresp, rsp) != NULL)
                {
                    if (debug)
                    {
                        ESP_LOGI(TAG, "AT RESPONSE: [%s]", sresp);
                    }
                    res++;
                }

                if (rsp1 && strstr(sresp, rsp1) != NULL)
                {
                    if (debug)
                    {
                        ESP_LOGI(TAG, "AT RESPONSE (1): [%s]", sresp);
                    }
                    res++;
                }

                // no match
                if (!res && debug)
                {
                    ESP_LOGI(TAG, "AT BAD RESPONSE: [%s]", sresp);
                }
            }
            break;
        }

        timeoutCnt += 10;
        if (timeoutCnt > timeout)
        {
            // timeout
            if (debug)
            {
                ESP_LOGE(TAG, "AT: TIMEOUT");
            }
            res = 0;
            break;
        }
    }

    return res;
}


static void _disconnect(uint8_t rfOff)
{
    int32_t res = atCmd_waitResponse("AT\r\n", GSM_OK_STRING, NULL, 4, 1000, NULL, 0, NULL);
    if (res == 1)
    {
        if (rfOff)
        {
            cmd_Reg.timeoutMs = 10000;
            res = atCmd_waitResponse("AT+CFUN=4\r\n", GSM_OK_STRING, NULL, 11, 10000, NULL, 0, NULL); // disable RF function
        }
        return;
    }

    if (debug)
    {
        ESP_LOGI(TAG, "ONLINE, DISCONNECTING...");
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    uart_flush(uart_num);
    uart_write_bytes(uart_num, "+++", 3);
    uart_wait_tx_done(uart_num, 10 / portTICK_RATE_MS);
    vTaskDelay(1100 / portTICK_PERIOD_MS);

    int32_t n = 0;
    res = atCmd_waitResponse("ATH\r\n", GSM_OK_STRING, "NO CARRIER", 5, 3000, NULL, 0, NULL);
    while (res == 0)
    {
        n++;
        if (n > 10)
        {
            if (debug)
            {
                ESP_LOGI(TAG, "STILL CONNECTED.");
            }
            n = 0;
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            uart_flush(uart_num);
            uart_write_bytes(uart_num, "+++", 3);
            uart_wait_tx_done(uart_num, 10 / portTICK_RATE_MS);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
        res = atCmd_waitResponse("ATH\r\n", GSM_OK_STRING, "NO CARRIER", 5, 3000, NULL, 0, NULL);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    if (rfOff)
    {
        cmd_Reg.timeoutMs = 10000;
        res = atCmd_waitResponse("AT+CFUN=4\r\n", GSM_OK_STRING, NULL, 11, 3000, NULL, 0, NULL);
    }
    if (debug)
    {
        ESP_LOGI(TAG, "DISCONNECTED.");
    }
}


static void enableAllInitCmd(void)
{
    for (int32_t idx = 0; idx < GSM_InitCmdsSize; idx++)
    {
        GSM_Init[idx]->skip = 0;
    }
}


static void checkSMS(void)
{
    if ((New_SMS_cb) && (SMS_check_interval > 0) && (doCheckSMS))
    {
        // Check for new SMS and schedule MicroPython callback function
        uint8_t dbg = debug;
        debug = 0;
        if (sms_timer > SMS_check_interval)
        {
            sms_timer = 0;
            SMS_indexes indexes;
            int32_t nmsg = smsCount(SMS_LIST_NEW, &indexes, SMS_SORT_NONE);
            if (nmsg > 0)
            {
                if (nmsg > 100)
                    nmsg = 100;
                // Create a string containing SMS indexes
                char sidx[5];
                char *sindexes = calloc(nmsg * 3, 1);
                if (sindexes)
                {
                    for (int32_t i = 0; i < nmsg; i++)
                    {
                        sprintf(sidx, "%d;", indexes.idx[i]);
                        strcat(sindexes, sidx);
                    }
#if 0
                    mp_sched_carg_t *carg = make_cargs(MP_SCHED_CTYPE_SINGLE);
                    if (!carg)
                        goto end;
                    if (!make_carg_entry(carg, 0, MP_SCHED_ENTRY_TYPE_STR, strlen(sindexes), (const uint8_t *)sindexes, NULL))
                        goto end;
#endif
                    mp_sched_schedule(New_SMS_cb, MP_OBJ_FROM_PTR(sindexes));
// end:
                    free(sindexes);
                }
            }
        }
        else
            sms_timer += 100;
        debug = dbg;
    }
}


static void pppos_client_task(void *args)
{
    uint8_t buf[BUF_SIZE];
    ts_GSM *psGSM = (ts_GSM *)args;

    if (!psGSM)
    {
        ESP_LOGI(TAG, "task args error");
    }


    ESP_LOGI(TAG, "pppos_client_task");

    while (1)
    {
        int32_t len = uart_read_bytes(psGSM->i32UartNum, (uint8_t *)buf, BUF_SIZE, 500 / portTICK_RATE_MS);
        if (len > 0) {
            pppos_input_tcpip(psGSM->pcb, (u8_t *)buf, len);
        }
    }

    psGSM->client_task_handle = NULL;
    vTaskDelete(NULL);
}


static int32_t sms_ready()
{
    if (ppposStatus(NULL, NULL, NULL, NULL) != GSM_STATE_IDLE)
        return 0;
    int32_t ret = 0;

    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 0;
    xSemaphoreGive(pppos_mutex);

    int32_t res = atCmd_waitResponse("AT+CFUN?\r\n", "+CFUN: 1", NULL, -1, 1000, NULL, 0, NULL);
    if (res != 1)
        goto exit;

    res = atCmd_waitResponse("AT+CMGF=1\r\n", GSM_OK_STRING, NULL, -1, 1000, NULL, 0, NULL);
    if (res != 1)
        goto exit;
    ret = 1;

    //res = atCmd_waitResponse("AT+CPMS=\"SM\"\r\n", GSM_OK_STRING, NULL, -1, 1000, NULL, 0, NULL);
    //if (res != 1) goto exit;
exit:
    xSemaphoreTake(pppos_mutex, PPPOSMUTEX_TIMEOUT);
    doCheckSMS = 1;
    xSemaphoreGive(pppos_mutex);

    return ret;
}


time_t sms_time(char *msg_time, int32_t *tz)
{
    if (strlen(msg_time) >= 20)
    {
        // Convert message time to time structure
        int32_t hh, mm, ss, yy, mn, dd, tz;
        struct tm tm;
        sscanf(msg_time, "%u/%u/%u,%u:%u:%u%d", &yy, &mn, &dd, &hh, &mm, &ss, &tz);
        tm.tm_hour = hh;
        tm.tm_min = mm;
        tm.tm_sec = ss;
        tm.tm_year = yy + 100;
        tm.tm_mon = mn - 1;
        tm.tm_mday = dd;
        if (tz)
            tz = tz / 4;    // time zone info
        return mktime(&tm); // Linux time
    }
    return 0;
}


/**
 * @brief Parse message in buffer to message structure
 */
static int32_t getSMS(char *msgstart, SMS_Msg *msg, uint8_t msgalloc)
{
    char *msgidx = msgstart;
    // Clear message structure
    memset(msg, 0, sizeof(SMS_Msg));

    // Get message info
    char *pend = strstr(msgidx, "\r\n");
    if (pend == NULL)
        return 0;

    int32_t len = pend - msgidx;
    char hdr[len + 4];
    char buf[32];

    memset(hdr, 0, len + 4);
    memcpy(hdr, msgidx, len);
    hdr[len] = '\0';

    if (msgalloc)
    {
        msgidx = pend + 2;
        // Allocate message body buffer and copy the data
        len = strlen(msgidx);
        msg->msg = (char *)calloc(len + 1, 1);
        if (msg->msg)
        {
            memcpy(msg->msg, msgidx, len);
            msg->msg[len] = '\0';
        }
    }

    // Parse message info
    msgidx = hdr;
    pend = strstr(hdr, ",\"");
    int32_t i = 1;
    while (pend != NULL)
    {
        len = pend - msgidx;
        if ((len < 32) && (len > 0))
        {
            memset(buf, 0, 32);
            strncpy(buf, msgidx, len);
            buf[len] = '\0';
            if (buf[len - 1] == '"')
                buf[len - 1] = '\0';

            if (i == 1)
            {
                msg->idx = (int32_t)strtol(buf, NULL, 0); // message index
            }
            else if (i == 2)
                strcpy(msg->stat, buf); // message status
            else if (i == 3)
                strcpy(msg->from, buf); // phone number of message sender
            else if (i == 5)
                strcpy(msg->time, buf); // the time when the message was sent
        }
        i++;
        msgidx = pend + 2;
        pend = strstr(msgidx, ",\"");
        if (pend == NULL)
            pend = strstr(msgidx, "\"");
    }

    msg->time_value = sms_time(msg->time, &msg->tz);

    return 1;
}


/**
 * @brief Get message index and time
 */
static int32_t getSMSindex(char *msgstart, time_t *msgtime)
{
    char *msgidx = msgstart;
    // Get message info
    char *pend = strstr(msgidx, "\r\n");
    if (pend == NULL)
        return 0;

    int32_t len = pend - msgidx;
    char hdr[len + 4];
    char buf[32];
    char msg_time[32] = {'\0'};
    int32_t msg_idx = 0;

    memcpy(hdr, msgidx, len);
    hdr[len] = '\0';

    // Parse message info
    msgidx = hdr;
    pend = strstr(hdr, ",\"");
    int32_t i = 1;
    while (pend != NULL)
    {
        len = pend - msgidx;
        if ((len < 32) && (len > 0))
        {
            memset(buf, 0, 32);
            memcpy(buf, msgidx, len);
            buf[len] = '\0';
            if (buf[len - 1] == '"')
                buf[len - 1] = '\0';

            if (i == 1)
                msg_idx = (int32_t)strtol(buf, NULL, 0); // message index
            else if (i == 5)
                strcpy(msg_time, buf); // the time when the message was sent
        }
        i++;
        msgidx = pend + 2;
        // find next entry
        pend = strstr(msgidx, ",\"");
        if (pend == NULL)
            pend = strstr(msgidx, "\"");
    }

    *msgtime = sms_time(msg_time, NULL);

    return msg_idx;
}


static int32_t checkMessages(uint8_t rd_status, int32_t sms_idx, SMS_Msg *msg, SMS_indexes *indexes, uint8_t sort)
{
    int32_t timeoutCnt = 0;
    size_t blen = 0;

    // ** Send command to GSM
    vTaskDelay(100 / portTICK_PERIOD_MS);
    uart_flush(uart_num);

    if (rd_status == SMS_LIST_NEW)
        uart_write_bytes(uart_num, SMS_LIST_NEW_STR, strlen(SMS_LIST_NEW_STR));
    else if (rd_status == SMS_LIST_OLD)
        uart_write_bytes(uart_num, SMS_LIST_OLD_STR, strlen(SMS_LIST_OLD_STR));
    else
        uart_write_bytes(uart_num, SMS_LIST_ALL_STR, strlen(SMS_LIST_ALL_STR));

    uart_wait_tx_done(uart_num, 100 / portTICK_RATE_MS);

    // ** Read GSM response
    // wait for first response data
    while (blen == 0)
    {
        uart_get_buffered_data_len(uart_num, &blen);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        timeoutCnt += 10;
        if (timeoutCnt > 1000)
        {
            if (debug)
            {
                ESP_LOGE(TAG, "Check SMS, no response (timeout)");
            }
            return 0;
        }
    }

    if (indexes != NULL)
        memset(indexes, 0, sizeof(SMS_indexes));

    char *rbuffer = calloc(1024, 1);
    if (rbuffer == NULL)
    {
        if (debug)
        {
            ESP_LOGE(TAG, "Check SMS, Error allocating receive buffer");
        }
        return 0;
    }

    int32_t len, buflen = 0, nmsg = 0;
    uint8_t idx_found = 0;
    char *msgstart = rbuffer;
    char *msgend = NULL;
    char *bufptr = rbuffer;
    while (1)
    {
        len = uart_read_bytes(uart_num, (uint8_t *)bufptr, 1023 - buflen, 50 / portTICK_RATE_MS);
        if (len == 0)
            break;
        buflen += len;
        bufptr += len;
        *bufptr = '\0';

        // Check message start string
        msgstart = strstr(rbuffer, "+CMGL: ");
        if (msgstart)
            msgend = strstr(msgstart, "\r\n\r\n");

        while ((msgstart) && (msgend))
        {
            *msgend = '\0';
            // We have the whole message in the buffer
            nmsg++;
            if ((indexes != NULL) && (nmsg < 33))
                indexes->idx[nmsg - 1] = getSMSindex(msgstart + 7, &indexes->time[nmsg - 1]);
            if ((sms_idx == nmsg) && (msg != NULL))
            {
                getSMS(msgstart + 7, msg, 1);
                // Ignore remaining data from module
                while (len > 0)
                {
                    len = uart_read_bytes(uart_num, (uint8_t *)rbuffer, 1023, 50 / portTICK_RATE_MS);
                }
                // and return
                idx_found = 1;
                break;
            }

            // Delete the message
            memmove(rbuffer, msgend + 4, buflen - (msgend - rbuffer + 4));
            buflen -= (msgend - rbuffer + 4);
            bufptr = rbuffer + buflen;
            *bufptr = '\0';

            // Check message start string
            msgend = NULL;
            msgstart = strstr(rbuffer, "+CMGL: ");
            if (msgstart)
                msgend = strstr(msgstart, "\r\n\r\n");
        }
    }

    free(rbuffer);

    if ((msg != NULL) && (idx_found == 0))
        return 0;

    if ((nmsg > 0) && (indexes != NULL) && (sort != SMS_SORT_NONE))
    {
        // Sort messages
        bool f;
        int32_t temp;
        time_t tempt;
        for (int32_t i = 0; i < nmsg; ++i)
        {
            for (int32_t j = i + 1; j < nmsg; ++j)
            {
                if (sort == SMS_SORT_ASC)
                    f = (indexes->time[i] > indexes->time[j]);
                else
                    f = (indexes->time[i] < indexes->time[j]);
                if (f)
                {
                    temp = indexes->idx[i];
                    tempt = indexes->time[i];
                    indexes->idx[i] = indexes->idx[j];
                    indexes->time[i] = indexes->time[j];
                    indexes->idx[j] = temp;
                    indexes->time[j] = tempt;
                }
            }
        }
    }
    return nmsg;
}


static int32_t user_uart_init(uart_port_t uart_num, int32_t tx, int32_t rx, int32_t rts, int32_t cts, int32_t bdr)
{
    uart_hw_flowcontrol_t flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    // Initialize the UART pins
    if (gpio_set_direction(tx, GPIO_MODE_OUTPUT))
        goto exit;
    if (gpio_set_direction(rx, GPIO_MODE_INPUT))
        goto exit;
    if (gpio_set_pull_mode(rx, GPIO_PULLUP_ONLY))
        goto exit;
    if ((rts >= 0) && (cts >= 0))
    {
        if (gpio_set_direction(rts, GPIO_MODE_OUTPUT))
            goto exit;
        if (gpio_set_direction(cts, GPIO_MODE_INPUT))
            goto exit;
        if (gpio_set_pull_mode(cts, GPIO_PULLUP_ONLY))
            goto exit;
        flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
    }
    else if (rts >= 0)
    {
        if (gpio_set_direction(rts, GPIO_MODE_OUTPUT))
            goto exit;
        flow_ctrl = UART_HW_FLOWCTRL_RTS;
    }
    else if (cts >= 0)
    {
        if (gpio_set_direction(cts, GPIO_MODE_INPUT))
            goto exit;
        if (gpio_set_pull_mode(cts, GPIO_PULLUP_ONLY))
            goto exit;
        flow_ctrl = UART_HW_FLOWCTRL_CTS;
    }

    uart_config_t uart_config = {
        .baud_rate = bdr,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = flow_ctrl};
    if (flow_ctrl & UART_HW_FLOWCTRL_RTS)
        uart_config.rx_flow_ctrl_thresh = UART_FIFO_LEN / 2;

    // Configure UART parameters
    if (uart_param_config(uart_num, &uart_config))
        goto exit;

    if (uart_set_pin(uart_num, tx, rx, rts, cts))
        goto exit;
    if (uart_driver_install(uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0))
        goto exit;
    return 0;

exit:
    return 1;
}


static int32_t gsm_init(void)
{
    int32_t gsmCmdIter = 0;
    int32_t nfail = 0;
    int32_t cmd_res = 0;

    while (gsmCmdIter < GSM_InitCmdsSize)
    {
        if (GSM_Init[gsmCmdIter]->skip)
        {
            if (debug)
            {
                infoCommand(GSM_Init[gsmCmdIter]->cmd, strlen(GSM_Init[gsmCmdIter]->cmd), "Skip command:");
            }
            gsmCmdIter++;
            continue ;
        }
        if ((GSM_Init[gsmCmdIter] == &cmd_Reg) && (allow_roaming))
        {
            cmd_res = atCmd_waitResponse(GSM_Init[gsmCmdIter]->cmd,
                                         GSM_Init[gsmCmdIter]->cmdResponseOnOk,
                                         "CREG: 0,5",
                                         GSM_Init[gsmCmdIter]->cmdSize,
                                         GSM_Init[gsmCmdIter]->timeoutMs,
                                         NULL,
                                         0,
                                         NULL);
        }
        else
        {
            cmd_res = atCmd_waitResponse(GSM_Init[gsmCmdIter]->cmd,
                                         GSM_Init[gsmCmdIter]->cmdResponseOnOk,
                                         NULL,
                                         GSM_Init[gsmCmdIter]->cmdSize,
                                         GSM_Init[gsmCmdIter]->timeoutMs,
                                         NULL,
                                         0,
                                         NULL);
        }
        if (cmd_res == 0)
        {
            // * No response or not as expected, start from first initialization command
            if (debug)
            {
                ESP_LOGW(TAG, "Wrong response, restarting...");
            }

            if (++nfail > 10) return 1;

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            gsmCmdIter = 0;
            continue ;
        }

        if ((GSM_Init[gsmCmdIter] == &cmd_Reg) && (allow_roaming))
        {
            if (cmd_res == 2)
            {
                if (debug)
                {
                    ESP_LOGW(TAG, "Connected in roaming");
                }
            }
        }

        if (GSM_Init[gsmCmdIter]->delayMs > 0)
            vTaskDelay(GSM_Init[gsmCmdIter]->delayMs / portTICK_PERIOD_MS);
        GSM_Init[gsmCmdIter]->skip = 1;
        if (GSM_Init[gsmCmdIter] == &cmd_Reg)
            GSM_Init[gsmCmdIter]->delayMs = 0;
        // Next command
        gsmCmdIter++;
    }

    if (debug)
    {
        ESP_LOGI(TAG, "GSM initialized.");
    }
    return 0;
}

// #endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/
