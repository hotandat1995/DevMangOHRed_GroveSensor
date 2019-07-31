/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "le_mdc_interface.h"
#include "le_mdc_messages.h"
#include "le_mdc_service.h"


//--------------------------------------------------------------------------------------------------
// Generic Client Types, Variables and Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Client Thread Objects
 *
 * This object is used to contain thread specific data for each IPC client.
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    le_msg_SessionRef_t sessionRef;     ///< Client Session Reference
    int                 clientCount;    ///< Number of clients sharing this thread
    le_mdc_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for client threads.
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(le_mdc_ClientThreadData,
                          LE_CDATA_COMPONENT_COUNT,
                          sizeof(_ClientThreadData_t));


//--------------------------------------------------------------------------------------------------
/**
 * The memory pool for client thread objects
 */
//--------------------------------------------------------------------------------------------------
static le_mem_PoolRef_t _ClientThreadDataPool;


//--------------------------------------------------------------------------------------------------
/**
 * Key under which the pointer to the Thread Object (_ClientThreadData_t) will be kept in
 * thread-local storage.  This allows a thread to quickly get a pointer to its own Thread Object.
 */
//--------------------------------------------------------------------------------------------------
static pthread_key_t _ThreadDataKey;


//--------------------------------------------------------------------------------------------------
/**
 * This global flag is shared by all client threads, and is used to indicate whether the common
 * data has been initialized.
 *
 * @warning Use InitMutex, defined below, to protect accesses to this data.
 */
//--------------------------------------------------------------------------------------------------
static bool CommonDataInitialized = false;


//--------------------------------------------------------------------------------------------------
/**
 * Mutex and associated macros for use with the above CommonDataInitialized.
 */
//--------------------------------------------------------------------------------------------------
extern le_mutex_Ref_t le_ifgen_InitMutexRef;

/// Locks the mutex.
#define LOCK_INIT    le_mutex_Lock(le_ifgen_InitMutexRef);

/// Unlocks the mutex.
#define UNLOCK_INIT  le_mutex_Unlock(le_ifgen_InitMutexRef);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize thread specific data, and connect to the service for the current thread.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is
 *    bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t InitClientForThread
(
    bool isBlocking
)
{
    // Open a session.
    le_msg_SessionRef_t sessionRef;
    le_msg_ProtocolRef_t protocolRef;

    protocolRef = le_msg_GetProtocolRef(PROTOCOL_ID_STR, sizeof(_Message_t));
    sessionRef = le_msg_CreateSession(protocolRef, SERVICE_INSTANCE_NAME);
    le_result_t result = ifgen_le_mdc_OpenSession(sessionRef, isBlocking);
    if (result != LE_OK)
    {
        LE_DEBUG("Could not connect to '%s' service", SERVICE_INSTANCE_NAME);

        return result;
    }

    // Store the client sessionRef in thread-local storage, since each thread requires
    // its own sessionRef.
    _ClientThreadData_t* clientThreadPtr = le_mem_ForceAlloc(_ClientThreadDataPool);
    memset(clientThreadPtr, 0, sizeof(_ClientThreadData_t));
    clientThreadPtr->sessionRef = sessionRef;
    if (pthread_setspecific(_ThreadDataKey, clientThreadPtr) != 0)
    {
        LE_FATAL("pthread_setspecific() failed!");
    }

    // This is the first client for the current thread
    clientThreadPtr->clientCount = 1;

    return LE_OK;
}


//--------------------------------------------------------------------------------------------------
/**
 * Get a pointer to the client thread data for the current thread.
 *
 * If the current thread does not have client data, then NULL is returned
 */
//--------------------------------------------------------------------------------------------------
static _ClientThreadData_t* GetClientThreadDataPtr
(
    void
)
{
    return pthread_getspecific(_ThreadDataKey);
}


//--------------------------------------------------------------------------------------------------
/**
 * Return the sessionRef for the current thread.
 *
 * If the current thread does not have a session ref, then this is a fatal error.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((unused)) static le_msg_SessionRef_t GetCurrentSessionRef
(
    void
)
{
    if (ifgen_le_mdc_HasLocalBinding())
    {
        return NULL;
    }
    else
    {
        _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

        // If the thread specific data is NULL, then the session ref has not been created.
        LE_FATAL_IF(clientThreadPtr==NULL,
                    "le_mdc_ConnectService() not called for current thread");

        return clientThreadPtr->sessionRef;
    }
}


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads.
 */
//--------------------------------------------------------------------------------------------------
static void InitCommonData(void)
{
    // Perform common initialization across all instances of this API.
    ifgen_le_mdc_InitCommonData();

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_InitStaticPool(le_mdc_ClientThreadData,
                                                  LE_CDATA_COMPONENT_COUNT,
                                                  sizeof(_ClientThreadData_t));

    // Create the thread-local data key to be used to store a pointer to each thread object.
    LE_ASSERT(pthread_key_create(&_ThreadDataKey, NULL) == 0);
}


//--------------------------------------------------------------------------------------------------
/**
 * Connect to the service, using either blocking or non-blocking calls.
 *
 * This function implements the details of the public ConnectService functions.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is
 *    bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
static le_result_t DoConnectService
(
    bool isBlocking
)
{
    // If this is the first time the function is called, init the client common data.
    LOCK_INIT
    if ( ! CommonDataInitialized )
    {
        InitCommonData();
        CommonDataInitialized = true;
    }
    UNLOCK_INIT

    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    // If the thread specific data is NULL, then there is no current client session.
    if (clientThreadPtr == NULL)
    {
        le_result_t result;

        result = InitClientForThread(isBlocking);
        if ( result != LE_OK )
        {
            // Note that the blocking call will always return LE_OK
            return result;
        }

        LE_DEBUG("======= Starting client for '%s' service ========", SERVICE_INSTANCE_NAME);
    }
    else
    {
        // Keep track of the number of clients for the current thread.  There is only one
        // connection per thread, and it is shared by all clients.
        clientThreadPtr->clientCount++;
        LE_DEBUG("======= Starting another client for '%s' service ========",
                 SERVICE_INSTANCE_NAME);
    }

    return LE_OK;
}


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
)
{
    if (!ifgen_le_mdc_HasLocalBinding())
    {
        // Connect to the service; block until connected.
        DoConnectService(true);
    }
}

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
)
{
    if (ifgen_le_mdc_HasLocalBinding())
    {
        return LE_OK;
    }
    else
    {
        // Connect to the service; return with an error if not connected.
        return DoConnectService(false);
    }
}

//--------------------------------------------------------------------------------------------------
// Session close handler.
//
// Dispatches session close notifications to the registered client handler function (if any)
//--------------------------------------------------------------------------------------------------
static void SessionCloseHandler
(
    le_msg_SessionRef_t sessionRef,
    void *contextPtr
)
{
    _ClientThreadData_t* clientThreadPtr = contextPtr;

    le_msg_DeleteSession( clientThreadPtr->sessionRef );

    // Need to delete the thread specific data, since it is no longer valid.  If a new
    // client session is started, new thread specific data will be allocated.
    le_mem_Release(clientThreadPtr);
    if (pthread_setspecific(_ThreadDataKey, NULL) != 0)
    {
        LE_FATAL("pthread_setspecific() failed!");
    }

    LE_DEBUG("======= '%s' service spontaneously disconnected ========", SERVICE_INSTANCE_NAME);

    if (clientThreadPtr->disconnectHandler)
    {
        clientThreadPtr->disconnectHandler(clientThreadPtr->contextPtr);
    }

    LE_FATAL("Component for le_mdc disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_SetServerDisconnectHandler
(
    le_mdc_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    if (ifgen_le_mdc_HasLocalBinding())
    {
        // Local bindings don't disconnect
        return;
    }

    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    if (NULL == clientThreadPtr)
    {
        LE_CRIT("Trying to set disconnect handler for non-existent client session for '%s' service",
                SERVICE_INSTANCE_NAME);
    }
    else
    {
        clientThreadPtr->disconnectHandler = disconnectHandler;
        clientThreadPtr->contextPtr = contextPtr;

        if (disconnectHandler)
        {
            le_msg_SetSessionCloseHandler(clientThreadPtr->sessionRef,
                                          SessionCloseHandler,
                                          clientThreadPtr);
        }
    }
}


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
)
{
    if (ifgen_le_mdc_HasLocalBinding())
    {
        // Cannot disconnect local bindings
        return;
    }

    _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

    // If the thread specific data is NULL, then there is no current client session.
    if (clientThreadPtr == NULL)
    {
        LE_CRIT("Trying to stop non-existent client session for '%s' service",
                SERVICE_INSTANCE_NAME);
    }
    else
    {
        // This is the last client for this thread, so close the session.
        if ( clientThreadPtr->clientCount == 1 )
        {
            le_msg_DeleteSession( clientThreadPtr->sessionRef );

            // Need to delete the thread specific data, since it is no longer valid.  If a new
            // client session is started, new thread specific data will be allocated.
            le_mem_Release(clientThreadPtr);
            if (pthread_setspecific(_ThreadDataKey, NULL) != 0)
            {
                LE_FATAL("pthread_setspecific() failed!");
            }

            LE_DEBUG("======= Stopping client for '%s' service ========", SERVICE_INSTANCE_NAME);
        }
        else
        {
            // There is one less client sharing this thread's connection.
            clientThreadPtr->clientCount--;

            LE_DEBUG("======= Stopping another client for '%s' service ========",
                     SERVICE_INSTANCE_NAME);
        }
    }
}

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
)
{
    return ifgen_le_mdc_AddSessionStateHandler(
        GetCurrentSessionRef(),
        profileRef,
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mdc_SessionState'
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_RemoveSessionStateHandler
(
    le_mdc_SessionStateHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_mdc_RemoveSessionStateHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

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
)
{
    return ifgen_le_mdc_AddMtPdpSessionStateHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mdc_MtPdpSessionState'
 */
//--------------------------------------------------------------------------------------------------
void le_mdc_RemoveMtPdpSessionStateHandler
(
    le_mdc_MtPdpSessionStateHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_mdc_RemoveMtPdpSessionStateHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

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
)
{
    return ifgen_le_mdc_GetProfile(
        GetCurrentSessionRef(),
        index
    );
}

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
)
{
    return ifgen_le_mdc_GetProfileIndex(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
    return ifgen_le_mdc_StartSession(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
     ifgen_le_mdc_StartSessionAsync(
        GetCurrentSessionRef(),
        profileRef,
        handlerPtr,
        contextPtr
    );
}

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
)
{
    return ifgen_le_mdc_StopSession(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
     ifgen_le_mdc_StopSessionAsync(
        GetCurrentSessionRef(),
        profileRef,
        handlerPtr,
        contextPtr
    );
}

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
)
{
    return ifgen_le_mdc_RejectMtPdpSession(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
    return ifgen_le_mdc_GetSessionState(
        GetCurrentSessionRef(),
        profileRef,
        connectionStatePtr
    );
}

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
)
{
    return ifgen_le_mdc_GetInterfaceName(
        GetCurrentSessionRef(),
        profileRef,
        interfaceName,
        interfaceNameSize
    );
}

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
)
{
    return ifgen_le_mdc_GetIPv4Address(
        GetCurrentSessionRef(),
        profileRef,
        ipAddr,
        ipAddrSize
    );
}

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
)
{
    return ifgen_le_mdc_GetIPv4GatewayAddress(
        GetCurrentSessionRef(),
        profileRef,
        gatewayAddr,
        gatewayAddrSize
    );
}

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
)
{
    return ifgen_le_mdc_GetIPv4DNSAddresses(
        GetCurrentSessionRef(),
        profileRef,
        dns1AddrStr,
        dns1AddrStrSize,
        dns2AddrStr,
        dns2AddrStrSize
    );
}

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
)
{
    return ifgen_le_mdc_GetIPv6Address(
        GetCurrentSessionRef(),
        profileRef,
        ipAddr,
        ipAddrSize
    );
}

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
)
{
    return ifgen_le_mdc_GetIPv6GatewayAddress(
        GetCurrentSessionRef(),
        profileRef,
        gatewayAddr,
        gatewayAddrSize
    );
}

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
)
{
    return ifgen_le_mdc_GetIPv6DNSAddresses(
        GetCurrentSessionRef(),
        profileRef,
        dns1AddrStr,
        dns1AddrStrSize,
        dns2AddrStr,
        dns2AddrStrSize
    );
}

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
)
{
    return ifgen_le_mdc_IsIPv4(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
    return ifgen_le_mdc_IsIPv6(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
    return ifgen_le_mdc_GetDataBearerTechnology(
        GetCurrentSessionRef(),
        profileRef,
        downlinkDataBearerTechPtrPtr,
        uplinkDataBearerTechPtrPtr
    );
}

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
)
{
    return ifgen_le_mdc_GetBytesCounters(
        GetCurrentSessionRef(),
        rxBytesPtr,
        txBytesPtr
    );
}

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
)
{
    return ifgen_le_mdc_ResetBytesCounter(
        GetCurrentSessionRef()
    );
}

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
)
{
    return ifgen_le_mdc_StopBytesCounter(
        GetCurrentSessionRef()
    );
}

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
)
{
    return ifgen_le_mdc_StartBytesCounter(
        GetCurrentSessionRef()
    );
}

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
)
{
    return ifgen_le_mdc_SetPDP(
        GetCurrentSessionRef(),
        profileRef,
        pdp
    );
}

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
)
{
    return ifgen_le_mdc_GetPDP(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
    return ifgen_le_mdc_SetAPN(
        GetCurrentSessionRef(),
        profileRef,
        apnStr
    );
}

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
)
{
    return ifgen_le_mdc_SetDefaultAPN(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
    return ifgen_le_mdc_GetAPN(
        GetCurrentSessionRef(),
        profileRef,
        apnStr,
        apnStrSize
    );
}

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
)
{
    return ifgen_le_mdc_SetAuthentication(
        GetCurrentSessionRef(),
        profileRef,
        type,
        userName,
        password
    );
}

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
)
{
    return ifgen_le_mdc_GetAuthentication(
        GetCurrentSessionRef(),
        profileRef,
        typePtr,
        userName,
        userNameSize,
        password,
        passwordSize
    );
}

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
)
{
    return ifgen_le_mdc_NumProfiles(
        GetCurrentSessionRef()
    );
}

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
)
{
    return ifgen_le_mdc_GetProfileFromApn(
        GetCurrentSessionRef(),
        apnStr,
        profileRefPtr
    );
}

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
)
{
    return ifgen_le_mdc_GetDisconnectionReason(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
    return ifgen_le_mdc_GetPlatformSpecificDisconnectionCode(
        GetCurrentSessionRef(),
        profileRef
    );
}

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
)
{
     ifgen_le_mdc_GetPlatformSpecificFailureConnectionReason(
        GetCurrentSessionRef(),
        profileRef,
        failureTypePtr,
        failureCodePtr
    );
}

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
)
{
    return ifgen_le_mdc_GetDisconnectionReasonExt(
        GetCurrentSessionRef(),
        profileRef,
        pdpType
    );
}

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
)
{
    return ifgen_le_mdc_GetPlatformSpecificDisconnectionCodeExt(
        GetCurrentSessionRef(),
        profileRef,
        pdpType
    );
}

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
)
{
     ifgen_le_mdc_GetPlatformSpecificFailureConnectionReasonExt(
        GetCurrentSessionRef(),
        profileRef,
        pdpType,
        failureTypePtr,
        failureCodePtr
    );
}

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
)
{
    return ifgen_le_mdc_MapProfileOnNetworkInterface(
        GetCurrentSessionRef(),
        profileRef,
        interfaceName
    );
}

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
)
{
    return ifgen_le_mdc_GetProfileList(
        GetCurrentSessionRef(),
        profileListPtr,
        profileListSizePtr
    );
}