

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_mdc Modem Data Control
 *
 * @ref le_mdc_interface.h "API Reference"
 *
 * <HR>
 *
 * A data session is useful for  applications that need to send or receive data over a network
 * where SMS messages are insufficient.  To start a data session, a data profile must
 * be configured as specified by the target network.
 *
 * The Modem Data Control (mdc) API is used to manage data profiles and data sessions.
 *
 * @section le_mdc_binding IPC interfaces binding
 *
 * All the functions of this API are provided by the @b modemService service.
 *
 * Here's a code sample binding to modem services:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_mdc -> modemService.le_mdc
   }
   @endverbatim
 *
 * @section le_mdc_profile Data Profiles
 *
 * If a pre-defined data profile has been configured then this profile can
 * be loaded using le_mdc_GetProfile().
 * le_mdc_GetProfile() must be called with @c LE_MDC_DEFAULT_PROFILE to retrieve the default index
 * used by the modem for data connection.
 * le_mdc_GetProfile() must be called with @c LE_MDC_SIMTOOLKIT_BIP_DEFAULT_PROFILE to retrieve the
 * default index used by the modem for Bearer Independent Protocol (BIP).
 * The maximum number of data profiles supported is modem dependent and can be retrieved with
 * le_mdc_NumProfiles().
 *
 * @note le_mdc_GetProfile() creates a new profile if the profile's index can't be found.
 *
 * @warning 0 is not a valid index.
 *
 * A pre-defined data profile can be retrieved using le_mdc_GetProfileFromApn() thanks to its
 * APN.
 *
 * A default APN can be set for a defined profile with le_mdc_SetDefaultAPN(), based on the SIM
 * identification number (ICCID). If no match is found in the database using the ICCID, the search
 * falls back on the home network (MCC/MNC) to determine the default APN.
 *
 * @warning Ensure to check the list of supported data profiles for your specific platform.
 *
 * The following data profile parameters can be retrieved:
 *  - Packet Data Protocol using le_mdc_GetPDP().
 *  - Access Point Name using le_mdc_GetAPN().
 *  - Authentication settings using le_mdc_GetAuthentication().
 *
 * The following data profile parameters can be set:
 *  - Packet Data Protocol using le_mdc_SetPDP().
 *  - Access Point Name using le_mdc_SetAPN().
 *  - Authentication settings using le_mdc_SetAuthentication().
 *
 * @warning The maximum APN length might be limited by the platform.
 *          Please refer to the platform documentation.
 *
 *  A sample code can be seen in the following page:
 * - @subpage c_mdcDataProfiles
 *
 * @section le_mdc_session Data Sessions
 *
 * @subsection le_mdc_session_MO Mobile Originated (MO-PDP context activation)
 *
 * le_mdc_MapProfileOnNetworkInterface() may be used to map a data session with a network interface.
 * To take effect, this API has to be called before starting the data session. Otherwise, the
 * mapping will be taken into account at the next start of the data profile.
 *
 * A data session can be started using le_mdc_StartSession(). To start a data session, a
 * data profile must be created and written to the modem, or  an existing data profile
 * can be used. A data session can be stopped using le_mdc_StopSession(). The number of
 * simultaneous data sessions supported is dependent on the modem, but cannot be more than the
 * maximum number of supported profiles.
 *
 * A data session can be started using le_mdc_StartSessionAsync() and stopped using
 * le_mdc_StopSessionAsync(). These functions are not blocking. The response will be returned
 * with the @c le_mdc_SessionHandlerFunc_t handler function.
 *
 * The current state of a data session can be queried using le_mdc_GetSessionState(). An application
 * can also a register handler to be notified when the session state changes. The handler
 * can be managed using le_mdc_AddSessionStateHandler() and le_mdc_RemoveSessionStateHandler().
 *
 * @subsection le_mdc_session_MT Mobile Terminated (MT-PDP context activation)
 * To be notified by the MT-PDP context request, the application has to subscribe to the state
 * handler using le_mdc_AddMtPdpSessionStateHandler().
 * When the incoming MT-PDP context request is notified,
 * the application takes responsibility to trigger the data session for that MT PDP request using
 * le_mdc_StartSession(). That data session can be stopped using le_mdc_StopSession().
 *
 * The network interface settings of that MT-PDP context can be retrieved using the provided
 * Data Profile.
 * Please refer to @ref le_mdc_session_networkItf.
 *
 * A data session can be rejected using le_mdc_RejectMtPdpSession().
 * The number of simultaneous data sessions supported is dependent on the modem,
 * but cannot be more than the maximum number of supported profiles.
 *
 * The handler can be released using le_mdc_RemoveMtPdpSessionStateHandler().
 *
 * @warning The MT-PDP context activation feature is not supported on all platforms. Please refer to
 * @ref MT-PDP_context section for full details.
 *
 * @subsection le_mdc_session_networkItf Network interface settings
 * Once a data session starts, a Linux network interface is created.  It's the application's
 * responsibility to configure the network interface, usually through a DHCP client. Query the
 * interface name using le_mdc_GetInterfaceName(). The IP Preference can be checked with
 * le_mdc_IsIPv4() or le_mdc_IsIPv6() when the profile is connected. The IP address for the
 * current data session
 * can be retrieved by  le_mdc_GetIPv4Address() or le_mdc_GetIPv6Address(). The Gateway and
 * DNS
 * addresses can be retrieved using le_mdc_GetIPv4GatewayAddress(),
 * le_mdc_GetIPv4DNSAddresses() or le_mdc_GetIPv6GatewayAddress(), le_mdc_GetIPv6DNSAddresses().
 * The Access Point Name can be retrieved by le_mdc_GetAPN(). The Data bearer
 * Technology can be retrieved by le_mdc_GetDataBearerTechnology().
 *
 * le_mdc_GetDisconnectionReason() or le_mdc_GetPlatformSpecificDisconnectionCode() let you get
 * the reason for disconnection of data session by retrieving the call end failure code
 * from @c le_mdc_ProfileRef_t.
 *
 * le_mdc_GetPlatformSpecificFailureConnectionReason() let you get the data session connection
 * failure reason by retrieving the  call connection failure code and type from
 * @c le_mdc_ProfileRef_t.
 *
 * Please refer to @ref platformConstraintsSpecificErrorCodes for platform specific
 * disconnection code description.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_mdcDataSessions
 *
 * The following extended functions should be used to get the disconnection reason instead.
 * A pdpType parameter is required to specify which session's(IPv4 or IPv6 session) disconnect
 * reason to get for IPv4v6 mode. For IPv4 and IPv6 mode, the pdpType is ignored because there
 * is only one session.
 * - le_mdc_GetDisconnectionReasonExt()
 * - le_mdc_GetPlatformSpecificDisconnectionCodeExt()
 * - le_mdc_GetPlatformSpecificFailureConnectionReasonExt()
 *
 * @section le_mdc_dataStatistics Data Statistics
 *
 * The amount of received and transmitted data can be retrieved through le_mdc_GetBytesCounters().
 * The returned values correspond to the number of received and transmitted
 * bytes since the last call to le_mdc_ResetBytesCounter().
 *
 * The data statistics collection can be enabled with le_mdc_StartBytesCounter() and disabled
 * without resetting the counters with le_mdc_StopBytesCounter().
 *
 * @note The data statistics collection activation and the data counters are persistent even after
 * a reboot of the platform.
 *
 * A sample code can be seen in the following page:
 * - @subpage c_mdcDataStatistics
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @page c_mdcDataProfiles Sample code for Data Profiles
 *
 * @snippet "apps/test/modemServices/mdc/mdcIntegrationTest/mdcTestComp/mdcTest.c" Profiles
 */
/**
 * @page c_mdcDataSessions Sample code for network interface settings
 *
 * @snippet "apps/test/modemServices/mdc/mdcIntegrationTest/mdcTestComp/mdcTest.c" Sessions
 */
/**
 * @page c_mdcDataStatistics Sample code for Data Statistics
 *
 * @snippet "apps/test/modemServices/mdc/mdcIntegrationTest/mdcTestComp/mdcTest.c" Statistics
 */
/**
 * @file le_mdc_interface.h
 *
 * Legato @ref c_mdc include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_MDC_INTERFACE_H_INCLUDE_GUARD
#define LE_MDC_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_mdc_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_mdc_DisconnectHandler_t)(void *);

//--------------------------------------------------------------------------------------------------
/**
 *
 * Connect the current client thread to the service providing this API. Block until the service is
 * available.
 *
 * For each thread that wants to use this API, either ConnectService or TryConnectService must be
 * called before any other functions in this API.  Normally, ConnectService is automatically called
 * for the main thread, but not for any other thread. For details, see @ref apiFilesC_client.
 *
 * This function is created automatically.
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_ConnectService
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 *
 * Try to connect the current client thread to the service providing this API. Return with an error
 * if the service is not available.
 *
 * For each thread that wants to use this API, either ConnectService or TryConnectService must be
 * called before any other functions in this API.  Normally, ConnectService is automatically called
 * for the main thread, but not for any other thread. For details, see @ref apiFilesC_client.
 *
 * This function is created automatically.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is
 *    bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_TryConnectService
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
LE_FULL_API void le_mdc_SetServerDisconnectHandler
(
    le_mdc_DisconnectHandler_t disconnectHandler,
    void *contextPtr
);

//--------------------------------------------------------------------------------------------------
/**
 *
 * Disconnect the current client thread from the service providing this API.
 *
 * Normally, this function doesn't need to be called. After this function is called, there's no
 * longer a connection to the service, and the functions in this API can't be used. For details, see
 * @ref apiFilesC_client.
 *
 * This function is created automatically.
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference to a modem data connection profile.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Enumeration of data bearer technologies.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Enumeration of Packet Data Protocol.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Authentication bit mask.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Enumeration of connection state.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 *  Enumeration of the possible reasons for the disconnection.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for Data session connection state changes.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for MT-PDP Data session connection state changes.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 *  Handler for asynchronous session start and stop result response
 *
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_mdc_SessionState'
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_mdc_MtPdpSessionState'
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * This is the structure with info about a given cellular data profile
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mdc_SessionState'
 *
 * This event provides information on data session connection state changes for the given profileRef.
 *
 */
//--------------------------------------------------------------------------------------------------
le_mdc_SessionStateHandlerRef_t le_mdc_AddSessionStateHandler
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] The profile object of interest
    le_mdc_SessionStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mdc_SessionState'
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_RemoveSessionStateHandler
(
    le_mdc_SessionStateHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mdc_MtPdpSessionState'
 *
 * This event provides information on data session connection state changes for the given profileRef.
 *
 */
//--------------------------------------------------------------------------------------------------
le_mdc_MtPdpSessionStateHandlerRef_t le_mdc_AddMtPdpSessionStateHandler
(
    le_mdc_SessionStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mdc_MtPdpSessionState'
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_RemoveMtPdpSessionStateHandler
(
    le_mdc_MtPdpSessionStateHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get Profile Reference for index
 *
 * @note Create a new profile if the profile's index can't be found.
 *
 * @warning 0 is not a valid index.
 *
 * @warning Ensure to check the list of supported data profiles for your specific platform.
 *
 * @return
 *      - Reference to the data profile
 *      - NULL if the profile index does not exist
 */
//--------------------------------------------------------------------------------------------------
le_mdc_ProfileRef_t le_mdc_GetProfile
(
    uint32_t index
        ///< [IN] index of the profile.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the index for the given Profile.
 *
 * @return
 *      - index of the profile in the modem
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_mdc_GetProfileIndex
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Start profile data session.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if input parameter is incorrect
 *      - LE_DUPLICATE if the data session is already connected for the given profile
 *      - LE_TIMEOUT for session start timeout
 *      - LE_FAULT for other failures
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_StartSession
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Start data session for this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Start profile data session.
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_StartSessionAsync
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Start data session for this profile object
    le_mdc_SessionHandlerFunc_t handlerPtr,
        ///< [IN] Handler for start data session result
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop profile data session.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if the input parameter is not valid
 *      - LE_FAULT for other failures
 *
 * @note
 *      The process exits, if an invalid profile object is given
 *
 * @warning The MT-PDP context activation feature is not supported on all platforms. Please refer to
 * @ref MT-PDP_context section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_StopSession
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Stop data session for this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop profile data session.
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_StopSessionAsync
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Stop data session for this profile object
    le_mdc_SessionHandlerFunc_t handlerPtr,
        ///< [IN] Handler for stop data session result
    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Reject MT-PDP profile data session.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if the input parameter is not valid
 *      - LE_UNSUPPORTED if not supported by the target
 *      - LE_FAULT for other failures
 *
 * @note
 *      The process exits, if an invalid profile object is given
 *
 * @warning The MT-PDP context activation feature is not supported on all platforms. Please refer to
 * @ref MT-PDP_context section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_RejectMtPdpSession
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Reject MT-PDP data session for this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the current data session state.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_FAULT on failure
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetSessionState
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_ConState_t* connectionStatePtr
        ///< [OUT] The data session state
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the network interface name, if the data session is connected.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the interface name would not fit in interfaceNameStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetInterfaceName
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* interfaceName,
        ///< [OUT] The name of the network interface
    size_t interfaceNameSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the IPv4 address for the given profile, if the data session is connected and has an IPv4
 * address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in ipAddrStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv4Address
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* ipAddr,
        ///< [OUT] The IP address in dotted format
    size_t ipAddrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the gateway IPv4 address for the given profile, if the data session is connected and has an
 * IPv4 address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in gatewayAddrStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv4GatewayAddress
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* gatewayAddr,
        ///< [OUT] The gateway IP address in dotted format
    size_t gatewayAddrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the primary/secondary DNS v4 addresses for the given profile, if the data session is
 * connected and has an IPv4 address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in buffer
 *      - LE_FAULT for all other errors
 *
 * @note
 *      - If only one DNS address is available, then it will be returned, and an empty string will
 *        be returned for the unavailable address
 *      - The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv4DNSAddresses
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* dns1AddrStr,
        ///< [OUT] The primary DNS IP address in dotted format
    size_t dns1AddrStrSize,
        ///< [IN]
    char* dns2AddrStr,
        ///< [OUT] The secondary DNS IP address in dotted format
    size_t dns2AddrStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the IPv6 address for the given profile, if the data session is connected and has an IPv6
 * address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in ipAddrStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv6Address
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* ipAddr,
        ///< [OUT] The IP address in dotted format
    size_t ipAddrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the gateway IPv6 address for the given profile, if the data session is connected and has an
 * IPv6 address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address would not fit in gatewayAddrStr
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv6GatewayAddress
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* gatewayAddr,
        ///< [OUT] The gateway IP address in dotted format
    size_t gatewayAddrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the primary/secondary DNS v6 addresses, if the data session is connected and has an IPv6
 * address.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the IP address can't fit in buffer
 *      - LE_FAULT for all other errors
 *
 * @note
 *      - If only one DNS address is available, it will be returned, and an empty string will
 *        be returned for the unavailable address.
 *      - The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetIPv6DNSAddresses
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* dns1AddrStr,
        ///< [OUT] The primary DNS IP address in dotted format
    size_t dns1AddrStrSize,
        ///< [IN]
    char* dns2AddrStr,
        ///< [OUT] The secondary DNS IP address in dotted format
    size_t dns2AddrStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Allow the caller to know if the given profile is actually supporting IPv4, if the data session
 * is connected.
 *
 * @return TRUE if PDP type is IPv4, FALSE otherwise.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_mdc_IsIPv4
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Allow the caller to know if the given profile is actually supporting IPv6, if the data session
 * is connected.
 *
 * @return TRUE if PDP type is IPv6, FALSE otherwise.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_mdc_IsIPv6
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Data Bearer Technology for the given profile, if the data session is connected.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetDataBearerTechnology
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_DataBearerTechnology_t* downlinkDataBearerTechPtrPtr,
        ///< [OUT] downlink data bearer technology
    le_mdc_DataBearerTechnology_t* uplinkDataBearerTechPtrPtr
        ///< [OUT] uplink data bearer technology
);

//--------------------------------------------------------------------------------------------------
/**
 * Get number of bytes received/transmitted without error since the last reset.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 *
 * @note
 *      - The process exits, if an invalid pointer is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetBytesCounters
(
    uint64_t* rxBytesPtr,
        ///< [OUT] bytes amount received since the last counter reset
    uint64_t* txBytesPtr
        ///< [OUT] bytes amount transmitted since the last counter reset
);

//--------------------------------------------------------------------------------------------------
/**
 * Reset received/transmitted data flow statistics
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_ResetBytesCounter
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop collecting received/transmitted data flow statistics
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_StopBytesCounter
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Start collecting received/transmitted data flow statistics
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for all other errors
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_StartBytesCounter
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Packet Data Protocol (PDP) for the given profile.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if the PDP is not supported
 *      - LE_FAULT if the data session is currently connected for the given profile
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_SetPDP
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_Pdp_t pdp
        ///< [IN] The Packet Data Protocol
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Packet Data Protocol (PDP) for the given profile.
 *
 * @return
 *      - packet data protocol value
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_mdc_Pdp_t le_mdc_GetPDP
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Access Point Name (APN) for the given profile.
 *
 * The APN must be an ASCII string.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_FAULT if the data session is currently connected for the given profile
 *
 * @note If APN is too long (max APN_NAME_MAX_LEN digits), it is a fatal error, the
 *       function will not return.
 *
 * @warning The maximum APN length might be limited by the platform.
 *          Please refer to the platform documentation @ref platformConstraintsMdc.
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_SetAPN
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    const char* LE_NONNULL apnStr
        ///< [IN] The Access Point Name
);

//--------------------------------------------------------------------------------------------------
/**
 * Set the Access Point Name (APN) for the given profile according to the SIM identification
 * number (ICCID). If no APN is found using the ICCID, fall back on the home network (MCC/MNC)
 * to determine the default APN.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_FAULT for all other errors
 *
 * @note
 *      The process exits if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_SetDefaultAPN
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] Query this profile object
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Access Point Name (APN) for the given profile.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_OVERFLOW if the APN is is too long
 *      - LE_FAULT on failed
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetAPN
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    char* apnStr,
        ///< [OUT] The Access Point Name
    size_t apnStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Set authentication property
 *
 * @return
 *      - LE_OK on success
 *
 * @note
 *      - The process exits, if userName or password are NULL when type is not PA_MDC_AUTH_NONE
 *      - The process exits, if an invalid profile object is given
 * @note If userName is too long (max USER_NAME_MAX_LEN digits), it is a fatal error, the
 *       function will not return.
 * @note If password is too long (max PASSWORD_NAME_MAX_LEN digits), it is a fatal error, the
 *       function will not return.
 * @note Both PAP and CHAP authentication can be set for 3GPP network: in this case, the device
 *       decides which authentication procedure is performed. For example, the device can have a
 *       policy to select the most secure authentication mechanism.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_SetAuthentication
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_Auth_t type,
        ///< [IN] Authentication type
    const char* LE_NONNULL userName,
        ///< [IN] UserName used by authentication
    const char* LE_NONNULL password
        ///< [IN] Password used by authentication
);

//--------------------------------------------------------------------------------------------------
/**
 * Get authentication property
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_OVERFLOW userName or password are too small
 *      - LE_FAULT on failed
 *
 * @note
 *      The process exits, if an invalid profile object is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetAuthentication
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Query this profile object
    le_mdc_Auth_t* typePtr,
        ///< [OUT] Authentication type
    char* userName,
        ///< [OUT] UserName used by authentication
    size_t userNameSize,
        ///< [IN]
    char* password,
        ///< [OUT] Password used by authentication
    size_t passwordSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of profiles on the modem.
 *
 * @return
 *      - number of profiles existing on modem
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_mdc_NumProfiles
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get a profile selected by its APN
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if an input parameter is not valid
 *      - LE_NOT_FOUND if the requested APN is not found
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetProfileFromApn
(
    const char* LE_NONNULL apnStr,
        ///< [IN] The Access Point Name
    le_mdc_ProfileRef_t* profileRefPtr
        ///< [OUT] profile reference
);

//--------------------------------------------------------------------------------------------------
/**
 * Called to get the disconnection reason.
 *
 * @return The disconnection reason.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 *
 * @deprecated This function is deprecated, le_mdc_GetDisconnectionReasonExt should be used for the
 *             new code.
 */
//--------------------------------------------------------------------------------------------------
le_mdc_DisconnectionReason_t le_mdc_GetDisconnectionReason
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] profile reference
);

//--------------------------------------------------------------------------------------------------
/**
 * Called to get the platform specific disconnection code.
 *
 * Refer to @ref platformConstraintsSpecificErrorCodes for platform specific
 * disconnection code description.
 *
 * @return The platform specific disconnection code.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 *
 * @deprecated This function is deprecated, le_mdc_GetPlatformSpecificDisconnectionCodeExt should
 *             be used for the new code.
 */
//--------------------------------------------------------------------------------------------------
int32_t le_mdc_GetPlatformSpecificDisconnectionCode
(
    le_mdc_ProfileRef_t profileRef
        ///< [IN] profile reference
);

//--------------------------------------------------------------------------------------------------
/**
 * Called to get the platform specific connection failure reason.
 *
 * Refer to @ref platformConstraintsSpecificErrorCodes for platform specific
 * connection failure types and code descriptions.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 *
 * @deprecated This function is deprecated, le_mdc_GetPlatformSpecificFailureConnectionReasonExt
 *             should be used for new code.
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_GetPlatformSpecificFailureConnectionReason
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] profile reference
    int32_t* failureTypePtr,
        ///< [OUT] platform specific failure type
    int32_t* failureCodePtr
        ///< [OUT] platform specific failure code
);

//--------------------------------------------------------------------------------------------------
/**
 * Called to get the disconnection reason.
 *
 * @return The disconnection reason.
 *
 * @warning The return value le_mdc_DisconnectionReason_t might be limited by the platform.
 *          Please refer to the platform documentation @ref platformConstraintsMdc.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 * @note For IPv4v6 mode, pdpType specifies which session's disconnect reason to get(IPv4 or IPv6
 *       session). For IPv4 and IPv6 mode, pdpType is ignored because there is only one session for
 *       IPv4 and IPv6 mode.
 */
//--------------------------------------------------------------------------------------------------
le_mdc_DisconnectionReason_t le_mdc_GetDisconnectionReasonExt
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] profile reference
    le_mdc_Pdp_t pdpType
        ///< [IN] pdp type of session
);

//--------------------------------------------------------------------------------------------------
/**
 * Called to get the platform specific disconnection code.
 *
 * Refer to @ref platformConstraintsSpecificErrorCodes for platform specific
 * disconnection code description.
 *
 * @return The platform specific disconnection code.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 * @note For IPv4v6 mode, pdpType specifies which session's disconnect reason to get(IPv4 or IPv6
 *       session). For IPv4 and IPv6 mode, pdpType is ignored because there is only one session for
 *       IPv4 and IPv6 mode.
 */
//--------------------------------------------------------------------------------------------------
int32_t le_mdc_GetPlatformSpecificDisconnectionCodeExt
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] profile reference
    le_mdc_Pdp_t pdpType
        ///< [IN] pdp type of session
);

//--------------------------------------------------------------------------------------------------
/**
 * Called to get the platform specific connection failure reason.
 *
 * Refer to @ref platformConstraintsSpecificErrorCodes for platform specific connection failure
 * types and code descriptions.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 * @note For IPv4v6 mode, pdpType specifies which session's disconnect reason to get(IPv4 or IPv6
 *       session). For IPv4 and IPv6 mode, pdpType is ignored because there is only one session for
 *       IPv4 and IPv6 mode.
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_GetPlatformSpecificFailureConnectionReasonExt
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] profile reference
    le_mdc_Pdp_t pdpType,
        ///< [IN] pdp type of session
    int32_t* failureTypePtr,
        ///< [OUT] platform specific failure type
    int32_t* failureCodePtr
        ///< [OUT] platform specific failure code
);

//--------------------------------------------------------------------------------------------------
/**
 * Map a profile on a network interface
 *
 * * @return
 *      - LE_OK on success
 *      - LE_UNSUPPORTED if not supported by the target
 *      - LE_FAULT for all other errors
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_MapProfileOnNetworkInterface
(
    le_mdc_ProfileRef_t profileRef,
        ///< [IN] Profile reference
    const char* LE_NONNULL interfaceName
        ///< [IN] Network interface name
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the list of available profiles
 *
 * @return
 *      - list of available profiles
 *      - LE_OK if the retrieval is successful; otherwise, some error code
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mdc_GetProfileList
(
    le_mdc_ProfileInfo_t* profileListPtr,
        ///< [OUT] list of available profiles
    size_t* profileListSizePtr
        ///< [INOUT]
);

#endif // LE_MDC_INTERFACE_H_INCLUDE_GUARD