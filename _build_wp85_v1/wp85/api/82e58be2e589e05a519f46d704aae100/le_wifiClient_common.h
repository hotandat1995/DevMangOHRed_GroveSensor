
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_WIFICLIENT_COMMON_H_INCLUDE_GUARD
#define LE_WIFICLIENT_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_wifiDefs_common.h"

#define IFGEN_LE_WIFICLIENT_PROTOCOL_ID "b621c67ff40d02c70b733bd6aa455c67"
#define IFGEN_LE_WIFICLIENT_MSG_SIZE 252



//--------------------------------------------------------------------------------------------------
/**
 * Value for no signal strength.
 */
//--------------------------------------------------------------------------------------------------
#define LE_WIFICLIENT_NO_SIGNAL_STRENGTH 32767

//--------------------------------------------------------------------------------------------------
/**
 * Reference type for AccessPoint that is returned by the WiFi Scan.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_wifiClient_AccessPoint* le_wifiClient_AccessPointRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * WiFi Client Events.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_WIFICLIENT_EVENT_CONNECTED = 0,
        ///< WiFi Client Connected
    LE_WIFICLIENT_EVENT_DISCONNECTED = 1,
        ///< WiFi Client Disconnected
    LE_WIFICLIENT_EVENT_SCAN_DONE = 2,
        ///< WiFi Scan result for available Access Points available
    LE_WIFICLIENT_EVENT_SCAN_FAILED = 3
        ///< WiFi Scan failed
}
le_wifiClient_Event_t;


//--------------------------------------------------------------------------------------------------
/**
 * WiFi Client disconnect cause type enum.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_WIFICLIENT_UNKNOWN_CAUSE = 0,
        ///< Default value, could be connected
    LE_WIFICLIENT_CLIENT_REQUEST = 1,
        ///< WiFi Client request to disconnect
    LE_WIFICLIENT_HARDWARE_DETACHED = 2,
        ///< WiFi card is detached
    LE_WIFICLIENT_HARDWARE_STOP = 3,
        ///< WiFi driver has been removed
    LE_WIFICLIENT_BEACON_LOSS = 4,
        ///< Beacon loss due to poor SINR
    LE_WIFICLIENT_BY_AP = 5
        ///< Disconnected by AP
}
le_wifiClient_DisconnectionCause_t;


//--------------------------------------------------------------------------------------------------
/**
 * WiFi Client Security Protocol for connection
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_WIFICLIENT_SECURITY_NONE = 0,
        ///< no security.
    LE_WIFICLIENT_SECURITY_WEP = 1,
        ///< Using WEP.
    LE_WIFICLIENT_SECURITY_WPA_PSK_PERSONAL = 2,
        ///< Using WPA
    LE_WIFICLIENT_SECURITY_WPA2_PSK_PERSONAL = 3,
        ///< Using WPA2
    LE_WIFICLIENT_SECURITY_WPA_EAP_PEAP0_ENTERPRISE = 4,
        ///< Using WPA Enterprise
    LE_WIFICLIENT_SECURITY_WPA2_EAP_PEAP0_ENTERPRISE = 5
        ///< Using WPA2 Enterprise
}
le_wifiClient_SecurityProtocol_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_wifiClient_NewEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_wifiClient_NewEventHandler* le_wifiClient_NewEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * WiFi event indication structure. The disconnectionCause only applies to event EVENT_DISCONNECTED.
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    le_wifiClient_Event_t event;
    le_wifiClient_DisconnectionCause_t disconnectionCause;
    char ifName[31 + 1];
    char apBssid[18 + 1];
}
le_wifiClient_EventInd_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_wifiClient_ConnectionEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_wifiClient_ConnectionEventHandler* le_wifiClient_ConnectionEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for WiFi Client changes
 * @deprecated le_wifiClient_AddNewEventHandler() will be removed in near future.
 * It is replaced by le_wifiClient_AddConnectionEventHandler().
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_wifiClient_NewEventHandlerFunc_t)
(
        le_wifiClient_Event_t event,
        ///< Handles the wifi events
        void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for WiFi Client connection changes
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_wifiClient_ConnectionEventHandlerFunc_t)
(
        const le_wifiClient_EventInd_t * LE_NONNULL wifiEventIndPtr,
        ///< EventInd Reference.
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_wifiClient_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_wifiClient_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_wifiClient_NewEvent'
 *
 * This event provide information on WiFi Client event changes.
 * NewEvent will be deprecated.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_wifiClient_NewEventHandlerRef_t ifgen_le_wifiClient_AddNewEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_NewEventHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_wifiClient_NewEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_wifiClient_RemoveNewEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_NewEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_wifiClient_ConnectionEvent'
 *
 * This event provide information on WiFi Client connection event changes.
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_wifiClient_ConnectionEventHandlerRef_t ifgen_le_wifiClient_AddConnectionEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_ConnectionEventHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_wifiClient_ConnectionEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_wifiClient_RemoveConnectionEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_ConnectionEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Start the WIFI device.
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 *      - LE_BUSY   The WIFI device is already started.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_Start
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop the WIFI device.
 *
 * @return
 *      - LE_OK        Function succeeded.
 *      - LE_FAULT     Function failed.
 *      - LE_DUPLICATE The WIFI device is already stopped.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_Stop
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Start Scanning for WiFi Access points
 * Will result in event LE_WIFICLIENT_EVENT_SCAN_DONE when the scan results are available.
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 *      - LE_BUSY   Scan already running.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_Scan
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the first WiFi Access Point found.
 *
 * @return
 *      - WiFi  Access Point reference if ok.
 *      - NULL  If no Access Point reference available.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_wifiClient_AccessPointRef_t ifgen_le_wifiClient_GetFirstAccessPoint
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the next WiFi Access Point.
 * Will return the Access Points in the order of found.
 * This function must be called in the same context as the GetFirstAccessPoint
 *
 * @return
 *      - WiFi  Access Point reference if ok.
 *      - NULL  If no Access Point reference available.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_wifiClient_AccessPointRef_t ifgen_le_wifiClient_GetNextAccessPoint
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the signal strength of the AccessPoint
 *
 * @return
 *      - Signal strength in dBm. Example -30 = -30dBm
 *      - If no signal available it will return LE_WIFICLIENT_NO_SIGNAL_STRENGTH
 *
 * @note The function returns the signal strength as reported at the time of the scan.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED int16_t ifgen_le_wifiClient_GetSignalStrength
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef
        ///< [IN] WiFi Access Point reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Basic Service set identifier (BSSID) of the AccessPoint
 *
 * @return
 *         LE_OK            Function succeeded.
 *         LE_FAULT         Function failed.
 *         LE_BAD_PARAMETER Invalid parameter.
 *         LE_OVERFLOW      bssid buffer is too small to contain the BSSID.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_GetBssid
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
        char* bssid,
        ///< [OUT] The BSSID
        size_t bssidSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Service set identification (SSID) of the AccessPoint
 *
 * @return
 *        LE_OK            Function succeeded.
 *        LE_FAULT         Function failed.
 *        LE_BAD_PARAMETER Invalid parameter.
 *        LE_OVERFLOW      ssid buffer is too small to contain the SSID.
 *
 * @note The SSID does not have to be human readable ASCII values, but often is.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_GetSsid
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
        uint8_t* ssidPtr,
        ///< [OUT] The SSID returned as a octet array.
        size_t* ssidSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the currently selected connection to be established
 *
 * @return
 *      - LE_OK upon successful retrieval of the selected SSID to be connected
 *      - LE_FAULT upon failure to retrieve it
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_wifiClient_GetCurrentConnection
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t* apRefPtr
        ///< [OUT] currently selected connection's AP reference
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the passphrase used to generate the PSK.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 *      - LE_BAD_PARAMETER Invalid parameter.
 *
 * @note The difference between le_wifiClient_SetPreSharedKey() and this function
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_SetPassphrase
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
        const char* LE_NONNULL PassPhrase
        ///< [IN] pass-phrase for PSK
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Pre Shared Key, PSK.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          Function failed.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 *
 * @note This is one way to authenticate against the access point. The other one is provided by the
 * le_wifiClient_SetPassPhrase() function. Both ways are exclusive and are effective only when used
 * with WPA-personal authentication.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_SetPreSharedKey
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
        const char* LE_NONNULL PreSharedKey
        ///< [IN] PSK. Note the difference between PSK and
        ///< Pass Phrase.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the security protocol for connection
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          Function failed.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_SetSecurityProtocol
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
        le_wifiClient_SecurityProtocol_t securityProtocol
        ///< [IN] Security Mode
);

//--------------------------------------------------------------------------------------------------
/**
 * WPA-Enterprise requires a username and password to authenticate.
 * This function sets these parameters.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          Function failed.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_SetUserCredentials
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
        const char* LE_NONNULL userName,
        ///< [IN] UserName used for WPA-Enterprise.
        const char* LE_NONNULL password
        ///< [IN] Password used for WPA-Enterprise.
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the WEP key (WEP)
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_SetWepKey
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
        const char* LE_NONNULL wepKey
        ///< [IN] The WEP key
);

//--------------------------------------------------------------------------------------------------
/**
 * This function specifies whether the target Access Point is hiding its presence from clients or
 * not. When an Access Point is hidden, it cannot be discovered by a scan process.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 *
 * @note By default, this attribute is not set which means that the client is unable to connect to
 * a hidden access point. When enabled, the client will be able to connect to the access point
 * whether it is hidden or not.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_SetHiddenNetworkAttribute
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef,
        ///< [IN] WiFi Access Point reference.
        bool hidden
        ///< [IN] If TRUE, the WIFI client will be able to connect to a hidden access point.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function creates a reference to an Access Point given its SSID.
 * If an Access Point is hidden, it will not show up in the scan. So, its SSID must be known
 * in advance in order to create a reference.
 *
 * @return
 *      - AccessPoint reference to the current Access Point.
 *
 * @note This function fails if called while scan is running.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_wifiClient_AccessPointRef_t ifgen_le_wifiClient_Create
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const uint8_t* SsidPtr,
        ///< [IN] The SSID as a octet array.
        size_t SsidSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Deletes an accessPointRef.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 *      - LE_BUSY           Function called during scan.
 *
 * @note The handle becomes invalid after it has been deleted.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_Delete
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef
        ///< [IN] WiFi Access Point reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Connect to the WiFi Access Point.
 * All authentication must be set prior to calling this function.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 *
 * @note For PSK credentials see le_wifiClient_SetPassphrase() or le_wifiClient_SetPreSharedKey() .
 * @note For WPA-Enterprise credentials see le_wifiClient_SetUserCredentials()
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_Connect
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_wifiClient_AccessPointRef_t accessPointRef
        ///< [IN] WiFi Access Point reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * Disconnect from the current connected WiFi Access Point.
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_Disconnect
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Load the given SSID's configurations as it is selected as the connection to be established,
 * after creating for it an AP reference
 *
 * @return
 *      - LE_OK     Function succeeded.
 *      - LE_FAULT  Function failed.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_LoadSsid
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const uint8_t* ssidPtr,
        ///< [IN] SSID which configs are to be installed
        size_t ssidSize,
        ///< [IN]
        le_wifiClient_AccessPointRef_t* apRefPtr
        ///< [OUT] reference to be created
);

//--------------------------------------------------------------------------------------------------
/**
 * Configure the given SSID to use WEP and the given WEP key in the respective input argument.
 * The WEP key is a mandatory input to be provided.
 *
 * @return
 *      - LE_OK     Succeeded to configure the givwn WEP key for the given SSID.
 *      - LE_FAULT  Failed to configure.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_ConfigureWep
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const uint8_t* ssidPtr,
        ///< [IN] SSID which configs are to be installed
        size_t ssidSize,
        ///< [IN]
        const uint8_t* wepKeyPtr,
        ///< [IN] WEP key used for this SSID
        size_t wepKeySize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Configure the given SSID to use PSK and the given pass-phrase or pre-shared key in the
 * respective input arguments. The protocol input is mandatory and has to be set to either
 * LE_WIFICLIENT_SECURITY_WPA_PSK_PERSONAL or LE_WIFICLIENT_SECURITY_WPA2_PSK_PERSONAL.
 * Besides, it's mandatory to have at least one of the pass-phrase and pre-shared key supplied. If
 * both are provided as input, the pass-phrase has precedence and will be used. But it fails to
 * authenticate, a second attempt using the provided pre-shared key will not be done.
 *
 * @return
 *      - LE_OK     Succeeded to configure the given pass-phrase or pre-shared key for the given
 *                  SSID.
 *      - LE_FAULT  Failed to configure.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_ConfigurePsk
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const uint8_t* ssidPtr,
        ///< [IN] SSID which configs are to be installed
        size_t ssidSize,
        ///< [IN]
        le_wifiClient_SecurityProtocol_t protocol,
        ///< [IN] security protocol WPA-PSK or WPA2-PSK
        const uint8_t* passPhrasePtr,
        ///< [IN] pass-phrase used for this SSID
        size_t passPhraseSize,
        ///< [IN]
        const uint8_t* pskPtr,
        ///< [IN] pre-shared key used for this SSID
        size_t pskSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Configure the given SSID to use EAP and the given EAP username and password in the respective
 * input arguments. The protocol input is mandatory and has to be set to either
 * LE_WIFICLIENT_SECURITY_WPA_EAP_PEAP0_ENTERPRISE or
 * LE_WIFICLIENT_SECURITY_WPA2_EAP_PEAP0_ENTERPRISE. Besides, both the username and password inputs
 * are mandatory.
 *
 * @return
 *      - LE_OK     Succeeded to configure the given EAP username and password for the given
 *                  SSID.
 *      - LE_FAULT  Failed to configure.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_ConfigureEap
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const uint8_t* ssidPtr,
        ///< [IN] SSID which configs are to be installed
        size_t ssidSize,
        ///< [IN]
        le_wifiClient_SecurityProtocol_t protocol,
        ///< [IN] security protocol WPA-EAP or WPA2-EAP
        const uint8_t* usernamePtr,
        ///< [IN] EAP username used for this SSID
        size_t usernameSize,
        ///< [IN]
        const uint8_t* passwordPtr,
        ///< [IN] EAP password used for this SSID
        size_t passwordSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove and clear Wifi's security configurations to use with the given SSID from the config tree
 * and secured store. This includes the security protocol and all the username, password,
 * passphrase, pre-shared key, key, etc., previously configured via le_wifiClient_Configure APIs for
 * WEP, PSK and EAP.
 *
 * @return
 *      - LE_OK upon success to deconfigure the given SSID's configured user credentials;
 *        LE_FAULT otherwise
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_wifiClient_RemoveSsidSecurityConfigs
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const uint8_t* ssidPtr,
        ///< [IN]  SSID which user credentials to be deconfigured
        size_t ssidSize
        ///< [IN]
);

#endif // LE_WIFICLIENT_COMMON_H_INCLUDE_GUARD