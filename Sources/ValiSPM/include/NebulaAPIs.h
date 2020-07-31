/*! \file NebulaAPIs.h
This file describes all the APIs of the IOTC module in IOTC platform.
IOTC module is a kind of data communication modules to provide basic data
transfer among devices and clients.

\copyright Copyright (c) 2010 by Throughtek Co., Ltd. All Rights Reserved.

Revision Table

Version     | Name             |Date           |Description
------------|------------------|---------------|-------------------
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_Initialize, Nebula_DeInitialize, Nebula_Device_New
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_Device_Login, Nebula_Device_Bind, Nebula_Device_Load_Settings
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_Device_Push_Notification, Nebula_Device_Delete, Nebula_Client_New
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_Client_New_From_Struct, Nebula_Client_New_From_String, Nebula_Client_To_String
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_Client_Bind, Nebula_Client_Free_Bind_Response, Nebula_Client_Send_Command
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_Client_Free_Send_Command_Response, Nebula_Device_Get_Sleep_Packet
4.0.0.0     |Terry Liu         |2019-08-07     |+ Add Nebula_Client_Wakeup_Device, Nebula_Client_Delete
 */
 
#ifndef __NebulaAPIs_H__
#define __NebulaAPIs_H__

#include "NebulaError.h"
#include "NebulaJsonAPIs.h"
#include "TUTKGlobalAPIs.h"

#ifdef _WIN32
    #ifdef IOTC_STATIC_LIB
        #define NEBULA_API
    #elif defined P2PAPI_EXPORTS
        #define NEBULA_API __declspec(dllexport)
    #else
        #define NEBULA_API __declspec(dllimport)
    #endif
#else // #ifdef _WIN32
    #define NEBULA_API
	#define __stdcall
#endif //#ifdef _WIN32




#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ============================================================================
 * Generic Macro Definition
 * ============================================================================
 */

#define MAX_PUBLIC_UDID_LENGTH 40
#define MAX_PIN_CODE_LENGTH 9
#define MAX_UDID_LENGTH 106
#define MAX_PROFILE_LENGTH 45000
#define MAX_REALM_LENGTH 128
#define MAX_NEBULA_PSK_LENGTH 1024
#define MAX_NEBULA_IDENTITY_LENGTH 119
#define MAX_NEBULA_SECRETID_LENGTH 128

#define NEBULA_MAX_SLEEP_ALIVE_PACKET_SIZE 256

#ifndef INET6_ADDRSTRLEN
    #define INET6_ADDRSTRLEN 46
#endif

/* ============================================================================
 * Type Definition
 * ============================================================================
 */

typedef void NebulaDeviceCtx;
typedef void NebulaClientCtx;
typedef void NebulaNodeCtx;


typedef enum NebulaSocketProtocol {
    NEBULA_PROTO_TCP,
    NEBULA_PROTO_UDP,
} NebulaSocketProtocol;

typedef struct NebulaWakeUpData {
    char ip[INET6_ADDRSTRLEN];
    unsigned short port;
    unsigned int packet_size; // Login packet length
    char sleep_alive_packet[NEBULA_MAX_SLEEP_ALIVE_PACKET_SIZE]; // Login packet data buffer
    unsigned int login_interval_sec; // Recommended send login packet interval
} NebulaWakeUpData;

typedef struct NebulaClientInfo {
    int version;
    char public_udid[MAX_PUBLIC_UDID_LENGTH + 1];    //need to include \0
    char psk[MAX_NEBULA_PSK_LENGTH + 1];             //need to include \0
    char identity[MAX_NEBULA_IDENTITY_LENGTH + 1];   //need to include \0
    char secret_id[MAX_NEBULA_SECRETID_LENGTH + 1];  //need to include \0
} NebulaClientInfo;

typedef enum
{
	NEBULA_DEVLOGIN_ST_CONNECTED       = 1 << 0, // get the login response from Nebula server
	NEBULA_DEVLOGIN_ST_DISCONNECTED    = 1 << 1, // disconnected from Nebula server, please check the network status
	NEBULA_DEVLOGIN_ST_RETRYLOGIN      = 1 << 2, // retry login to Nebula server
} NebulaDeviceLoginState;

/**
 * \brief The prototype of identity handle callback function.
 *
 * \details This callback function is called when Nebula module need to decrypt the data from Nebula client.
 *          Nebula device is required to provide the correspond psk when this callback is called.
 *
 * \param device [in] device's context generate by Nebula_Device_New()
 * \param identity [in] the identity from Nebula client
 * \param psk [out] the pre share key keep in device that is correspond with the specific identity
 * \param psk_size [in] size of psk.
 *
 * \see Nebula_Device_New()
 */
typedef void(__stdcall *NebulaIdentityHandleFn)(NebulaDeviceCtx *device, const char *identity, char *psk, unsigned int psk_size);


/**
 * \brief The prototype of command handle callback function.
 *
 * \details This callback function is called when Nebula client send request to Nebula device.
 *          Nebula device should handle the request and send an appropriate response.
 *
 * \param device [in] Device's context generate by Nebula_Device_New()
 * \param identity [in] The identity from Nebula client
 * \param fun [out] The json object name
 * \param args [out] The json object
 * \param response [in] The json response object
 *
 * \see Nebula_Device_New()
 */
typedef int(__stdcall *NebulaCommandHandleFn)(NebulaDeviceCtx *device, const char *identity, const char *func, const NebulaJsonObject *args, NebulaJsonObject **response);

/**
 * \brief The prototype of node command handle callback function.
 *
 * \details This callback function is called when Nebula client send request to Nebula node.
 *          Nebula node should handle the request and send an appropriate response.
 *
 * \param node [in] Node's context generate by Nebula_Device_New()
 * \param fun [out] The json object name
 * \param args [out] The json object
 * \param response [in] The json response object
 *
 * \see Nebula_Node_New()
 */
typedef int(__stdcall *NebulaNodeCommandHandleFn)(NebulaNodeCtx *node, const char *func, const NebulaJsonObject *args, NebulaJsonObject **response);

/**
 * \brief The prototype of deivce setting change handle callback function.
 *
 * \details This callback function is called when settings of Nebula device changed.
 *          Nebula device should have a safekeeping of settings and should use 
 *          Nebula_Device_Load_Settings() to load the settings when Nebula device restart next time.
 *
 * \param device [in] device's context generate by Nebula_Device_New()
 * \param settings [in] the encrypted settings string of Nebula device
 *
 * \see Nebula_Device_New() Nebula_Device_Load_Settings()
 */
typedef int(__stdcall *NebulaSettingsChangeHandleFn)(NebulaDeviceCtx* device, const char* settings);

/**
 * \brief The prototype of deivce login state callback function.
 *
 * \details This callback function is called when device login state changed.
 *
 * \param device [in] device's context generate by Nebula_Device_New()
 * \param state [in] the login state of Nebula device
 *
 * \see Nebula_Device_Login()
 */
typedef int(__stdcall *NebulaDeviceLoginStateFn)(NebulaDeviceCtx* device, NebulaDeviceLoginState state);

/**
 * \brief The prototype of client connect state callback function.
 *
 * \details This callback function is called when client connect state changed.
 *
 * \param client [in] client's context generate by Nebula_Client_New()
 * \param state [in] the connect state of Nebula client
 *
 * \see Nebula_Client_Connect()
 */
typedef void(__stdcall *NebulaClientConnectStateFn)(NebulaClientCtx *client, NebulaDeviceLoginState state);

/* ============================================================================
 * Function Declaration
 * ============================================================================
 */

/**
 * \brief Initialize Nebula module
 *
 * \details This function is used by devices to initialize Nebula module 
 *			and shall be called before any Nebula module related function
 *			is invoked.
 *
 * \return #NEBULA_ER_NoERROR if initializing successfully
 * \return Error code if return value < 0
 *          - #NEBULA_ER_RESOURCE_ERROR Getting system resource fail
 *
 * \see Nebula_DeInitialize()
 *
 * \attention   This function must be the first function to call, and 
 *              this call MUST have a corresponding call to Nebula_DeInitialize
 *              when the operation is complete.
 */
NEBULA_API int Nebula_Initialize();


/**
 * \brief Deinitialize Nebula module
 *
 * \details This function will deinitialize Nebula module and
 *          must be the last function to call for Nebula module
 *          This would release all the resource allocated in this module. 
 *
 * \return #NEBULA_ER_NoERROR if deinitialize successfully
 * \return Error code if return value < 0
 *			- #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *
 * \see Nebula_Initialize()
 *
 */
NEBULA_API int Nebula_DeInitialize();


/**
 * \brief Set Attribute of log file
 *
 * \param logAttr [in] See #LogAttr
 *
 * \return #NEBULA_ER_NoERROR on success.
 * \return The value < 0
 *			- #NEBULA_ER_INVALID_ARG   Invalid input argument.
 */
NEBULA_API int Nebula_Set_Log_Attr(LogAttr logAttr);


/**
 * \brief Generat Device context for Nebula module
 *
 * \details This function will generate Device context for Nebula module.
 *          Device API for Nebula module need this context to work.
 *
 * \param udid [in] device's udid for Nebula module
 * \param secret_id [in] device's secret_id for Nebula module
 * \param profile [in] device's profile for client to use.
 * \param command_handler [in] Nebula command handler callback function
 * \param identity_handler [in] identity handler callback function
 * \param settings_change_handler [in] setting change handler callback function
 * \param ctx[out]  device's context generate by Nebula_Device_New;
 *
 * \return #NEBULA_ER_NoERROR if generate device context successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The udid, secret_id, profile, command_handler, identity_handler, settings_change_handler or ctx is null or
 *               profile length larger than MAX_PROFILE_LENGTH
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_MEM_INSUFFICIENT Insufficient memory for allocation
 *            - #NEBULA_ER_FAIL_CREATE_THREAD Fails to create threads
 * \see Nebula_Device_Delete()
 *
 */
NEBULA_API int Nebula_Device_New(const char* udid, const char* secret_id, const char* profile,
								 NebulaCommandHandleFn command_handler, NebulaIdentityHandleFn identity_handler,
                                 NebulaSettingsChangeHandleFn settings_change_handler, NebulaDeviceCtx** ctx);


/**
 * \brief Device login to Bridge server.
 *
 * \details This function is used by devices to login to bridge server.
 *          Device can receive data from client that is binded by nebula module after logining
 *          to Bridge server. Device need to call this api before binding to client.
 *          
 *
 * \param ctx[in] device's context generate by Nebula_Device_New;
 *
 * \return #NEBULA_ER_NoERROR if login to Nebula server successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx is null
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_RESOURCE_ERROR Getting system resource fail
 *            - #NEBULA_ER_BRIDGE_SERVER_LOGIN_FAIL Login to bridge server fail
 *            - #NEBULA_ER_TIMEOUT Query bridge server timeout
 *            - #NEBULA_ER_HTTP_ERROR Query bridge server response error
 *
 * \see Nebula_Device_Bind()
 */
NEBULA_API int Nebula_Device_Login(NebulaDeviceCtx* ctx, NebulaDeviceLoginStateFn login_state_handler);


/**
 * \brief Device bind to a client
 *
 * \details This function is used by devices to binding client.
 *          Device bind to client with identity, pin code, avtoken
 *          and psk. This api can only being called once.
 *          Device need call Nebula_Device_Login before binding to client.
 *
 * \param ctx[in] device's context generate by Nebula_Device_New()
 * \param pin_code[in] device's pin code which for authenticating with client.
 * \param psk[in] device's psk to encode/decode data in Nebula module
 * \param timeout_msec[in] The timeout for this function in unit of millisecond, give 0 means block forever
 *
 * \return #NEBULA_ER_NoERROR if bind to a client successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx, pin_code, av_token or psk is null
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_BRIDGE_SERVER_NOT_LOGIN Device not login to bridge server yet
 *            - #NEBULA_ER_TIMEOUT Query bind server timeout
 *            - #NEBULA_ER_HTTP_ERROR Query or login bind Server response error
 *
 * \see Nebula_Client_Bind()
 *
 * \attention (1) Recommended value of timeout: 1000 millisecond ~ 30000 millisecond
 *            (2) Once the device bind to a sepecific client successfully, there is no need to do binding again
 *                when device restart.
 */
NEBULA_API int Nebula_Device_Bind(NebulaDeviceCtx* ctx, const char *pin_code, const char *psk, unsigned int timeout_msec, unsigned int *abort_flag);

/**
 * \brief Device generate bind message for client
 *
 * \details This function is used by devices to generate bind message.
 *          When Device get Nebula bind request from local client (BLE or AP mode LAN),
 *          this function can provide bind message to reaopnse.
 *
 * \param udid[in] device's udid for Nebula module
 * \param psk[in] device's psk to encode/decode data in Nebula module
 * \param secret_id[in] device's secret_id for Nebula module
 * \param bind_message_string[out] Nebula bind string for client
 *
 * \return #NEBULA_ER_NoERROR if generate message success
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The udid, psk , secret_id is null or length invalid
 *            - #NEBULA_ER_MEM_INSUFFICIENT Insufficient memory for allocation
 *
 * \see Nebula_Client_New_From_String()
 * 
 * \attention bind_message_string shall be free after use it
 *
 */
NEBULA_API int Nebula_Device_New_Local_Bind_Message(const char *udid, const char *psk, const char *secret_id, char **bind_message_string);

/**
 * \brief Nebula device load settings
 *
 * \details This function is used by device to load settings.
 *
 *
 * \param ctx[in] device's context generate by Nebula_Device_New()
 * \param settings[in] the encrypted settings string of Nebula device
 *
 * \return #NEBULA_ER_NoERROR if load settings successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx or settings is null
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *
 * \see NebulaSettingsChangeHandleFn
 *
 */
NEBULA_API int Nebula_Device_Load_Settings(NebulaDeviceCtx* ctx, const char* settings);


/**
 * \brief Nebula device push a notification to server
 *
 * \details This function is used by device to push a notification to server
 *          when some event happened and device want to notify Nebula clients
 *          with event messages.
 *
 * \param ctx[in] device's context generate by Nebula_Device_New()
 * \param notification_obj[in] The json object contains a list of key value pair for push server to generate the push message.
 *                             The key and value should be a string.
 * \param timeout_msec[in] The timeout for this function in unit of millisecond, give 0 means block forever
 *
 * \return #NEBULA_ER_NoERROR if bind to a client successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx or notification_obj is null
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_BRIDGE_SERVER_NOT_LOGIN Device not login to bridge server yet
 *            - #NEBULA_ER_TIMEOUT Push notification to server timeout
 *            - #NEBULA_ER_HTTP_ERROR Http error occurred when push notification to server
 *
 *
 * \attention (1) Recommended value of timeout: 1000 millisecond ~ 30000 millisecond
 */
NEBULA_API int Nebula_Device_Push_Notification(NebulaDeviceCtx* ctx, NebulaJsonObject *notification_obj, 
                                               unsigned int timeout_msec, unsigned int *abort_flag);


/**
 * \brief Release device context
 *
 * \details This function will free device context that created by Nebula_Device_New()
 *
 * \param ctx[in] device's context generate by Nebula_Device_New;
 *
 * \return #NEBULA_ER_NoERROR if delete device context successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG  The input ctx is invalid.
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 * 
 * \see Nebula_Device_New()
 *
 */
NEBULA_API int Nebula_Device_Delete(NebulaDeviceCtx* ctx);


/**
 * \brief Generat client context for Nebula module at first time.
 *
 * \details This function will generate client context for Nebula module.
 *          Client API for Nebula module need this context to work.
 *          Client need to call this api if never bind to device before.
 *          User can call Nebula_Client_New_From_String replace this api
 *          if user already bind to device. 
 *
 * \param public_udid [in] device's public_udid for Nebula module
 * \param ctx[out]  client's context generate by Nebula_Client_New or Nebula_Client_New_From_String;
 *
 * \return #NEBULA_ER_NoERROR if generate client context successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx or public_udid is null
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_MEM_INSUFFICIENT Insufficient memory for allocation
 *
 * \see Nebula_Device_Delete()
 *
 */
NEBULA_API int Nebula_Client_New(const char* public_udid, NebulaClientCtx** ctx);

/**
 * \brief Generate client context with device's information for Nebula module
 *
 * \details This function will generate client context for Nebula module.
 *          Client API for Nebula module need this context to work.
 *          Client call this api if have bind to device and get device'information before. 
 *
 * \param NebulaClientInfo [in] the necessary informations from Nebula device
 * \param ctx[out]  client's context generate by Nebula_Client_New or Nebula_Client_New_From_String;
 *
 * \return #NEBULA_ER_NoERROR if generate client context successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx or client_info is null or invalid UDID
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_MEM_INSUFFICIENT Insufficient memory for allocation
 *
 */
NEBULA_API int Nebula_Client_New_From_Struct(NebulaClientInfo *client_info, NebulaClientCtx **ctx);

/**
 * \brief Generate client context with device's information for Nebula module
 *
 * \details This function will generate client context for Nebula module.
 *          Client API for Nebula module need this context to work.
 *          Client call this api if have bind to device and get device'information before. 
 *
 * \param public_udid [in] device's public_udid for Nebula module
 * \param string_data [in] client's information that return by Nebula_Client_To_String.
 * \param ctx[out]  client's context generate by Nebula_Client_New or Nebula_Client_New_From_String;
 *
 * \return #NEBULA_ER_NoERROR if generate client context successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx or string_data is null or parsing string_data return null
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_MEM_INSUFFICIENT Insufficient memory for allocation
 *
 * \see Nebula_Client_To_String()
 *
 */
NEBULA_API int Nebula_Client_New_From_String(const char* public_udid, const char* string_data, NebulaClientCtx** ctx);


/**
 * \brief Generating device's information for client which used to create client's context.
 *
 * \details This function will generate device's information in json string after bind to device successfully. 
 *          This api must be used after calling Nebula_Client_Bind.
 *
 * \param ctx[in]  client's context generate by Nebula_Client_New or Nebula_Client_New_From_String;
 *
 * \return null if ctx's psk, secret_id or identity is null
 *
 * \see Nebula_Device_Delete()
 *
 */
NEBULA_API char *Nebula_Client_To_String(NebulaClientCtx* ctx);


/**
 * \brief Pairing Nebula Client and Device
 *
 * \details This function is used when Nebula client want to bind a Nebula device.
 *          Device and client need to use same pin code when doing binding process,
 *          after bind success, client will get infomations that is needed for establish
 *          a connection to device.
 *
 * \param client_ctx [in] Nebula context of client, it's from Nebula_Client_New() or Nebula_Client_New_From_String()
 * \param pin_code [in] Same PIN code of Nebula_Device_Bind()
 * \param bind_response [out] The bind response json object,this object has identity, avToken and authKey
 * \param timeout_msec [in] The timeout for this function in unit of millisecond, give 0 means block forever
 *
 * \return #NEBULA_ER_NoERROR if bind to a device successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx, pin_code or json_response is null
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_BIND_SERVER_LOGIN_FAIL Failed to login to binding server
 *            - #NEBULA_ER_EXCEED_BUFFER_SIZE If generate json string size larger than response_buf_size
 *            - #NEBULA_ER_TIMEOUT Bind or query bind server timeout
 *            - #NEBULA_ER_HTTP_ERROR Bind or query bind Server response error
 *
 * \see Nebula_Device_Bind(), Nebula_Client_New(), Nebula_Client_New_From_String()
 *
 * \attention (1) Recommended value of timeout: 1000 millisecond ~ 30000 millisecond
 *            (2) Once the client bind to a sepecific device successfully, there is no need to do binding again
 *                when client want to connect to the device next time.
 */
NEBULA_API int Nebula_Client_Bind(NebulaClientCtx* ctx, const char *pin_code,
                                  NebulaJsonObject **bind_response,
                                  unsigned int timeout_msec, unsigned int *abort_flag);


/**
 * \brief Release response data of Nebula_Client_Bind
 * 
 * \details  This function is used to release json response data from Nebula_Client_Bind,
 *           you must call it after you got the json response data from Nebula_Client_Bind
 * 
 * \param bind_response [in] This json response data pointer
 * 
 * \return #Nebula_ER_NoERROR if free json response data pointer successfully
 * \return Error code if return value < 0
 *            - #Nebula_ER_INVALID_ARG The bind_response in null
 */
NEBULA_API int Nebula_Client_Free_Bind_Response(NebulaJsonObject *bind_response);

/**
 * \brief Client connect to Bridge server.
 *
 * \details This function is used by client to connect to bridge server.
 *          Client will create persistent connection with bridge server for reduce delay of send command
 *          This function is unnecessary before Nebula_Client_Send_Command()
 *          
 * \param ctx[in] client's context generate by Nebula_Client_New()
 * \param connect_state_handler[in] this function will be call when connect status change
 *
 * \return #NEBULA_ER_NoERROR if login to Nebula server successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx is null
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_LOGIN_ALREADY_CALLED This function already called
 *            - #NEBULA_ER_FAIL_CREATE_THREAD Fails to create threads
 *
 * \see Nebula_Client_New() ,Nebula_Client_New_From_Struct(), Nebula_Client_New_From_String(), Nebula_Client_Send_Command()
 */
NEBULA_API int Nebula_Client_Connect(NebulaClientCtx *ctx, NebulaClientConnectStateFn connect_state_handler);

/**
 * \brief Send Nubula command message to Device
 *
 * \details This function is used by Nebula client to send a command in json format.
 *          User can get response from Nebula device through the output response buffer.
 *          This function is not support for Nebula device currently. 
 *
 * \param client_ctx [in] Nebula context of client, it's from Nebula_Client_New() or Nebula_Client_New_From_String()
 * \param request [in] JSON string of Nebula request
 * \param response [out] The bind response json object
 * \param timeout_msec [in] The timeout for this function in unit of millisecond, give 0 means block forever
 *
 * \return #NEBULA_ER_NoERROR if exchange successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx, request or response is null.Or parsing request return null
 *            - #NEBULA_ER_NOT_INITIALIZE The Nebula module is not initialized yet
 *            - #NEBULA_ER_CLIENT_NOT_BIND_TO_DEVICE Client is not bind to device yet
 *            - #NEBULA_ER_TIMEOUT Query or connect bridge server timeout
 *            - #NEBULA_ER_RESOURCE_ERROR Getting system resource fail
 *            - #NEBULA_ER_BRIDGE_SERVER_LOGIN_FAIL Login to bridge server fail
 *            - #NEBULA_ER_DEVICE_OFFLINE Device offline
 *            - #NEBULA_ER_DEVICE_SLEEPING Device sleeping
 *            - #NEBULA_ER_DEVICE_AWAKENING Device awakening
 *
 * \attention (1) Recommended value of timeout: 1000 millisecond ~ 30000 millisecond
 */
NEBULA_API int Nebula_Client_Send_Command(NebulaClientCtx* ctx, const char *request, NebulaJsonObject **response,  unsigned int timeout_msec, unsigned int *abort_flag);

/**
 * \brief Release response data of Nebula_Client_Send_Command
 * \details  This function is used to release json response data from Nebula_Client_Send_Command,
 *           you must call it after you got the json response data from Nebula_Client_Send_Command
 * 
 * \param response [in] This json response data pointer
 * 
 * \return #Nebula_ER_NoERROR if free json response data pointer successfully
 * \return Error code if return value < 0
 *            - #Nebula_ER_INVALID_ARG The response in null
 * 
 */
NEBULA_API int Nebula_Client_Free_Send_Command_Response(NebulaJsonObject *response);

/**
 * \brief Get packet to keep alive when device sleeping
 * \details  This function is used to get keep alive packet information when device sleeping
 * 
 * \param ctx [in] device's context generate by Nebula_Device_New
 * \param pattern [in] The wakeup pattern of device, see #Nebula_Client_Wakeup_Device()
 * \param pattern_size [in] The size fo wakeup pattern
 * \param protocol [in] The protocol to send sleep packet
 * \param data [out] The keep alive packet information, see #NebulaWakeUpData
 * \param timeout_msec [in] The timeout for this function in unit of millisecond, give 0 means block forever
 * 
 * \return #NEBULA_ER_NoERROR if get the sleep packet successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx is null or protocol invalid
 *            - #NEBULA_ER_NOT_INITIALIZE The Nebula module is not initialized yet
 *            - #NEBULA_ER_TIMEOUT Get sleep packet timeout
 * 
 */
NEBULA_API int Nebula_Device_Get_Sleep_Packet(NebulaDeviceCtx* ctx, unsigned char* pattern,
        unsigned int pattern_size, NebulaSocketProtocol protocol, NebulaWakeUpData** data,
        unsigned int* data_count, unsigned int timeout_ms);

/**
 * \brief Release wake up data from Nebula_Device_Get_Sleep_Packet
 * 
 * \details  This function is used to release wake up data from Nebula_Device_Get_Sleep_Packet,
 *           you must call it after you got the wake up data from Nebula_Device_Get_Sleep_Packet
 * 
 * \param data [in] The wake up data pointer
 * 
 * \return #Nebula_ER_NoERROR if free wake up data successfully
 * \return Error code if return value < 0
 *            - #Nebula_ER_INVALID_ARG The data is null pointer
 */
NEBULA_API int Nebula_Device_Free_Sleep_Packet(NebulaWakeUpData* data);

/**
 * \brief Wake up sleep device
 *
 * \details This function will wakeup sleeping device by sending wakeup pattern in
 *          #Nebula_Device_Get_Sleep_Packet()
 *
 * \param ctx [in] Nebula context of client, it's from Nebula_Client_New() or Nebula_Client_New_From_String()
 * \param timeout_msec [in] The timeout for this function in unit of millisecond, give 0 means block forever
 *
 * \return #NEBULA_ER_NoERROR if wakeup device successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx is null
 *            - #NEBULA_ER_NOT_INITIALIZE The Nebula module is not initialized yet
 *            - #NEBULA_ER_CLIENT_NOT_BIND_TO_DEVICE Client is not bind to device yet
 *            - #NEBULA_ER_TIMEOUT Wakeup deivce timeout
 *
 */
NEBULA_API int Nebula_Client_Wakeup_Device(NebulaClientCtx* ctx, unsigned int timeout_msec, unsigned int *abort_flag);


/**
 * \brief Release client context
 *
 * \details This function will free client context that created by Nebula_Device_New()
 *
 * \param client_ctx [in] Nebula context of client, it's from Nebula_Client_New() or Nebula_Client_New_From_String()
 *
 * \return #NEBULA_ER_NoERROR if delete client context successfully
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx is null
 *            - #NEBULA_ER_NOT_INITIALIZE The Nebula module is not initialized yet
 *            - #NEBULA_ER_DEVICE_ONLINE Device already awakened
 *            - #NEBULA_ER_DEVICE_OFFLINE Device offline
 *
 * \see Nebula_Client_New(), Nebula_Client_New_From_String()
 */
NEBULA_API int Nebula_Client_Delete(NebulaClientCtx* ctx);

/**
 * \brief Create Nebula node
 *
 * \details Create Node context for Nebula module.
 *          Node API for Nebula module need this context to work.
 *
 * \param udid [in] node's udid for Nebula module
 * \param profile [in] node's profile for client to use.
 * \param command_handler [in] Nebula command handler callback function
 * \param node_ctx [out] node's context created
 *
 * \return #NEBULA_ER_NoERROR if node context is successfully created
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG udid, profile, command_handler, identity_handler is null or
 *               profile length larger than MAX_PROFILE_LENGTH
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_MEM_INSUFFICIENT Insufficient memory for allocation
 * \see Nebula_Node_Delete()
 *
 */
NEBULA_API int Nebula_Node_New(const char* udid, const char* profile, NebulaNodeCommandHandleFn command_handler, NebulaNodeCtx** node_ctx);

/**
 * \brief Destroy Nebula node
 *
 * \details Free node context that created by Nebula_Node_New()
 *
 * \param node_ctx [in] Nebula context of node
 *
 * \return #NEBULA_ER_NoERROR if node context is successfully deleted
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG The ctx is null
 *            - #NEBULA_ER_NOT_INITIALIZE The Nebula module is not initialized yet
 *
 * \see Nebula_Node_New()
 */
NEBULA_API int Nebula_Node_Delete(NebulaNodeCtx* node_ctx);

/**
 * \brief Add node to device
 *
 * \details Add node for receiving forward commands.
 *
 * \param device_ctx[in] The device's context created by Nebula_Device_New()
 * \param node_ctx[in] The node's context created by Nebula_Node_New()
 * \param timeout_msec[in] The timeout for this function in unit of millisecond, give 0 means block forever
 *
 * \return #NEBULA_ER_NoERROR if node is successfully added to device
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_MEM_INSUFFICIENT Insufficient memory for allocation
 *            - #NEBULA_ER_TIMEOUT Add node timeout
 *            - #NEBULA_ER_HTTP_ERROR Http error
 *            - #NEBULA_ER_DUPLICATE The udid of node already exists
 *
 * \see Nebula_Node_New(), Nebula_Device_Find_Node(), Nebula_Device_Remove_Node()
 */
NEBULA_API int Nebula_Device_Add_Node(NebulaDeviceCtx* device_ctx, NebulaNodeCtx* node_ctx, unsigned int timeout_msec, unsigned int *abort_flag);

/**
 * \brief Get node in deivce
 *
 * \details Get node's context with specified udid
 * \param device_ctx[in] The device's context created by Nebula_Device_New()
 * \param udid[in] The udid of node
 * \param node_ctx[out] The node's context with specified udid
 *
 * \return #NEBULA_ER_NoERROR if the node's context exists
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_NO_SUCH_ENTRY  No such node's context
 *
 * \see Nebula_Node_New(), Nebula_Device_Add_Node(), Nebula_Device_Remove_Node()
 */
NEBULA_API int Nebula_Device_Find_Node(NebulaDeviceCtx* device_ctx, const char* udid, NebulaNodeCtx** node_ctx);

/**
 * \brief Remove node from device
 *
 * \details Remove node's context from device
 * \param device_ctx[in] The device's context created by Nebula_Device_New()
 * \param node_ctx[in] The node's context created by Nebula_Node_New()
 *
 * \return Error code if return value < 0
 *            - #NEBULA_ER_INVALID_ARG
 *            - #NEBULA_ER_NOT_INITIALIZE  The Nebula module is not initialized yet
 *            - #NEBULA_ER_NO_SUCH_ENTRY  No such node's context
 *
 * \see Nebula_Node_New(), Nebula_Device_Add_Node(), Nebula_Device_Find_Node()
 */
NEBULA_API int Nebula_Device_Remove_Node(NebulaDeviceCtx* device_ctx, NebulaNodeCtx* node_ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NebulaAPIs_H__ */
