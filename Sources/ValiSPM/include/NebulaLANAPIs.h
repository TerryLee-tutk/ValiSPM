/*! \file NebulaLANAPIs.h
This file describes all the APIs of the IOTC module in IOTC platform.
IOTC module is a kind of data communication modules to provide basic data
transfer among devices and clients.

\copyright Copyright (c) 2010 by Throughtek Co., Ltd. All Rights Reserved.

Revision Table

Version     | Name             |Date           |Description
------------|------------------|---------------|-------------------
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_WiFi_Setup_Start_On_LAN, Nebula_Device_Listen_On_LAN
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_App_Search_UDID_On_LAN, Nebula_App_Request_TCP_Connect_On_LAN
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_WiFi_Setup_Stop_On_LAN, Nebula_Send_IOCtrl_On_LAN
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_Recv_IOCtrl_From_LAN
 */
 
#ifndef _NEBULA_LANAPIs_H_
#define _NEBULA_LANAPIs_H_

#include "NebulaWiFiConfig.h"
#include "NebulaAPIs.h"
#include "NebulaError.h"

/* ============================================================================
 * Platform Dependant Macro Definition
 * ============================================================================
 */

#define NEBULA_LAN_API

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ============================================================================
 * Generic Macro Definition
 * ============================================================================
 */

#define MAX_DEVICE_NAME_LENGTH 128


/* ============================================================================
 * Enumeration Declaration
 * ============================================================================
 */

  typedef enum{
    DEVICE,
    CLIENT
  } LanSearchRole;


/* ============================================================================
 * Structure Definition
 * ============================================================================
 */

  typedef struct _st_UDIDInfo
  {
    char udid[MAX_PUBLIC_UDID_LENGTH + 1];
    char device_name[MAX_DEVICE_NAME_LENGTH + 1];
  } st_UDIDInfo;


/* ============================================================================
 * Function Declaration
 * ============================================================================
 */

/**
 * \brief Start to lan search without UID for WiFi setup
 *
 * \details Start to lan search for WiFi setup. After calling this api, user can listen or
 *          request tcp connect to create tcp connection.
 *
 * \param role [in] role of lan search.
 * \param searchable [in] This device could be searched with empty search name or not.
 *
 * \return 0 if start lan search successfully.
 * \return Error code if return value < 0
 *          - #NEBULA_ER_INVALID_ARG if value of role is invalid.
 *
 */

NEBULA_LAN_API int Nebula_WiFi_Setup_Start_On_LAN(LanSearchRole role, int searchable);

/**
 * \brief Device can listen for tcp request on lan or response to lan search with search id.
 *
 * \details Devcie will start listen for tcp request and create connection on LAN
 *          after calling this api. 
 *
 * \param udid [in] The udid of Device.
 * \param pwd [in] The pwd of Device. This param could be null, if device don't want to use.
 * \param timeout_ms [in] timeout in miliseconds for this api.
 *
 * \return 0 if tcp connection create successfully.
 * \return Error code if return value < 0
 *          - #NEBULA_ER_INVALID_ARG if value of role is invalid.
 *
 */

NEBULA_LAN_API int Nebula_Device_Listen_On_LAN(const char* udid, const char* pwd, const char* device_name, uint16_t timeout_ms);

/**
 * \brief Used by client for searching devices's UDID on LAN.
 *
 * \details  When clients and devices are stay in a LAN environment, client can call this function
 *      to discovery devices.
 *
 * \param udid_array [in] The array of udid to store search result.
 * \param array_cnt [in] The size of the udid array.
 * \param timeout_ms [in] The timeout in miliseconds before discovery process end.
 *
 * \return The number of devices found.
 * \return Error code if return value < 0
 *          - #NEBULA_ER_INVALID_ARG if value of role is invalid.
 *
 */


NEBULA_LAN_API int Nebula_App_Search_UDID_On_LAN(st_UDIDInfo *udid_array, uint16_t array_cnt, uint16_t timeout_ms);

/**
 * \brief Used by client for connect to devices with UDID on LAN.
 *
 * \details  When clients and devices are stay in a LAN environment, client can call this function
 *      to connect to device with TCP.
 *
 * \param udid [in] The udid of Device.
 * \param pwd [in] The pwd of Device. This param could be null, if device not support.
 * \param timeout_ms [in] The timeout in miliseconds before discovery process end.
 *
 * \return 0 if tcp connection create successfully.
 * \return Error code if return value < 0
 *          - #NEBULA_ER_INVALID_ARG if value of role is invalid.
 *
 */

NEBULA_LAN_API int Nebula_App_Request_TCP_Connect_On_LAN(const char * udid, const char* pwd, uint16_t timeout_ms);

/**
 * \brief Stop to lan search for WiFi setup
 *
 * \details Stop to lan search for WiFi setup. This api must be called, if 
 *          NEBULA_WiFi_Setup_Start_On_LAN has been called. This api will close tcp connection and 
 *          free memory.
 *
 */

NEBULA_LAN_API void Nebula_WiFi_Setup_Stop_On_LAN();

/**
 * \brief Send WiFi setup IO control
 *
 * \details This function is used by devices or clients to send a
 *      WiFi setup IO control.
 *
 * \param type [in] The type of IO control.
 * \param ioctrl_buf [in] The buffer of IO control data
 * \param ioctrl_len [in] The length of IO control data
 *
 * \return #AV_ER_NoERROR if sending successfully
 * \return Error code if return value < 0
 *
 *
 */

NEBULA_LAN_API int Nebula_Send_IOCtrl_On_LAN(NebulaIOCtrlType type, const char * ioctrl_buf, uint16_t ioctrl_len);

/**
 * \brief Receive WiFi setup IO control
 *
 * \details This function is used by devices or clients to receive a
 *      WiFi setup io control.
 *
 * \param type [out] The type of IO control.
 * \param result_buf [in] The buffer of received IO control data.
 * \param buf_size [in] The max length of buffer of received IO control data.
 * \param timeout_ms [out] The timeout_ms for this function in unit of million-second, give 0 means return immediately.
 *
 * \return The actual length of received result stored in abIOCtrlData if
 *      receiving IO control successfully
 * \return Error code if return value < 0
 *      - #AV_ER_INVALID_ARG The AV channel ID is not valid or IO control type
 *        / data is null
 *
 */

NEBULA_LAN_API int Nebula_Recv_IOCtrl_From_LAN(NebulaIOCtrlType* type, char *result_buf, uint16_t buf_size, uint16_t timeout_ms);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _NEBULA_LANAPIs_H_ */
