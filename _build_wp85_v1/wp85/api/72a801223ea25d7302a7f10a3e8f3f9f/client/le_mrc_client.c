/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "le_mrc_interface.h"
#include "le_mrc_messages.h"
#include "le_mrc_service.h"


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
    le_mrc_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for client threads.
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(le_mrc_ClientThreadData,
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
    le_result_t result = ifgen_le_mrc_OpenSession(sessionRef, isBlocking);
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
    if (ifgen_le_mrc_HasLocalBinding())
    {
        return NULL;
    }
    else
    {
        _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

        // If the thread specific data is NULL, then the session ref has not been created.
        LE_FATAL_IF(clientThreadPtr==NULL,
                    "le_mrc_ConnectService() not called for current thread");

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
    ifgen_le_mrc_InitCommonData();

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_InitStaticPool(le_mrc_ClientThreadData,
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
void le_mrc_ConnectService
(
    void
)
{
    if (!ifgen_le_mrc_HasLocalBinding())
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
le_result_t le_mrc_TryConnectService
(
    void
)
{
    if (ifgen_le_mrc_HasLocalBinding())
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

    LE_FATAL("Component for le_mrc disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_SetServerDisconnectHandler
(
    le_mrc_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    if (ifgen_le_mrc_HasLocalBinding())
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
void le_mrc_DisconnectService
(
    void
)
{
    if (ifgen_le_mrc_HasLocalBinding())
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
 * Add handler function for EVENT 'le_mrc_NetRegStateEvent'
 *
 * This event provides information on network registration state changes.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_NetRegStateEventHandlerRef_t le_mrc_AddNetRegStateEventHandler
(
    le_mrc_NetRegStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_mrc_AddNetRegStateEventHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mrc_NetRegStateEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_RemoveNetRegStateEventHandler
(
    le_mrc_NetRegStateEventHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_mrc_RemoveNetRegStateEventHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mrc_RatChange'
 *
 * This event provides information on Radio Access Technology changes.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_RatChangeHandlerRef_t le_mrc_AddRatChangeHandler
(
    le_mrc_RatChangeHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_mrc_AddRatChangeHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mrc_RatChange'
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_RemoveRatChangeHandler
(
    le_mrc_RatChangeHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_mrc_RemoveRatChangeHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mrc_PacketSwitchedChange'
 *
 * This event provides information on Packet Switched service changes.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_PacketSwitchedChangeHandlerRef_t le_mrc_AddPacketSwitchedChangeHandler
(
    le_mrc_PacketSwitchedChangeHandlerFunc_t packetHandlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_mrc_AddPacketSwitchedChangeHandler(
        GetCurrentSessionRef(),
        packetHandlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mrc_PacketSwitchedChange'
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_RemovePacketSwitchedChangeHandler
(
    le_mrc_PacketSwitchedChangeHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_mrc_RemovePacketSwitchedChangeHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mrc_SignalStrengthChange'
 *
 * This event provides information on Signal Strength value changes.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_SignalStrengthChangeHandlerRef_t le_mrc_AddSignalStrengthChangeHandler
(
    le_mrc_Rat_t rat,
        ///< [IN] Radio Access Technology
    int32_t lowerRangeThreshold,
        ///< [IN] Lower-range Signal strength threshold in dBm
    int32_t upperRangeThreshold,
        ///< [IN] Upper-range Signal strength threshold in dBm
    le_mrc_SignalStrengthChangeHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_mrc_AddSignalStrengthChangeHandler(
        GetCurrentSessionRef(),
        rat,
        lowerRangeThreshold,
        upperRangeThreshold,
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mrc_SignalStrengthChange'
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_RemoveSignalStrengthChangeHandler
(
    le_mrc_SignalStrengthChangeHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_mrc_RemoveSignalStrengthChangeHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function sets signal strength indication thresholds for a specific RAT.
 *
 * @return
 *  - LE_OK             Function succeeded.
 *  - LE_BAD_PARAMETER  Bad parameters
 *  - LE_FAULT          Function failed.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetSignalStrengthIndThresholds
(
    le_mrc_Rat_t rat,
        ///< [IN] Radio Access Technology
    int32_t lowerRangeThreshold,
        ///< [IN] Lower-range Signal strength threshold in dBm
    int32_t upperRangeThreshold
        ///< [IN] Upper-range Signal strength threshold in dBm
)
{
    return ifgen_le_mrc_SetSignalStrengthIndThresholds(
        GetCurrentSessionRef(),
        rat,
        lowerRangeThreshold,
        upperRangeThreshold
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function sets a signal strength indication delta for a specific RAT.
 *
 * @return
 *  - LE_OK             Function succeeded.
 *  - LE_BAD_PARAMETER  Bad parameters.
 *  - LE_FAULT          Function failed.
 *
 * @note The signal delta is set in units of 0.1 dBm. For example, to set a delta of 10.6 dBm, the
 *       delta value must be set to 106.
 *
 * @warning The signal delta resolution is platform dependent. Please refer to
 *          @ref platformConstraintsMdc section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetSignalStrengthIndDelta
(
    le_mrc_Rat_t rat,
        ///< [IN] Radio Access Technology
    uint16_t delta
        ///< [IN] Signal delta in units of 0.1 dBm
)
{
    return ifgen_le_mrc_SetSignalStrengthIndDelta(
        GetCurrentSessionRef(),
        rat,
        delta
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Enable the automatic Selection Register mode.
 *
 * @return
 *  - LE_FAULT  Function failed.
 *  - LE_OK     Function succeeded.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetAutomaticRegisterMode
(
    void
)
{
    return ifgen_le_mrc_SetAutomaticRegisterMode(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the manual Selection Register mode with the MCC/MNC parameters.
 *
 * @return
 *  - LE_FAULT  Function failed.
 *  - LE_OK     Function succeeded.
 *
 * @note If strings are not set, too long (bigger than LE_MRC_MCC_LEN/LE_MRC_MNC_LEN digits), or too
 *       short (less than LE_MRC_MCC_LEN/LE_MRC_MNC_LEN-1 digits) it's a fatal error, the function
 *       won't return.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetManualRegisterMode
(
    const char* LE_NONNULL mcc,
        ///< [IN] Mobile Country Code
    const char* LE_NONNULL mnc
        ///< [IN] Mobile Network Code
)
{
    return ifgen_le_mrc_SetManualRegisterMode(
        GetCurrentSessionRef(),
        mcc,
        mnc
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the manual selection register mode asynchronously. This function is not blocking,
 *  the response will be returned with a handler function.
 *
 * @note If strings are not set, too long (bigger than LE_MRC_MCC_LEN/LE_MRC_MNC_LEN digits), or too
 *       short (less than LE_MRC_MCC_LEN/LE_MRC_MNC_LEN-1 digits) it's a fatal error, the function
 *       won't return.
 *
 *@note <b>NOT multi-app safe</b>
 *
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_SetManualRegisterModeAsync
(
    const char* LE_NONNULL mcc,
        ///< [IN] Mobile Country Code
    const char* LE_NONNULL mnc,
        ///< [IN] Mobile Network Code
    le_mrc_ManualSelectionHandlerFunc_t handlerPtr,
        ///< [IN] handler for sending result
    void* contextPtr
        ///< [IN]
)
{
     ifgen_le_mrc_SetManualRegisterModeAsync(
        GetCurrentSessionRef(),
        mcc,
        mnc,
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the selected Selection Register mode.
 *
 * @return
 *  - LE_FAULT  Function failed.
 *  - LE_OK     Function succeeded.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetRegisterMode
(
    bool* isManualPtrPtr,
        ///< [OUT] true if the scan mode is manual, false if the scan mode is automatic.
    char* mccPtr,
        ///< [OUT] Mobile Country Code
    size_t mccPtrSize,
        ///< [IN]
    char* mncPtr,
        ///< [OUT] Mobile Network Code
    size_t mncPtrSize
        ///< [IN]
)
{
    return ifgen_le_mrc_GetRegisterMode(
        GetCurrentSessionRef(),
        isManualPtrPtr,
        mccPtr,
        mccPtrSize,
        mncPtr,
        mncPtrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Packet Switched state.
 *
 * @return
 *  - LE_FAULT  Function failed.
 *  - LE_OK     Function succeeded.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetPacketSwitchedState
(
    le_mrc_NetRegState_t* statePtr
        ///< [OUT] The current Packet switched state.
)
{
    return ifgen_le_mrc_GetPacketSwitchedState(
        GetCurrentSessionRef(),
        statePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the platform specific network registration error code.
 *
 * Refer to @ref platformConstraintsSpecificErrorCodes for platform specific
 * registration error code description.
 *
 * @return the platform specific registration error code
 *
 */
//--------------------------------------------------------------------------------------------------
int32_t le_mrc_GetPlatformSpecificRegistrationErrorCode
(
    void
)
{
    return ifgen_le_mrc_GetPlatformSpecificRegistrationErrorCode(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the Radio Access Technology preferences by using a bit mask.
 *
 * @return
 *  - LE_FAULT           Function failed.
 *  - LE_OK              Function succeeded.
 *  - LE_UNSUPPORTED     Not supported by platform.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetRatPreferences
(
    le_mrc_RatBitMask_t ratMask
        ///< [IN] Bit mask for the Radio Access Technology preferences.
)
{
    return ifgen_le_mrc_SetRatPreferences(
        GetCurrentSessionRef(),
        ratMask
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Radio Access Technology preferences
 *
 * @return
 * - LE_FAULT  Function failed.
 * - LE_OK     Function succeeded.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetRatPreferences
(
    le_mrc_RatBitMask_t* ratMaskPtrPtr
        ///< [OUT] Bit mask for the Radio Access Technology preferences.
)
{
    return ifgen_le_mrc_GetRatPreferences(
        GetCurrentSessionRef(),
        ratMaskPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the 2G/3G Band preferences by using a bit mask.
 *
 * @return
 *  - LE_FAULT  Function failed.
 *  - LE_OK     Function succeeded.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetBandPreferences
(
    le_mrc_BandBitMask_t bandMask
        ///< [IN] Bit mask for 2G/3G Band preferences.
)
{
    return ifgen_le_mrc_SetBandPreferences(
        GetCurrentSessionRef(),
        bandMask
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Bit mask for 2G/3G Band preferences.
 *
 * @return
 *  - LE_FAULT  Function failed.
 *  - LE_OK     Function succeeded.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetBandPreferences
(
    le_mrc_BandBitMask_t* bandMaskPtrPtr
        ///< [OUT] Bit mask for 2G/3G Band preferences.
)
{
    return ifgen_le_mrc_GetBandPreferences(
        GetCurrentSessionRef(),
        bandMaskPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the LTE Band preferences by using a bit mask.
 *
 * @return
 *  - LE_FAULT        Function failed.
 *  - LE_OK           Function succeeded.
 *  - LE_UNSUPPORTED  The platform doesn't support setting LTE Band preferences.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetLteBandPreferences
(
    le_mrc_LteBandBitMask_t bandMask
        ///< [IN] Bit mask for LTE Band preferences.
)
{
    return ifgen_le_mrc_SetLteBandPreferences(
        GetCurrentSessionRef(),
        bandMask
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Bit mask for LTE Band preferences.
 *
 * @return
 *  - LE_FAULT  Function failed.
 *  - LE_OK     Function succeeded.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetLteBandPreferences
(
    le_mrc_LteBandBitMask_t* bandMaskPtrPtr
        ///< [OUT] Bit mask for LTE Band preferences.
)
{
    return ifgen_le_mrc_GetLteBandPreferences(
        GetCurrentSessionRef(),
        bandMaskPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the TD-SCDMA Band preferences by using a bit mask.
 *
 * @return
 *  - LE_OK           Function succeeded.
 *  - LE_FAULT        Function failed.
 *  - LE_UNSUPPORTED  The platform doesn't support setting TD-SCDMA Band preferences.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetTdScdmaBandPreferences
(
    le_mrc_TdScdmaBandBitMask_t bandMask
        ///< [IN] Bit mask for TD-SCDMA Band preferences.
)
{
    return ifgen_le_mrc_SetTdScdmaBandPreferences(
        GetCurrentSessionRef(),
        bandMask
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Bit mask for TD-SCDMA Band preferences.
 *
 * @return
 *  - LE_OK           Function succeeded.
 *  - LE_FAULT        Function failed.
 *  - LE_UNSUPPORTED  The platform doesn't support getting TD-SCDMA Band preferences.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetTdScdmaBandPreferences
(
    le_mrc_TdScdmaBandBitMask_t* bandMaskPtrPtr
        ///< [OUT] Bit mask for TD-SCDMA Band preferences.
)
{
    return ifgen_le_mrc_GetTdScdmaBandPreferences(
        GetCurrentSessionRef(),
        bandMaskPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add a preferred operator by specifying the MCC/MNC and the Radio Access Technology.
 *
 * @return
 *  - LE_UNSUPPORTED   List of User Preferred operators not available.
 *  - LE_FAULT         Function failed.
 *  - LE_BAD_PARAMETER RAT mask is invalid.
 *  - LE_OK            Function succeeded.
 *
 * @note If strings are not set, too long (bigger than LE_MRC_MCC_LEN/LE_MRC_MNC_LEN digits), or too
 *       short (less than LE_MRC_MCC_LEN/LE_MRC_MNC_LEN-1 digits) it's a fatal error, the function
 *       won't return.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_AddPreferredOperator
(
    const char* LE_NONNULL mcc,
        ///< [IN] Mobile Country Code
    const char* LE_NONNULL mnc,
        ///< [IN] Mobile Network Code
    le_mrc_RatBitMask_t ratMask
        ///< [IN] Bit mask for the Radio Access Technology preferences.
)
{
    return ifgen_le_mrc_AddPreferredOperator(
        GetCurrentSessionRef(),
        mcc,
        mnc,
        ratMask
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove a preferred operator by specifying the MCC/MNC.
 *
 * @return
 *  - LE_UNSUPPORTED    List of User Preferred operators not available.
 *  - LE_NOT_FOUND      Operator not found in the User Preferred operators list.
 *  - LE_FAULT          Function failed.
 *  - LE_OK             Function succeeded.
 *
 * @note If strings are not set, too long (bigger than LE_MRC_MCC_LEN/LE_MRC_MNC_LEN digits), or too
 *       short (less than LE_MRC_MCC_LEN/LE_MRC_MNC_LEN-1 digits) it's a fatal error, the function
 *       won't return.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_RemovePreferredOperator
(
    const char* LE_NONNULL mcc,
        ///< [IN] Mobile Country Code
    const char* LE_NONNULL mnc
        ///< [IN] Mobile Network Code
)
{
    return ifgen_le_mrc_RemovePreferredOperator(
        GetCurrentSessionRef(),
        mcc,
        mnc
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to retrieve a list of the preferred operators.
 *
 * @return
 * - Reference to the List object.
 * - Null pointer if there is no preferences list.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_PreferredOperatorListRef_t le_mrc_GetPreferredOperatorsList
(
    void
)
{
    return ifgen_le_mrc_GetPreferredOperatorsList(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the first Operator object reference in the list of the
 * preferred operators retrieved with le_mrc_GetPreferredOperators().
 *
 * @return
 *  - NULL                          No operator information found.
 *  - le_mrc_PreferredOperatorRef   The Operator object reference.
 *
 * @note If the caller is passing a bad reference into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_PreferredOperatorRef_t le_mrc_GetFirstPreferredOperator
(
    le_mrc_PreferredOperatorListRef_t preferredOperatorListRef
        ///< [IN] The list of the preferred operators.
)
{
    return ifgen_le_mrc_GetFirstPreferredOperator(
        GetCurrentSessionRef(),
        preferredOperatorListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the next Operator object reference in the list of the
 * preferred operators retrieved with le_mrc_GetPreferredOperators().
 *
 * @return
 *  - NULL                          No operator information found.
 *  - le_mrc_PreferredOperatorRef   The Operator object reference.
 *
 * @note If the caller is passing a bad reference into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_PreferredOperatorRef_t le_mrc_GetNextPreferredOperator
(
    le_mrc_PreferredOperatorListRef_t preferredOperatorListRef
        ///< [IN] The list of the preferred operators.
)
{
    return ifgen_le_mrc_GetNextPreferredOperator(
        GetCurrentSessionRef(),
        preferredOperatorListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to delete the list of the preferred operators retrieved with
 * le_mrc_GetPreferredOperators().
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_DeletePreferredOperatorsList
(
    le_mrc_PreferredOperatorListRef_t preferredOperatorListRef
        ///< [IN] The list of the preferred operators.
)
{
     ifgen_le_mrc_DeletePreferredOperatorsList(
        GetCurrentSessionRef(),
        preferredOperatorListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Operator information details.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the MCC or MNC would not fit in buffer
 *      - LE_FAULT for all other errors
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetPreferredOperatorDetails
(
    le_mrc_PreferredOperatorRef_t preferredOperatorRef,
        ///< [IN] Operator object reference.
    char* mccPtr,
        ///< [OUT] Mobile Country Code.
    size_t mccPtrSize,
        ///< [IN]
    char* mncPtr,
        ///< [OUT] Mobile Network Code.
    size_t mncPtrSize,
        ///< [IN]
    le_mrc_RatBitMask_t* ratMaskPtr
        ///< [OUT] Bit mask for the RAT preferences.
)
{
    return ifgen_le_mrc_GetPreferredOperatorDetails(
        GetCurrentSessionRef(),
        preferredOperatorRef,
        mccPtr,
        mccPtrSize,
        mncPtr,
        mncPtrSize,
        ratMaskPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the network registration state.
 *
 * @return LE_FAULT         The function failed to get the Network registration state.
 * @return LE_BAD_PARAMETER A bad parameter was passed.
 * @return LE_OK            The function succeeded.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetNetRegState
(
    le_mrc_NetRegState_t* statePtr
        ///< [OUT] Network Registration state.
)
{
    return ifgen_le_mrc_GetNetRegState(
        GetCurrentSessionRef(),
        statePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the signal quality.
 *
 * @return LE_FAULT         The function failed to get the Signal Quality information.
 * @return LE_BAD_PARAMETER A bad parameter was passed.
 * @return LE_OK            The function succeeded.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetSignalQual
(
    uint32_t* qualityPtr
        ///< [OUT] [OUT] Received signal strength quality (0 = no signal strength,
        ///<        5 = very good signal strength).
)
{
    return ifgen_le_mrc_GetSignalQual(
        GetCurrentSessionRef(),
        qualityPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the power of the Radio Module.
 *
 * @return LE_BAD_PARAMETER Bad power mode specified.
 * @return LE_FAULT         Function failed.
 * @return LE_OK            Function succeed.
 *
 * @note <b>NOT multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetRadioPower
(
    le_onoff_t power
        ///< [IN] The power state.
)
{
    return ifgen_le_mrc_SetRadioPower(
        GetCurrentSessionRef(),
        power
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Must be called to get the Radio Module power state.
 *
 * @return LE_FAULT         The function failed to get the Radio Module power state.
 * @return LE_BAD_PARAMETER if powerPtr is NULL.
 * @return LE_OK            The function succeed.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetRadioPower
(
    le_onoff_t* powerPtr
        ///< [OUT] Power state.
)
{
    return ifgen_le_mrc_GetRadioPower(
        GetCurrentSessionRef(),
        powerPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to perform a cellular network scan.
 *
 * @return Reference to the List object. Null pointer if the scan failed.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_ScanInformationListRef_t le_mrc_PerformCellularNetworkScan
(
    le_mrc_RatBitMask_t ratMask
        ///< [IN] Radio Access Technology mask
)
{
    return ifgen_le_mrc_PerformCellularNetworkScan(
        GetCurrentSessionRef(),
        ratMask
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to perform a PCI network scan.
 *
 * @return Reference to the List object. Null pointer if the scan failed.
 *
 * @warning PCI scan is platform dependent. Please refer to @ref platformConstraintsMdc for further
 *          details.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_PciScanInformationListRef_t le_mrc_PerformPciNetworkScan
(
    le_mrc_RatBitMask_t ratMask
        ///< [IN] Radio Access Technology mask
)
{
    return ifgen_le_mrc_PerformPciNetworkScan(
        GetCurrentSessionRef(),
        ratMask
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to perform a cellular network scan asynchronously. This function
 * is not blocking, the response will be returned with a handler function.
 *
 *@note <b>multi-app safe</b>
 *
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_PerformCellularNetworkScanAsync
(
    le_mrc_RatBitMask_t ratMask,
        ///< [IN] Radio Access Technology mask
    le_mrc_CellularNetworkScanHandlerFunc_t handlerPtr,
        ///< [IN] handler for sending result.
    void* contextPtr
        ///< [IN]
)
{
     ifgen_le_mrc_PerformCellularNetworkScanAsync(
        GetCurrentSessionRef(),
        ratMask,
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to perform a PCI network scan asynchronously. This function
 * is not blocking, the response will be returned with a handler function.
 *
 * @warning PCI scan is platform dependent. Please refer to @ref platformConstraintsMdc for further
 *          details.
 *
 *@note <b>multi-app safe</b>
 *
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_PerformPciNetworkScanAsync
(
    le_mrc_RatBitMask_t ratMask,
        ///< [IN] Radio Access Technology mask
    le_mrc_PciNetworkScanHandlerFunc_t handlerPtr,
        ///< [IN] handler for sending result.
    void* contextPtr
        ///< [IN]
)
{
     ifgen_le_mrc_PerformPciNetworkScanAsync(
        GetCurrentSessionRef(),
        ratMask,
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the first Scan Information object reference in the list of
 * scan Information retrieved with le_mrc_PerformCellularNetworkScan().
 *
 * @return NULL                         No scan information found.
 * @return le_mrc_ScanInformationRef_t  The Scan Information object reference.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_ScanInformationRef_t le_mrc_GetFirstCellularNetworkScan
(
    le_mrc_ScanInformationListRef_t scanInformationListRef
        ///< [IN] The list of scan information.
)
{
    return ifgen_le_mrc_GetFirstCellularNetworkScan(
        GetCurrentSessionRef(),
        scanInformationListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the next Scan Information object reference in the list of
 * scan Information retrieved with le_mrc_PerformCellularNetworkScan().
 *
 * @return NULL                         No scan information found.
 * @return le_mrc_ScanInformationRef_t  The Scan Information object reference.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_ScanInformationRef_t le_mrc_GetNextCellularNetworkScan
(
    le_mrc_ScanInformationListRef_t scanInformationListRef
        ///< [IN] The list of scan information.
)
{
    return ifgen_le_mrc_GetNextCellularNetworkScan(
        GetCurrentSessionRef(),
        scanInformationListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the first Pci Scan Information object reference in the list of
 * scan Information retrieved with le_mrc_PerformPciNetworkScan().
 *
 * @return NULL                         No scan information found.
 * @return The Scan Information object reference.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_PciScanInformationRef_t le_mrc_GetFirstPciScanInfo
(
    le_mrc_PciScanInformationListRef_t scanInformationListRef
        ///< [IN] The list of scan information.
)
{
    return ifgen_le_mrc_GetFirstPciScanInfo(
        GetCurrentSessionRef(),
        scanInformationListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the next Scan Information object reference in the list of
 * scan Information retrieved with le_mrc_PerformPciNetworkScan().
 *
 * @return NULL                         No scan information found.
 * @return The Scan Information object reference.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_PciScanInformationRef_t le_mrc_GetNextPciScanInfo
(
    le_mrc_PciScanInformationListRef_t scanInformationListRef
        ///< [IN] The list of scan information.
)
{
    return ifgen_le_mrc_GetNextPciScanInfo(
        GetCurrentSessionRef(),
        scanInformationListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the first Plmn Information object reference in the list of
 * Plmn on each cell.
 *
 * @return NULL                         No scan information found.
 * @return le_mrc_PlmnInformationRef_t  The Plmn Information object reference.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 */
//--------------------------------------------------------------------------------------------------
le_mrc_PlmnInformationRef_t le_mrc_GetFirstPlmnInfo
(
    le_mrc_PciScanInformationRef_t pciScanInformationRef
        ///< [IN] [IN] The reference to the cell information.
)
{
    return ifgen_le_mrc_GetFirstPlmnInfo(
        GetCurrentSessionRef(),
        pciScanInformationRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the next Plmn Information object reference in the list of
 * Plmn on each cell.
 *
 * @return NULL                         No scan information found.
 * @return le_mrc_PlmnInformationRef_t  The Plmn Information object reference.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 */
//--------------------------------------------------------------------------------------------------
le_mrc_PlmnInformationRef_t le_mrc_GetNextPlmnInfo
(
    le_mrc_PciScanInformationRef_t plmnRef
        ///< [IN] [IN] The reference to the cell information.
)
{
    return ifgen_le_mrc_GetNextPlmnInfo(
        GetCurrentSessionRef(),
        plmnRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the physical cell id referenced by PciScanInformation which
 * is returned by le_mrc_GetFirstPciScanInfo() and le_mrc_GetNextPciScanInfo().
 *
 * @return The Physical Cell Identifier.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 */
//--------------------------------------------------------------------------------------------------
uint16_t le_mrc_GetPciScanCellId
(
    le_mrc_PciScanInformationRef_t pciScanInformationRef
        ///< [IN] [IN] The reference to the cell information.
)
{
    return ifgen_le_mrc_GetPciScanCellId(
        GetCurrentSessionRef(),
        pciScanInformationRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the global cell id referenced by PciScanInformation which is
 * returned by le_mrc_GetFirstPciScanInfo() and le_mrc_GetNextPciScanInfo().
 *
 * @return The Global Cell Identifier.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_mrc_GetPciScanGlobalCellId
(
    le_mrc_PciScanInformationRef_t pciScanInformationRef
        ///< [IN] [IN] The reference to the cell information.
)
{
    return ifgen_le_mrc_GetPciScanGlobalCellId(
        GetCurrentSessionRef(),
        pciScanInformationRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Mcc/Mnc of each PLMN referenced in the list of
 * Plmn Information retrieved with le_mrc_GetFirstPlmnInfo() and le_mrc_GetNextPlmnInfo().
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the MCC or MNC would not fit in buffer
 *      - LE_FAULT for all other errors
 *
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetPciScanMccMnc
(
    le_mrc_PlmnInformationRef_t plmnRef,
        ///< [IN] [IN] The reference to the cell information.
    char* mccPtr,
        ///< [OUT] Mobile Country Code
    size_t mccPtrSize,
        ///< [IN]
    char* mncPtr,
        ///< [OUT] Mobile Network Code
    size_t mncPtrSize
        ///< [IN]
)
{
    return ifgen_le_mrc_GetPciScanMccMnc(
        GetCurrentSessionRef(),
        plmnRef,
        mccPtr,
        mccPtrSize,
        mncPtr,
        mncPtrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to delete the list of the Scan Information retrieved with
 * le_mrc_PerformCellularNetworkScan().
 *
 * @note
 *      On failure, the process exits, so you don't have to worry about checking the returned
 *      reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_DeleteCellularNetworkScan
(
    le_mrc_ScanInformationListRef_t scanInformationListRef
        ///< [IN] The list of scan information.
)
{
     ifgen_le_mrc_DeleteCellularNetworkScan(
        GetCurrentSessionRef(),
        scanInformationListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to delete the list of the Pci Scan Information retrieved with
 * le_mrc_PerformPciNetworkScan().
 *
 * @note
 *      On failure, the process exits, so you don't have to worry about checking the returned
 *      reference for validity.
 *
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_DeletePciNetworkScan
(
    le_mrc_PciScanInformationListRef_t scanInformationListRef
        ///< [IN] The list of scan information.
)
{
     ifgen_le_mrc_DeletePciNetworkScan(
        GetCurrentSessionRef(),
        scanInformationListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Cellular Network Code [mcc:mnc]
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the MCC or MNC would not fit in buffer
 *      - LE_FAULT for all other errors
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetCellularNetworkMccMnc
(
    le_mrc_ScanInformationRef_t scanInformationRef,
        ///< [IN] Scan information reference
    char* mccPtr,
        ///< [OUT] Mobile Country Code
    size_t mccPtrSize,
        ///< [IN]
    char* mncPtr,
        ///< [OUT] Mobile Network Code
    size_t mncPtrSize
        ///< [IN]
)
{
    return ifgen_le_mrc_GetCellularNetworkMccMnc(
        GetCurrentSessionRef(),
        scanInformationRef,
        mccPtr,
        mccPtrSize,
        mncPtr,
        mncPtrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Cellular Network Name.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the operator name would not fit in buffer
 *      - LE_FAULT for all other errors
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetCellularNetworkName
(
    le_mrc_ScanInformationRef_t scanInformationRef,
        ///< [IN] Scan information reference
    char* namePtr,
        ///< [OUT] Name of operator
    size_t namePtrSize
        ///< [IN]
)
{
    return ifgen_le_mrc_GetCellularNetworkName(
        GetCurrentSessionRef(),
        scanInformationRef,
        namePtr,
        namePtrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the radio access technology of a scanInformationRef.
 *
 * @return the radio access technology
 *
 * @note On failure, the process exits.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_Rat_t le_mrc_GetCellularNetworkRat
(
    le_mrc_ScanInformationRef_t scanInformationRef
        ///< [IN] Scan information reference
)
{
    return ifgen_le_mrc_GetCellularNetworkRat(
        GetCurrentSessionRef(),
        scanInformationRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to check if a cellular network is currently in use.
 *
 * @return true     The network is in use
 * @return false    The network isn't in use
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
bool le_mrc_IsCellularNetworkInUse
(
    le_mrc_ScanInformationRef_t scanInformationRef
        ///< [IN] Scan information reference
)
{
    return ifgen_le_mrc_IsCellularNetworkInUse(
        GetCurrentSessionRef(),
        scanInformationRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to check if a cellular network is available.
 *
 * @return true     The network is available
 * @return false    The network isn't available
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
bool le_mrc_IsCellularNetworkAvailable
(
    le_mrc_ScanInformationRef_t scanInformationRef
        ///< [IN] Scan information reference
)
{
    return ifgen_le_mrc_IsCellularNetworkAvailable(
        GetCurrentSessionRef(),
        scanInformationRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to check if a cellular network is currently in home mode.
 *
 * @return true     The network is home
 * @return false    The network is roaming
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
bool le_mrc_IsCellularNetworkHome
(
    le_mrc_ScanInformationRef_t scanInformationRef
        ///< [IN] Scan information reference
)
{
    return ifgen_le_mrc_IsCellularNetworkHome(
        GetCurrentSessionRef(),
        scanInformationRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to check if a cellular network is forbidden by the operator.
 *
 * @return true     The network is forbidden
 * @return false    The network is allowed
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
bool le_mrc_IsCellularNetworkForbidden
(
    le_mrc_ScanInformationRef_t scanInformationRef
        ///< [IN] Scan information reference
)
{
    return ifgen_le_mrc_IsCellularNetworkForbidden(
        GetCurrentSessionRef(),
        scanInformationRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the current network name information.
 *
 * @return
 *      - LE_OK             on success
 *      - LE_BAD_PARAMETER  if nameStr is NULL
 *      - LE_OVERFLOW       if the Home Network Name can't fit in nameStr
 *      - LE_FAULT          on any other failure
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetCurrentNetworkName
(
    char* nameStr,
        ///< [OUT] the home network Name
    size_t nameStrSize
        ///< [IN]
)
{
    return ifgen_le_mrc_GetCurrentNetworkName(
        GetCurrentSessionRef(),
        nameStr,
        nameStrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the current network PLMN information.
 *
 * @return
 *      - LE_OK       on success
 *      - LE_FAULT    on any other failure
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetCurrentNetworkMccMnc
(
    char* mccStr,
        ///< [OUT] the mobile country code
    size_t mccStrSize,
        ///< [IN]
    char* mncStr,
        ///< [OUT] the mobile network code
    size_t mncStrSize
        ///< [IN]
)
{
    return ifgen_le_mrc_GetCurrentNetworkMccMnc(
        GetCurrentSessionRef(),
        mccStr,
        mccStrSize,
        mncStr,
        mncStrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the current Radio Access Technology in use.
 *
 * @return LE_OK            Function succeeded.
 * @return LE_BAD_PARAMETER Invalid parameter.
 * @return LE_FAULT         Function failed to get the Radio Access Technology.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note The API returns the RAT only if the device is registered on the network.
 *       le_mrc_GetNetRegState() function can be called first to obtain the network registration
 *       state.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetRadioAccessTechInUse
(
    le_mrc_Rat_t* ratPtr
        ///< [OUT] The Radio Access Technology.
)
{
    return ifgen_le_mrc_GetRadioAccessTechInUse(
        GetCurrentSessionRef(),
        ratPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to retrieve the Neighboring Cells information. It creates and
 * returns a reference to the Neighboring Cells information.
 *
 * @return A reference to the Neighboring Cells information.
 * @return NULL if no Cells Information are available.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_NeighborCellsRef_t le_mrc_GetNeighborCellsInfo
(
    void
)
{
    return ifgen_le_mrc_GetNeighborCellsInfo(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to delete the Neighboring Cells information.
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_DeleteNeighborCellsInfo
(
    le_mrc_NeighborCellsRef_t ngbrCellsRef
        ///< [IN] Neighboring Cells reference.
)
{
     ifgen_le_mrc_DeleteNeighborCellsInfo(
        GetCurrentSessionRef(),
        ngbrCellsRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the first Cell Information reference in the list of
 * Neighboring Cells information retrieved with le_mrc_GetNeighborCellsInfo().
 *
 * @return NULL                   No Cell information object found.
 * @return le_mrc_CellInfoRef_t   The Cell information object reference.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_CellInfoRef_t le_mrc_GetFirstNeighborCellInfo
(
    le_mrc_NeighborCellsRef_t ngbrCellsRef
        ///< [IN] Neighboring Cells reference.
)
{
    return ifgen_le_mrc_GetFirstNeighborCellInfo(
        GetCurrentSessionRef(),
        ngbrCellsRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the next Cell Information reference in the list of
 * Neighboring Cells information retrieved with le_mrc_GetNeighborCellsInfo().
 *
 * @return NULL                   No Cell information object found.
 * @return le_mrc_CellInfoRef_t   Cell information object reference.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_CellInfoRef_t le_mrc_GetNextNeighborCellInfo
(
    le_mrc_NeighborCellsRef_t ngbrCellsRef
        ///< [IN] Neighboring Cells reference.
)
{
    return ifgen_le_mrc_GetNextNeighborCellInfo(
        GetCurrentSessionRef(),
        ngbrCellsRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Physical Cell Identifier.
 *
 * @return The Physical Cell Identifier. UINT32_MAX value is returned if the Cell Identifier is not
 * available.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_mrc_GetNeighborCellId
(
    le_mrc_CellInfoRef_t ngbrCellInfoRef
        ///< [IN] Cell information reference.
)
{
    return ifgen_le_mrc_GetNeighborCellId(
        GetCurrentSessionRef(),
        ngbrCellInfoRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Location Area Code of a cell.
 *
 * @return The Location Area Code of a cell. UINT16_MAX value is returned if the value is not
 * available.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_mrc_GetNeighborCellLocAreaCode
(
    le_mrc_CellInfoRef_t ngbrCellInfoRef
        ///< [IN] Cell information reference.
)
{
    return ifgen_le_mrc_GetNeighborCellLocAreaCode(
        GetCurrentSessionRef(),
        ngbrCellInfoRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the signal strength of a cell.
 *
 * @return The signal strength of a cell in dBm.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
int32_t le_mrc_GetNeighborCellRxLevel
(
    le_mrc_CellInfoRef_t ngbrCellInfoRef
        ///< [IN] Cell information reference.
)
{
    return ifgen_le_mrc_GetNeighborCellRxLevel(
        GetCurrentSessionRef(),
        ngbrCellInfoRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Radio Access Technology of a cell.
 *
 * @return The Radio Access Technology of a cell.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_Rat_t le_mrc_GetNeighborCellRat
(
    le_mrc_CellInfoRef_t ngbrCellInfoRef
        ///< [IN] Cell information reference.
)
{
    return ifgen_le_mrc_GetNeighborCellRat(
        GetCurrentSessionRef(),
        ngbrCellInfoRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Ec/Io; the received energy per chip divided by the power
 * density in the band measured in dBm on the primary CPICH channel of serving cell (negative value)
 *
 * @return
 *  - The Ec/Io of a cell given in dB with 1 decimal place. (only applicable for UMTS network).
 *  - INT32_MAX when the value isn't available.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
int32_t le_mrc_GetNeighborCellUmtsEcIo
(
    le_mrc_CellInfoRef_t ngbrCellInfoRef
        ///< [IN] Cell information reference.
)
{
    return ifgen_le_mrc_GetNeighborCellUmtsEcIo(
        GetCurrentSessionRef(),
        ngbrCellInfoRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the RSRP and RSRQ of the Intrafrequency of a LTE cell.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetNeighborCellLteIntraFreq
(
    le_mrc_CellInfoRef_t ngbrCellInfoRef,
        ///< [IN] Cell information reference.
    int32_t* rsrqPtr,
        ///< [OUT] Reference Signal Received Quality value in dB with 1 decimal
        ///< place
    int32_t* rsrpPtr
        ///< [OUT] Reference Signal Receiver Power value in dBm with 1 decimal
        ///< place
)
{
    return ifgen_le_mrc_GetNeighborCellLteIntraFreq(
        GetCurrentSessionRef(),
        ngbrCellInfoRef,
        rsrqPtr,
        rsrpPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the RSRP and RSRQ of the Interfrequency of a LTE cell.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetNeighborCellLteInterFreq
(
    le_mrc_CellInfoRef_t ngbrCellInfoRef,
        ///< [IN] Cell information reference.
    int32_t* rsrqPtr,
        ///< [OUT] Reference Signal Received Quality value in dB with 1 decimal
        ///< place
    int32_t* rsrpPtr
        ///< [OUT] Reference Signal Receiver Power value in dBm with 1 decimal
        ///< place
)
{
    return ifgen_le_mrc_GetNeighborCellLteInterFreq(
        GetCurrentSessionRef(),
        ngbrCellInfoRef,
        rsrqPtr,
        rsrpPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to measure the signal metrics. It creates and returns a reference
 * to the signal metrics.
 *
 * @return A reference to the signal metrics.
 * @return NULL if no signal metrics are available.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_MetricsRef_t le_mrc_MeasureSignalMetrics
(
    void
)
{
    return ifgen_le_mrc_MeasureSignalMetrics(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to delete the the signal metrics.
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_DeleteSignalMetrics
(
    le_mrc_MetricsRef_t MetricsRef
        ///< [IN] Signal metrics reference.
)
{
     ifgen_le_mrc_DeleteSignalMetrics(
        GetCurrentSessionRef(),
        MetricsRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function returns the Radio Access Technology of the signal metrics.
 *
 * @return The Radio Access Technology of the signal measure.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_mrc_Rat_t le_mrc_GetRatOfSignalMetrics
(
    le_mrc_MetricsRef_t MetricsRef
        ///< [IN] Signal metrics reference.
)
{
    return ifgen_le_mrc_GetRatOfSignalMetrics(
        GetCurrentSessionRef(),
        MetricsRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function returns the signal strength in dBm and the bit error rate measured on GSM network.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetGsmSignalMetrics
(
    le_mrc_MetricsRef_t MetricsRef,
        ///< [IN] Signal metrics reference.
    int32_t* rssiPtr,
        ///< [OUT] Signal strength in dBm
    uint32_t* berPtr
        ///< [OUT] Bit error rate.
)
{
    return ifgen_le_mrc_GetGsmSignalMetrics(
        GetCurrentSessionRef(),
        MetricsRef,
        rssiPtr,
        berPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function returns the signal metrics measured on UMTS or TD-SCDMA networks.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetUmtsSignalMetrics
(
    le_mrc_MetricsRef_t MetricsRef,
        ///< [IN] Signal metrics reference.
    int32_t* ssPtr,
        ///< [OUT] Signal strength in dBm
    uint32_t* blerPtr,
        ///< [OUT] Block error rate
    int32_t* ecioPtr,
        ///< [OUT] Ec/Io value in dB with 1 decimal place (-15 = -1.5 dB) (Negative
        ///< value)
    int32_t* rscpPtr,
        ///< [OUT] Measured RSCP in dBm (negative value, value INT32_MAX means
        ///< that RSCP is not available)
    int32_t* sinrPtr
        ///< [OUT] Measured SINR in dB (only applicable for TD-SCDMA network, value
        ///< INT32_MAX means that the SINR is not available)
)
{
    return ifgen_le_mrc_GetUmtsSignalMetrics(
        GetCurrentSessionRef(),
        MetricsRef,
        ssPtr,
        blerPtr,
        ecioPtr,
        rscpPtr,
        sinrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function returns the signal metrics measured on LTE network.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 *      - sinrPtr and ioPtr return INT32_MAX when the value isn't available.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetLteSignalMetrics
(
    le_mrc_MetricsRef_t MetricsRef,
        ///< [IN] Signal metrics reference.
    int32_t* ssPtr,
        ///< [OUT] Signal strength in dBm
    uint32_t* blerPtr,
        ///< [OUT] Block error rate
    int32_t* rsrqPtr,
        ///< [OUT] RSRQ value in dB as measured by L1 with 1 decimal place
    int32_t* rsrpPtr,
        ///< [OUT] Current RSRP in dBm as measured by L1 with 1 decimal place
    int32_t* snrPtr
        ///< [OUT] SNR level in dB with 1 decimal place (15 = 1.5 dB)
)
{
    return ifgen_le_mrc_GetLteSignalMetrics(
        GetCurrentSessionRef(),
        MetricsRef,
        ssPtr,
        blerPtr,
        rsrqPtr,
        rsrpPtr,
        snrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function returns the signal metrics measured on CDMA network.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 *      - rscpPtr and sinrPtr return INT32_MAX when the value isn't available.
 *
 * @note If the caller is passing a bad pointer into this function, it's a fatal error, the
 *       function won't return.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetCdmaSignalMetrics
(
    le_mrc_MetricsRef_t MetricsRef,
        ///< [IN] Signal metrics reference.
    int32_t* ssPtr,
        ///< [OUT] Signal strength in dBm
    uint32_t* erPtr,
        ///< [OUT] Frame/Packet error rate
    int32_t* ecioPtr,
        ///< [OUT] ECIO value in dB with 1 decimal place (-15 = -1.5 dB) (Negative
        ///< value)
    int32_t* sinrPtr,
        ///< [OUT] SINR level in dB with 1 decimal place, (only applicable for
        ///< 1xEV-DO, value INT32_MAX means that the value is not available)
    int32_t* ioPtr
        ///< [OUT] Received IO in dBm (only applicable for 1xEV-DO, value INT32_MAX
        ///< means that the value is not available)
)
{
    return ifgen_le_mrc_GetCdmaSignalMetrics(
        GetCurrentSessionRef(),
        MetricsRef,
        ssPtr,
        erPtr,
        ecioPtr,
        sinrPtr,
        ioPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the serving Cell Identifier.
 *
 * @return The Cell Identifier. UINT32_MAX value is returned if the value is not available.
 *
 * @note When the module is in UMTS network, the API returns the serving UTRAN Cell Identity (UC-Id)
 * which is used to identify the cell uniquely.
 * It is composed of the Controlling Radio Network Controller Identifier (CRNC-Id, 12 bits) and the
 * Cell Identifier (C-Id, 16 bits). (3GPP 25.401, section 6.1.5)
 * The Cell Identifier is coded in the lower 2 bytes of the 4 bytes UC-Id and the CRNC-Id is coded
 * in the upper 2 bytes.
 * Example: the API returns 7807609 -> 0x772279 (CRNC-Id = 0x77 , cell ID = 0x2279)
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_mrc_GetServingCellId
(
    void
)
{
    return ifgen_le_mrc_GetServingCellId(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Location Area Code of the serving cell.
 *
 * @return The Location Area Code. UINT32_MAX value is returned if the value is not available.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
uint32_t le_mrc_GetServingCellLocAreaCode
(
    void
)
{
    return ifgen_le_mrc_GetServingCellLocAreaCode(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Tracking Area Code of the serving cell (LTE only).
 *
 * @return The Tracking Area Code. UINT16_MAX value is returned if the value is not available.
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
uint16_t le_mrc_GetServingCellLteTracAreaCode
(
    void
)
{
    return ifgen_le_mrc_GetServingCellLteTracAreaCode(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Bit mask for 2G/3G Band capabilities.
 *
 * @return
 *  - LE_OK              on success
 *  - LE_FAULT           on failure
 *  - LE_UNSUPPORTED     Unable to get the 2G/3G Band capabilities on this platform
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetBandCapabilities
(
    le_mrc_BandBitMask_t* bandMaskPtrPtr
        ///< [OUT] Bit mask for 2G/3G Band capabilities.
)
{
    return ifgen_le_mrc_GetBandCapabilities(
        GetCurrentSessionRef(),
        bandMaskPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Bit mask for LTE Band capabilities.
 *
 * @return
 *  - LE_OK              on success
 *  - LE_FAULT           on failure
 *  - LE_UNSUPPORTED     Unable to get the LTE Band capabilities on this platform
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetLteBandCapabilities
(
    le_mrc_LteBandBitMask_t* bandMaskPtrPtr
        ///< [OUT] Bit mask for LTE Band capabilities.
)
{
    return ifgen_le_mrc_GetLteBandCapabilities(
        GetCurrentSessionRef(),
        bandMaskPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Bit mask for TD-SCDMA Band capabilities.
 *
 * @return
 *  - LE_OK              on success
 *  - LE_FAULT           on failure
 *  - LE_UNSUPPORTED     Unable to get the TD-SCDMA Band Capabilities on this platform
 *
 * @note <b>multi-app safe</b>
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetTdScdmaBandCapabilities
(
    le_mrc_TdScdmaBandBitMask_t* bandMaskPtrPtr
        ///< [OUT] Bit mask for TD-SCDMA Band capabilities.
)
{
    return ifgen_le_mrc_GetTdScdmaBandCapabilities(
        GetCurrentSessionRef(),
        bandMaskPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mrc_NetworkReject'
 *
 * Event to report network reject indication.
 *
 */
//--------------------------------------------------------------------------------------------------
le_mrc_NetworkRejectHandlerRef_t le_mrc_AddNetworkRejectHandler
(
    le_mrc_NetworkRejectHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_mrc_AddNetworkRejectHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mrc_NetworkReject'
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_RemoveNetworkRejectHandler
(
    le_mrc_NetworkRejectHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_mrc_RemoveNetworkRejectHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mrc_NetRegReject'
 *
 * Event to report network registration reject indication.
 * NetworkReject will be deprecated.
 */
//--------------------------------------------------------------------------------------------------
le_mrc_NetRegRejectHandlerRef_t le_mrc_AddNetRegRejectHandler
(
    le_mrc_NetRegRejectHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_mrc_AddNetRegRejectHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mrc_NetRegReject'
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_RemoveNetRegRejectHandler
(
    le_mrc_NetRegRejectHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_mrc_RemoveNetRegRejectHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_mrc_JammingDetectionEvent'
 *
 * This event provides information on jamming detection.
 */
//--------------------------------------------------------------------------------------------------
le_mrc_JammingDetectionEventHandlerRef_t le_mrc_AddJammingDetectionEventHandler
(
    le_mrc_JammingDetectionHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_mrc_AddJammingDetectionEventHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_mrc_JammingDetectionEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_mrc_RemoveJammingDetectionEventHandler
(
    le_mrc_JammingDetectionEventHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_mrc_RemoveJammingDetectionEventHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Start the jamming detection monitoring.
 *
 * @warning The jamming detection feature might be limited by the platform.
 *          Please refer to the platform documentation @ref platformConstraintsMdc.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_FAULT          The function failed.
 *  - LE_DUPLICATE      The feature is already activated and an activation is requested.
 *  - LE_UNSUPPORTED    The feature is not supported.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_StartJammingDetection
(
    void
)
{
    return ifgen_le_mrc_StartJammingDetection(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Stop the jamming detection monitoring.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_FAULT          The function failed or the application did not start the jamming detection.
 *  - LE_UNSUPPORTED    The feature is not supported.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_StopJammingDetection
(
    void
)
{
    return ifgen_le_mrc_StopJammingDetection(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the SAR backoff state
 *
 * @warning The SAR backoff feature might be unsupported by some platforms.
 *          Please refer to the platform documentation @ref platformConstraintsMdc.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_FAULT          The function failed.
 *  - LE_UNSUPPORTED    The feature is not supported.
 *  - LE_OUT_OF_RANGE   The provided index is out of range.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_SetSarBackoffState
(
    uint8_t state
        ///< [IN] New state to enable. By default, SAR is disabled (state = 0).
        ///< Refer to @ref platformConstraitsMdc for the number of maximum states.
)
{
    return ifgen_le_mrc_SetSarBackoffState(
        GetCurrentSessionRef(),
        state
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the SAR backoff state
 *
 * @warning The SAR backoff feature might be unsupported by some platforms.
 *          Please refer to the platform documentation @ref platformConstraintsMdc.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_FAULT          The function failed.
 *  - LE_UNSUPPORTED    The feature is not supported.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_mrc_GetSarBackoffState
(
    uint8_t* statePtr
        ///< [OUT] Current state. By default, SAR is disabled (state = 0).
        ///< Refer to @ref platformConstraitsMdc for the number of maximum states.
)
{
    return ifgen_le_mrc_GetSarBackoffState(
        GetCurrentSessionRef(),
        statePtr
    );
}