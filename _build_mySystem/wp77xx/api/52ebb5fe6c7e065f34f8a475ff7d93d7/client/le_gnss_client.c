/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "le_gnss_interface.h"
#include "le_gnss_messages.h"
#include "le_gnss_service.h"


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
    le_gnss_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for client threads.
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(le_gnss_ClientThreadData,
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
    le_result_t result = ifgen_le_gnss_OpenSession(sessionRef, isBlocking);
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
    if (ifgen_le_gnss_HasLocalBinding())
    {
        return NULL;
    }
    else
    {
        _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

        // If the thread specific data is NULL, then the session ref has not been created.
        LE_FATAL_IF(clientThreadPtr==NULL,
                    "le_gnss_ConnectService() not called for current thread");

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
    ifgen_le_gnss_InitCommonData();

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_InitStaticPool(le_gnss_ClientThreadData,
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
void le_gnss_ConnectService
(
    void
)
{
    if (!ifgen_le_gnss_HasLocalBinding())
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
le_result_t le_gnss_TryConnectService
(
    void
)
{
    if (ifgen_le_gnss_HasLocalBinding())
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

    LE_FATAL("Component for le_gnss disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void le_gnss_SetServerDisconnectHandler
(
    le_gnss_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    if (ifgen_le_gnss_HasLocalBinding())
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
void le_gnss_DisconnectService
(
    void
)
{
    if (ifgen_le_gnss_HasLocalBinding())
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
 * Set the GNSS constellation bit mask
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_UNSUPPORTED   If the request is not supported.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized, disabled or active.
 *  - LE_OK            The function succeeded.
 *
 * @warning Some constellation types are unsupported depending on the plateform. Please refer to
 *          @ref platformConstraintsGnss_ConstellationType section for full details.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetConstellation
(
    le_gnss_ConstellationBitMask_t constellationMask
        ///< [IN] GNSS constellation used in solution.
)
{
    return ifgen_le_gnss_SetConstellation(
        GetCurrentSessionRef(),
        constellationMask
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the GNSS constellation bit mask
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *
 * @note If the caller is passing a null pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetConstellation
(
    le_gnss_ConstellationBitMask_t* constellationMaskPtr
        ///< [OUT] GNSS constellation used in solution.
)
{
    return ifgen_le_gnss_GetConstellation(
        GetCurrentSessionRef(),
        constellationMaskPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the area for the GNSS constellation
 *
 * @return
 *  - LE_OK            The function succeeded.
 *  - LE_FAULT         The function failed.
 *  - LE_UNSUPPORTED   If the request is not supported.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized, disabled or active.
 *  - LE_BAD_PARAMETER Invalid constellation area.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetConstellationArea
(
    le_gnss_Constellation_t satConstellation,
        ///< [IN] GNSS constellation type.
    le_gnss_ConstellationArea_t constellationArea
        ///< [IN] GNSS constellation area.
)
{
    return ifgen_le_gnss_SetConstellationArea(
        GetCurrentSessionRef(),
        satConstellation,
        constellationArea
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the area for the GNSS constellation
 *
 * @return
 *  - LE_OK            On success
 *  - LE_FAULT         On failure
 *  - LE_UNSUPPORTED   Request not supported
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized, disabled or active.
 *
 * @note If the caller is passing a null pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetConstellationArea
(
    le_gnss_Constellation_t satConstellation,
        ///< [IN] GNSS constellation type.
    le_gnss_ConstellationArea_t* constellationAreaPtr
        ///< [OUT] GNSS constellation area.
)
{
    return ifgen_le_gnss_GetConstellationArea(
        GetCurrentSessionRef(),
        satConstellation,
        constellationAreaPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function enables the use of the 'Extended Ephemeris' file into the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_OK            The function succeeded.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_EnableExtendedEphemerisFile
(
    void
)
{
    return ifgen_le_gnss_EnableExtendedEphemerisFile(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function disables the use of the 'Extended Ephemeris' file into the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_OK            The function succeeded.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_DisableExtendedEphemerisFile
(
    void
)
{
    return ifgen_le_gnss_DisableExtendedEphemerisFile(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to load an 'Extended Ephemeris' file into the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed to inject the 'Extended Ephemeris' file.
 *  - LE_TIMEOUT       A time-out occurred.
 *  - LE_FORMAT_ERROR  'Extended Ephemeris' file format error.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_LoadExtendedEphemerisFile
(
    int fd
        ///< [IN] Extended ephemeris file descriptor
)
{
    return ifgen_le_gnss_LoadExtendedEphemerisFile(
        GetCurrentSessionRef(),
        fd
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the validity of the last injected Extended Ephemeris.
 *
 * @return
 *  - LE_FAULT         The function failed to get the validity
 *  - LE_OK            The function succeeded.
 *
 * @note If the caller is passing an invalid Position sample reference or null pointers into this
 *       function, it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetExtendedEphemerisValidity
(
    uint64_t* startTimePtr,
        ///< [OUT] Start time in seconds (since Jan. 1, 1970)
    uint64_t* stopTimePtr
        ///< [OUT] Stop time in seconds (since Jan. 1, 1970)
)
{
    return ifgen_le_gnss_GetExtendedEphemerisValidity(
        GetCurrentSessionRef(),
        startTimePtr,
        stopTimePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to inject the UTC time into the GNSS device.
 *
 * @return
 *  - LE_OK            The function succeeded.
 *  - LE_FAULT         The function failed to inject the UTC time.
 *  - LE_TIMEOUT       A time-out occurred.
 *
 * @note It is mandatory to enable the 'Extended Ephemeris' file injection into the GNSS device with
 * le_gnss_EnableExtendedEphemerisFile() before injecting time with this API.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_InjectUtcTime
(
    uint64_t timeUtc,
        ///< [IN] [IN] UTC time since Jan. 1, 1970 in milliseconds
    uint32_t timeUnc
        ///< [IN] [IN] Time uncertainty in milliseconds
)
{
    return ifgen_le_gnss_InjectUtcTime(
        GetCurrentSessionRef(),
        timeUtc,
        timeUnc
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function starts the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_DUPLICATE     If the GNSS device is already started.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized or disabled.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_Start
(
    void
)
{
    return ifgen_le_gnss_Start(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function stops the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_DUPLICATE     If the GNSS device is already stopped.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized or disabled.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_Stop
(
    void
)
{
    return ifgen_le_gnss_Stop(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function performs a "HOT" restart of the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_ForceHotRestart
(
    void
)
{
    return ifgen_le_gnss_ForceHotRestart(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function performs a "WARM" restart of the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            The function succeeded.
 *
 * @Note This API has a platform dependent feature. Please refer to
 * @ref platformConstraintsGnss_WarmRestart for further details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_ForceWarmRestart
(
    void
)
{
    return ifgen_le_gnss_ForceWarmRestart(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function performs a "COLD" restart of the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_ForceColdRestart
(
    void
)
{
    return ifgen_le_gnss_ForceColdRestart(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function performs a "FACTORY" restart of the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            The function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_ForceFactoryRestart
(
    void
)
{
    return ifgen_le_gnss_ForceFactoryRestart(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the TTFF in milliseconds
 *
 * @return
 *  - LE_BUSY          The position is not fixed and TTFF can't be measured.
 *  - LE_NOT_PERMITTED If the GNSS device is not enabled or not started.
 *  - LE_OK            Function succeeded.
 *  - LE_FAULT         If there are some other errors.
 *
 * @note If the caller is passing a null pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetTtff
(
    uint32_t* ttffPtr
        ///< [OUT] TTFF in milliseconds
)
{
    return ifgen_le_gnss_GetTtff(
        GetCurrentSessionRef(),
        ttffPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function enables the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_DUPLICATE     If the GNSS device is already enabled.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized.
 *  - LE_OK            The function succeeded.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_Enable
(
    void
)
{
    return ifgen_le_gnss_Enable(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function disables the GNSS device.
 *
 * @return
 *  - LE_FAULT         The function failed.
 *  - LE_DUPLICATE     If the GNSS device is already disabled.
 *  - LE_NOT_PERMITTED If the GNSS device is not initialized or started.
 *  - LE_OK            The function succeeded.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_Disable
(
    void
)
{
    return ifgen_le_gnss_Disable(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the GNSS device acquisition rate.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED request not supported
 *  - LE_TIMEOUT a time-out occurred
 *  - LE_NOT_PERMITTED If the GNSS device is not in "ready" state.
 *  - LE_OUT_OF_RANGE  if acquisition rate value is equal to zero
 *
 * @warning This function may be subject to limitations depending on the platform. Please refer to
 *          the @ref platformConstraintsGnss page.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetAcquisitionRate
(
    uint32_t rate
        ///< [IN] Acquisition rate in milliseconds.
)
{
    return ifgen_le_gnss_SetAcquisitionRate(
        GetCurrentSessionRef(),
        rate
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the GNSS device acquisition rate.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_NOT_PERMITTED If the GNSS device is not in "ready" state.
 *
 * @note If the caller is passing a null pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetAcquisitionRate
(
    uint32_t* ratePtr
        ///< [OUT] Acquisition rate in milliseconds.
)
{
    return ifgen_le_gnss_GetAcquisitionRate(
        GetCurrentSessionRef(),
        ratePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_gnss_Position'
 *
 * This event provides information on position.
 *
 *  - A handler reference, which is only needed for later removal of the handler.
 *
 * @note Doesn't return on failure, so there's no need to check the return value for errors.
 */
//--------------------------------------------------------------------------------------------------
le_gnss_PositionHandlerRef_t le_gnss_AddPositionHandler
(
    le_gnss_PositionHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_gnss_AddPositionHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_gnss_Position'
 */
//--------------------------------------------------------------------------------------------------
void le_gnss_RemovePositionHandler
(
    le_gnss_PositionHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_gnss_RemovePositionHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the position sample's fix state
 *
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *
 * @note If the caller is passing an invalid Position sample reference or a null pointer into this
 *       function, it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetPositionState
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    le_gnss_FixState_t* statePtr
        ///< [OUT] Position fix state.
)
{
    return ifgen_le_gnss_GetPositionState(
        GetCurrentSessionRef(),
        positionSampleRef,
        statePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the location's data (Latitude, Longitude, Horizontal accuracy).
 *
 * @return
 *  - LE_FAULT         Function failed to get the location's data
 *  - LE_OUT_OF_RANGE  At least one of the retrieved parameters is invalid (set to INT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note latitudePtr, longitudePtr and hAccuracyPtr can be set to NULL if not needed.
 *
 * @note The latitude and longitude values are based on the WGS84 standard coordinate system.
 *
 * @note The latitude and longitude values are given in degrees with 6 decimal places like:
 *       Latitude +48858300 = 48.858300 degrees North
 *       Longitude +2294400 = 2.294400 degrees East
 *       (The latitude and longitude values are given in degrees, minutes, seconds in NMEA frame)
 *
 * @note In case the function returns LE_OUT_OF_RANGE, some of the retrieved parameters may be
 *       valid. Please compare them with INT32_MAX.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetLocation
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* latitudePtr,
        ///< [OUT] WGS84 Latitude in degrees, positive North [resolution 1e-6].
    int32_t* longitudePtr,
        ///< [OUT] WGS84 Longitude in degrees, positive East [resolution 1e-6].
    int32_t* hAccuracyPtr
        ///< [OUT] Horizontal position's accuracy in meters [resolution 1e-2].
)
{
    return ifgen_le_gnss_GetLocation(
        GetCurrentSessionRef(),
        positionSampleRef,
        latitudePtr,
        longitudePtr,
        hAccuracyPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's altitude.
 *
 * @return
 *  - LE_FAULT         Function failed to get the altitude. Invalid Position reference provided.
 *  - LE_OUT_OF_RANGE  At least one of the retrieved parameters is invalid (set to INT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note Altitude is in meters, above Mean Sea Level, with 3 decimal places (3047 = 3.047 meters).
 *
 * @note For a 2D position fix, the altitude will be indicated as invalid and set to INT32_MAX
 *
 * @note Vertical position accuracy is default set to meters with 1 decimal place (3047 = 3.0
 *       meters). To change its accuracy, call the @c le_gnss_SetDataResolution() function. Vertical
 *       position accuracy is set as data type and accuracy from 0 to 3 decimal place is set as
 *       resolution.
 *
 * @note In case the function returns LE_OUT_OF_RANGE, some of the retrieved parameters may be
 *       valid. Please compare them with INT32_MAX.
 *
 * @note If the caller is passing an invalid Position reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @note altitudePtr, altitudeAccuracyPtr can be set to NULL if not needed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetAltitude
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* altitudePtr,
        ///< [OUT] Altitude in meters, above Mean Sea Level [resolution 1e-3].
    int32_t* vAccuracyPtr
        ///< [OUT] Vertical position's accuracy in meters.
)
{
    return ifgen_le_gnss_GetAltitude(
        GetCurrentSessionRef(),
        positionSampleRef,
        altitudePtr,
        vAccuracyPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's time.
 *
 * @return
 *  - LE_FAULT         Function failed to get the time.
 *  - LE_OUT_OF_RANGE  The retrieved time is invalid (all fields are set to 0).
 *  - LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position sample reference or null pointers into this
 *       function, it is a fatal error, the function will not return.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetTime
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* hoursPtr,
        ///< [OUT] UTC Hours into the day [range 0..23].
    uint16_t* minutesPtr,
        ///< [OUT] UTC Minutes into the hour [range 0..59].
    uint16_t* secondsPtr,
        ///< [OUT] UTC Seconds into the minute [range 0..59].
    uint16_t* millisecondsPtr
        ///< [OUT] UTC Milliseconds into the second [range 0..999].
)
{
    return ifgen_le_gnss_GetTime(
        GetCurrentSessionRef(),
        positionSampleRef,
        hoursPtr,
        minutesPtr,
        secondsPtr,
        millisecondsPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's GPS time.
 *
 * @return
 *  - LE_FAULT         Function failed to get the time.
 *  - LE_OUT_OF_RANGE  The retrieved time is invalid (all fields are set to 0).
 *  - LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position sample reference or null pointers into this
 *       function, it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetGpsTime
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* gpsWeekPtr,
        ///< [OUT] GPS week number from midnight, Jan. 6, 1980.
    uint32_t* gpsTimeOfWeekPtr
        ///< [OUT] Amount of time in milliseconds into the GPS week.
)
{
    return ifgen_le_gnss_GetGpsTime(
        GetCurrentSessionRef(),
        positionSampleRef,
        gpsWeekPtr,
        gpsTimeOfWeekPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 *  Get the position sample's epoch time.
 *
 * @return
 *  - LE_FAULT         Function failed to acquire the epoch time.
 *  - LE_OK            Function succeeded.
 *  - LE_OUT_OF_RANGE  The retrieved time is invalid (all fields are set to 0).
 *
 * @note The epoch time is the number of seconds elapsed since January 1, 1970
 *       (midnight UTC/GMT), not counting leaps seconds.
 *
 * @note If the caller is passing an invalid position sample reference or a null pointer into this
 *       function, it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetEpochTime
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint64_t* millisecondsPtr
        ///< [OUT] Milliseconds since Jan. 1, 1970.
)
{
    return ifgen_le_gnss_GetEpochTime(
        GetCurrentSessionRef(),
        positionSampleRef,
        millisecondsPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's time accurary.
 *
 * @return
 *  - LE_FAULT         Function failed to get the time.
 *  - LE_OUT_OF_RANGE  The retrieved time accuracy is invalid (set to UINT16_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid position sample reference or a null pointer into this
 *       function, it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetTimeAccuracy
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* timeAccuracyPtr
        ///< [OUT] Estimated time accuracy in nanoseconds
)
{
    return ifgen_le_gnss_GetTimeAccuracy(
        GetCurrentSessionRef(),
        positionSampleRef,
        timeAccuracyPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's UTC leap seconds in advance
 *
 * @return
 *  - LE_FAULT         Function failed to get the leap seconds.
 *  - LE_OUT_OF_RANGE  The retrieved time accuracy is invalid (set to UINT8_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note The leap seconds in advance is the accumulated time in seconds since the start of GPS Epoch
 * time (Jan 6, 1980). This value has to be added to the UTC time (since Jan. 1, 1970)
 *
 * @note Insertion of each UTC leap second is usually decided about six months in advance by the
 * International Earth Rotation and Reference Systems Service (IERS).
 *
 * @note If the caller is passing an invalid position sample reference or a null pointer into this
 *       function, it is a fatal error, the function will not return.
 *
 * @deprecated This function is deprecated, le_gnss_GetLeapSeconds should be used instead.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetGpsLeapSeconds
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint8_t* leapSecondsPtr
        ///< [OUT] UTC leap seconds in advance in seconds
)
{
    return ifgen_le_gnss_GetGpsLeapSeconds(
        GetCurrentSessionRef(),
        positionSampleRef,
        leapSecondsPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get leap seconds information.
 *
 * @return
 *  - LE_OK            Function succeeded.
 *  - LE_FAULT         Function failed to get the data.
 *  - LE_TIMEOUT       Timeout occured.
 *  - LE_UNSUPPORTED   Not supported on this platform.
 *
 * @note Insertion of each UTC leap second is usually decided about six months in advance by the
 * International Earth Rotation and Reference Systems Service (IERS).
 *
 * @note If the caller is passing a null pointer into this function, it is considered a fatal
 * error and the function will not return.
 *
 * @note If the return value of a parameter is INT32_MAX/UINT64_MAX, the parameter is not valid.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetLeapSeconds
(
    uint64_t* gpsTimePtr,
        ///< [OUT] The number of milliseconds of GPS time since midnight,
        ///< Jan. 6, 1980.
    int32_t* currentLeapSecondsPtr,
        ///< [OUT] Current UTC leap seconds value in milliseconds.
    uint64_t* changeEventTimePtr,
        ///< [OUT] The number of milliseconds since midnight, Jan. 6, 1980
        ///< to the next leap seconds change event.
    int32_t* nextLeapSecondsPtr
        ///< [OUT] UTC leap seconds value to be applied at the change
        ///< event time in milliseconds.
)
{
    return ifgen_le_gnss_GetLeapSeconds(
        GetCurrentSessionRef(),
        gpsTimePtr,
        currentLeapSecondsPtr,
        changeEventTimePtr,
        nextLeapSecondsPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's date.
 *
 * @return
 *  - LE_FAULT         Function failed to get the date.
 *  - LE_OUT_OF_RANGE  The retrieved date is invalid (all fields are set to 0).
 *  - LE_OK            Function succeeded.
 *
 * @note If the caller is passing an invalid Position sample reference or null pointers into this
 *       function, it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetDate
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* yearPtr,
        ///< [OUT] UTC Year A.D. [e.g. 2014].
    uint16_t* monthPtr,
        ///< [OUT] UTC Month into the year [range 1...12].
    uint16_t* dayPtr
        ///< [OUT] UTC Days into the month [range 1...31].
)
{
    return ifgen_le_gnss_GetDate(
        GetCurrentSessionRef(),
        positionSampleRef,
        yearPtr,
        monthPtr,
        dayPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's horizontal speed.
 *
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  At least one of the retrieved parameters is invalid (set to UINT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note hSpeedPtr, hSpeedAccuracyPtr can be set to NULL if not needed.
 *
 * @note Horizontal speed is in meters/second with 2 decimal places (3047 = 30.47 meters/second).
 *
 * @note Horizontal speed accuracy estimate is default set to meters/second with 1 decimal place
 *       (304 = 30.4 meters/second). To change its accuracy, call the @c le_gnss_SetDataResolution()
 *       function. Horizontal speed accuracy estimate is set as data type and accuracy from 0 to 3
 *       decimal place is set as resolution.
 *
 * @note In case the function returns LE_OUT_OF_RANGE, some of the retrieved parameters may be
 *       valid. Please compare them with UINT32_MAX.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @warning The Horizontal speed accuracy is platform dependent. Please refer to
 *          @ref platformConstraintsGnss_speedAccuracies section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetHorizontalSpeed
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* hspeedPtr,
        ///< [OUT] Horizontal speed in meters/second [resolution 1e-2].
    uint32_t* hspeedAccuracyPtr
        ///< [OUT] Horizontal speed's accuracy estimate in meters/second.
)
{
    return ifgen_le_gnss_GetHorizontalSpeed(
        GetCurrentSessionRef(),
        positionSampleRef,
        hspeedPtr,
        hspeedAccuracyPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's vertical speed.
 *
 * @return
 *  - LE_FAULT         The function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  At least one of the retrieved parameters is not valid (set to INT32_MAX).
 *  - LE_OK            The function succeeded.
 *
 * @note vSpeedPtr, vSpeedAccuracyPtr can be set to NULL if not needed.
 *
 * @note For a 2D position Fix, the vertical speed will be indicated as invalid
 * and set to INT32_MAX.
 *
 * @note Vertical speed accuracy estimate is default set to meters/second with 1 decimal place
 *       (304 = 30.4 meters/second). To change its accuracy, call the @c le_gnss_SetDataResolution()
 *       function. Vertical speed accuracy estimate is set as data type and accuracy from 0 to 3
 *       decimal place is set as resolution.
 *
 * @note In case the function returns LE_OUT_OF_RANGE, some of the retrieved parameters may be
 *       valid. Please compare them with INT32_MAX.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 *
 * @warning The Vertical speed accuracy is platform dependent. Please refer to
 *          @ref platformConstraintsGnss_speedAccuracies section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetVerticalSpeed
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* vspeedPtr,
        ///< [OUT] Vertical speed in meters/second [resolution 1e-2],
        ///< positive up.
    int32_t* vspeedAccuracyPtr
        ///< [OUT] Vertical speed's accuracy estimate in meters/second.
)
{
    return ifgen_le_gnss_GetVerticalSpeed(
        GetCurrentSessionRef(),
        positionSampleRef,
        vspeedPtr,
        vspeedAccuracyPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's direction. Direction of movement is the direction that the vehicle or
 * person is actually moving.
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  At least one of the retrieved parameters is invalid (set to UINT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note Direction and direction accuracy are given in degrees with 1 decimal place: 1755 = 175.5
 *       degrees.
 *       Direction ranges from 0 to 359.9 degrees, where 0 is True North.
 *
 * @note In case the function returns LE_OUT_OF_RANGE, some of the retrieved parameters may be
 *       valid. Please compare them with UINT32_MAX.
 *
 * @note directionPtr, directionAccuracyPtr can be set to NULL if not needed.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetDirection
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint32_t* directionPtr,
        ///< [OUT] Direction in degrees [resolution 1e-1].
        ///< Range: 0 to 359.9, where 0 is True North
    uint32_t* directionAccuracyPtr
        ///< [OUT] Direction's accuracy estimate
        ///< in degrees [resolution 1e-1].
)
{
    return ifgen_le_gnss_GetDirection(
        GetCurrentSessionRef(),
        positionSampleRef,
        directionPtr,
        directionAccuracyPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Satellites Vehicle information.
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  At least one of the retrieved parameters is invalid.
 *  - LE_OK            Function succeeded.
 *
 * @note satId[] can be set to 0 if that information list index is not configured, so
 * all satellite parameters (satConst[], satSnr[],satAzim[], satElev[]) are fixed to 0.
 *
 * @note For LE_OUT_OF_RANGE returned code, invalid value depends on field type:
 * UINT16_MAX for satId, LE_GNSS_SV_CONSTELLATION_UNDEFINED for satConst, false for satUsed,
 * UINT8_MAX for satSnr, UINT16_MAX for satAzim, UINT8_MAX for satElev.
 *
 * @note In case the function returns LE_OUT_OF_RANGE, some of the retrieved parameters may be
 *       valid.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetSatellitesInfo
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* satIdPtr,
        ///< [OUT] Satellites in View ID number, referring
        ///< to NMEA standard.
    size_t* satIdSizePtr,
        ///< [INOUT]
    le_gnss_Constellation_t* satConstPtr,
        ///< [OUT] GNSS constellation type.
    size_t* satConstSizePtr,
        ///< [INOUT]
    bool* satUsedPtr,
        ///< [OUT] TRUE if satellite in View Used
        ///< for Navigation.
    size_t* satUsedSizePtr,
        ///< [INOUT]
    uint8_t* satSnrPtr,
        ///< [OUT] Satellites in View Signal To
        ///< Noise Ratio (C/No) [dBHz].
    size_t* satSnrSizePtr,
        ///< [INOUT]
    uint16_t* satAzimPtr,
        ///< [OUT] Satellites in View Azimuth [degrees].
        ///< Range: 0 to 360
        ///< If Azimuth angle is currently unknown,
        ///< the value is set to UINT16_MAX.
    size_t* satAzimSizePtr,
        ///< [INOUT]
    uint8_t* satElevPtr,
        ///< [OUT] Satellites in View Elevation [degrees].
        ///< Range: 0 to 90
        ///< If Elevation angle is currently unknown,
        ///< the value is set to UINT8_MAX.
    size_t* satElevSizePtr
        ///< [INOUT]
)
{
    return ifgen_le_gnss_GetSatellitesInfo(
        GetCurrentSessionRef(),
        positionSampleRef,
        satIdPtr,
        satIdSizePtr,
        satConstPtr,
        satConstSizePtr,
        satUsedPtr,
        satUsedSizePtr,
        satSnrPtr,
        satSnrSizePtr,
        satAzimPtr,
        satAzimSizePtr,
        satElevPtr,
        satElevSizePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the SBAS constellation category given the SBAS satellite number ID.
 *
 */
//--------------------------------------------------------------------------------------------------
le_gnss_SbasConstellationCategory_t le_gnss_GetSbasConstellationCategory
(
    uint16_t satId
        ///< [IN] SBAS satellite number ID, referring to NMEA standard.
)
{
    return ifgen_le_gnss_GetSbasConstellationCategory(
        GetCurrentSessionRef(),
        satId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the Satellites Vehicle status.
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  At least one of the retrieved parameters is invalid (set to UINT8_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note In case the function returns LE_OUT_OF_RANGE, some of the retrieved parameters may be
 *       valid. Please compare them with UINT8_MAX.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetSatellitesStatus
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint8_t* satsInViewCountPtr,
        ///< [OUT] Number of satellites expected to be in view.
    uint8_t* satsTrackingCountPtr,
        ///< [OUT] Number of satellites in view, when tracking.
    uint8_t* satsUsedCountPtr
        ///< [OUT] Number of satellites in view used for Navigation.
)
{
    return ifgen_le_gnss_GetSatellitesStatus(
        GetCurrentSessionRef(),
        positionSampleRef,
        satsInViewCountPtr,
        satsTrackingCountPtr,
        satsUsedCountPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the DOP parameters (Dilution Of Precision) for the fixed position
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  At least one of the retrieved parameters is invalid (set to UINT16_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @deprecated This function is deprecated, le_gnss_GetDilutionOfPrecision() should be used for
 *             new code.
 *
 * @note The DOP values are given with 3 decimal places like: DOP value 2200 = 2.200
 *
 * @note In case the function returns LE_OUT_OF_RANGE, some of the retrieved parameters may be
 *       valid. Please compare them with UINT16_MAX.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetDop
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    uint16_t* hdopPtr,
        ///< [OUT] Horizontal Dilution of Precision [resolution 1e-3].
    uint16_t* vdopPtr,
        ///< [OUT] Vertical Dilution of Precision [resolution 1e-3].
    uint16_t* pdopPtr
        ///< [OUT] Position Dilution of Precision [resolution 1e-3].
)
{
    return ifgen_le_gnss_GetDop(
        GetCurrentSessionRef(),
        positionSampleRef,
        hdopPtr,
        vdopPtr,
        pdopPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the DOP parameter (Dilution Of Precision) for the fixed position.
 *
 * @return
 *  - LE_FAULT         Function failed to find the DOP value.
 *  - LE_OUT_OF_RANGE  The retrieved parameter is invalid (set to UINT16_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note This function replaces the deprecated function le_gnss_GetDop().
 *
 * @note The DOP value is given with 3 decimal places by default like: DOP value 2200 = 2.200
 *       The resolution can be modified by calling the @c le_gnss_SetDopResolution() function.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetDilutionOfPrecision
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    le_gnss_DopType_t dopType,
        ///< [IN] Dilution of Precision type.
    uint16_t* dopPtr
        ///< [OUT] Dilution of Precision corresponding to the dopType
)
{
    return ifgen_le_gnss_GetDilutionOfPrecision(
        GetCurrentSessionRef(),
        positionSampleRef,
        dopType,
        dopPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's altitude with respect to the WGS-84 ellipsoid
 *
 * @return
 *  - LE_FAULT         Function failed to get the altitude.
 *  - LE_OUT_OF_RANGE  The altitudeOnWgs84 is invalid (set to INT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note altitudeOnWgs84 is in meters, with respect to the WGS-84 ellipsoid with 3 decimal
 *       places (3047 = 3.047 meters).
 *
 * @note For a 2D position fix, the altitude with respect to the WGS-84 ellipsoid will be indicated
 *       as invalid and set to INT32_MAX.
 *
 * @note If the caller is passing an invalid Position reference or a null pointer into this
 *       function, it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetAltitudeOnWgs84
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* altitudeOnWgs84Ptr
        ///< [OUT] Altitude in meters, between WGS-84 earth ellipsoid
        ///< and mean sea level [resolution 1e-3].
)
{
    return ifgen_le_gnss_GetAltitudeOnWgs84(
        GetCurrentSessionRef(),
        positionSampleRef,
        altitudeOnWgs84Ptr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the position sample's magnetic deviation. It is the difference between the bearing to
 * true north and the bearing shown on a magnetic compass. The deviation is positive when the
 * magnetic north is east of true north.
 *
 * @return
 *  - LE_FAULT         Function failed to find the positionSample.
 *  - LE_OUT_OF_RANGE  The magneticDeviation is invalid (set to INT32_MAX).
 *  - LE_OK            Function succeeded.
 *
 * @note magneticDeviation is in degrees, with 1 decimal places (47 = 4.7 degree).
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetMagneticDeviation
(
    le_gnss_SampleRef_t positionSampleRef,
        ///< [IN] Position sample's reference.
    int32_t* magneticDeviationPtr
        ///< [OUT] MagneticDeviation in degrees [resolution 1e-1].
)
{
    return ifgen_le_gnss_GetMagneticDeviation(
        GetCurrentSessionRef(),
        positionSampleRef,
        magneticDeviationPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the last updated position sample object reference.
 *
 * @return A reference to last Position's sample.
 *
 * @note
 *      On failure, the process exits, so you don't have to worry about checking the returned
 *      reference for validity.
 */
//--------------------------------------------------------------------------------------------------
le_gnss_SampleRef_t le_gnss_GetLastSampleRef
(
    void
)
{
    return ifgen_le_gnss_GetLastSampleRef(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to release the position sample.
 *
 * @note If the caller is passing an invalid Position sample reference into this function,
 *       it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
void le_gnss_ReleaseSampleRef
(
    le_gnss_SampleRef_t positionSampleRef
        ///< [IN] Position sample's reference.
)
{
     ifgen_le_gnss_ReleaseSampleRef(
        GetCurrentSessionRef(),
        positionSampleRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the SUPL Assisted-GNSS mode.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED request not supported
 *  - LE_TIMEOUT a time-out occurred
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetSuplAssistedMode
(
    le_gnss_AssistedMode_t assistedMode
        ///< [IN] Assisted-GNSS mode.
)
{
    return ifgen_le_gnss_SetSuplAssistedMode(
        GetCurrentSessionRef(),
        assistedMode
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the SUPL Assisted-GNSS mode.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *
 * @note If the caller is passing a null pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetSuplAssistedMode
(
    le_gnss_AssistedMode_t* assistedModePtr
        ///< [OUT] Assisted-GNSS mode.
)
{
    return ifgen_le_gnss_GetSuplAssistedMode(
        GetCurrentSessionRef(),
        assistedModePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the SUPL server URL.
 * That server URL is a NULL-terminated string with a maximum string length (including NULL
 * terminator) equal to 256. Optionally the port number is specified after a colon.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_BUSY service is busy
 *  - LE_TIMEOUT a time-out occurred
 *
 * @note If the SUPL server URL size is bigger than the maximum string length (including NULL
 * terminator) size, it is a fatal error, the function will not return.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetSuplServerUrl
(
    const char* LE_NONNULL suplServerUrl
        ///< [IN] SUPL server URL.
)
{
    return ifgen_le_gnss_SetSuplServerUrl(
        GetCurrentSessionRef(),
        suplServerUrl
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function injects the SUPL certificate to be used in A-GNSS sessions. Certificates must
 * be encoded in DER. Other certificate encryptions (e.g., PEM, CER and CRT)
 * aren't supported.
 *
 * @return
 *  - LE_OK on success
 *  - LE_BAD_PARAMETER on invalid parameter
 *  - LE_FAULT on failure
 *  - LE_BUSY service is busy
 *  - LE_TIMEOUT a time-out occurred
 *
 * @note If the SUPL certificate size is bigger than the Maximum SUPL certificate size,
 * it is a fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_InjectSuplCertificate
(
    uint8_t suplCertificateId,
        ///< [IN] ID of the SUPL certificate.
        ///< Certificate ID range is 0 to 9
    uint16_t suplCertificateLen,
        ///< [IN] SUPL certificate size in Bytes.
    const char* LE_NONNULL suplCertificate
        ///< [IN] SUPL certificate contents.
)
{
    return ifgen_le_gnss_InjectSuplCertificate(
        GetCurrentSessionRef(),
        suplCertificateId,
        suplCertificateLen,
        suplCertificate
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function deletes the SUPL certificate.
 *
 * @return
 *  - LE_OK on success
 *  - LE_BAD_PARAMETER on invalid parameter
 *  - LE_FAULT on failure
 *  - LE_BUSY service is busy
 *  - LE_TIMEOUT a time-out occurred
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_DeleteSuplCertificate
(
    uint8_t suplCertificateId
        ///< [IN] ID of the SUPL certificate.
        ///< Certificate ID range is 0 to 9
)
{
    return ifgen_le_gnss_DeleteSuplCertificate(
        GetCurrentSessionRef(),
        suplCertificateId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the enabled NMEA sentences using a bit mask.
 *
 * @return
 *  - LE_OK             Success
 *  - LE_BAD_PARAMETER  Bit mask exceeds the maximal value
 *  - LE_FAULT          Failure
 *  - LE_BUSY           Service is busy
 *  - LE_TIMEOUT        Timeout occurred
 *  - LE_NOT_PERMITTED  GNSS device is not in "ready" state
 *
 * @warning This function may be subject to limitations depending on the platform. Please refer to
 *          the @ref platformConstraintsGnss page.
 *
 * @note Some NMEA sentences are unsupported depending on the plateform. Please refer to
 *       @ref platformConstraintsGnss_nmeaMask section for full details. Setting an unsuported NMEA
 *       sentence won't report an error.
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 *
 * @deprecated LE_GNSS_NMEA_MASK_PQXFI is deprecated. LE_GNSS_NMEA_MASK_PTYPE should be used
 *             instead. Setting LE_GNSS_NMEA_MASK_PTYPE will also set LE_GNSS_NMEA_MASK_PQXFI.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetNmeaSentences
(
    le_gnss_NmeaBitMask_t nmeaMask
        ///< [IN] Bit mask for enabled NMEA sentences.
)
{
    return ifgen_le_gnss_SetNmeaSentences(
        GetCurrentSessionRef(),
        nmeaMask
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the bit mask for the enabled NMEA sentences.
 *
 * @return
 *  - LE_OK             Success
 *  - LE_FAULT          Failure
 *  - LE_BUSY           Service is busy
 *  - LE_TIMEOUT        Timeout occurred
 *  - LE_NOT_PERMITTED  GNSS device is not in "ready" state
 *
 * @note If the caller is passing a null pointer to this function, it is a fatal error, the
 *       function will not return.
 *
 * @note Some NMEA sentences are unsupported depending on the plateform. Please refer to
 *       @ref platformConstraintsGnss_nmeaMask section for full details. The bit mask for an unset
 *       or unsupported NMEA sentence is zero.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetNmeaSentences
(
    le_gnss_NmeaBitMask_t* nmeaMaskPtrPtr
        ///< [OUT] Bit mask for enabled NMEA sentences.
)
{
    return ifgen_le_gnss_GetNmeaSentences(
        GetCurrentSessionRef(),
        nmeaMaskPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function returns the status of the GNSS device.
 *
 */
//--------------------------------------------------------------------------------------------------
le_gnss_State_t le_gnss_GetState
(
    void
)
{
    return ifgen_le_gnss_GetState(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function sets the GNSS minimum elevation.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_OUT_OF_RANGE if the minimum elevation is above range
 *  - LE_UNSUPPORTED request not supported
 *
 * @warning The settings are platform dependent. Please refer to
 *          @ref platformConstraintsGnss_SettingConfiguration section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetMinElevation
(
    uint8_t minElevation
        ///< [IN] Minimum elevation in degrees [range 0..90].
)
{
    return ifgen_le_gnss_SetMinElevation(
        GetCurrentSessionRef(),
        minElevation
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the GNSS minimum elevation.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED request not supported
 *
 * @note If the caller is passing n null pointer to this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_GetMinElevation
(
    uint8_t* minElevationPtrPtr
        ///< [OUT] Minimum elevation in degrees [range 0..90].
)
{
    return ifgen_le_gnss_GetMinElevation(
        GetCurrentSessionRef(),
        minElevationPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the resolution for the DOP parameters
 *
 * @return LE_OK               Function succeeded.
 * @return LE_BAD_PARAMETER    Invalid parameter provided.
 * @return LE_FAULT            Function failed.
 *
 * @note The function sets the same resolution to all DOP values returned by
 *       le_gnss_GetDilutionOfPrecision() API. The resolution setting takes effect immediately.
 *
 * @note The resolution setting is done per client session.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetDopResolution
(
    le_gnss_Resolution_t resolution
        ///< [IN] Resolution.
)
{
    return ifgen_le_gnss_SetDopResolution(
        GetCurrentSessionRef(),
        resolution
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the resolution for the specific type of data
 *
 * @return LE_OK               Function succeeded.
 * @return LE_BAD_PARAMETER    Invalid parameter provided.
 * @return LE_FAULT            Function failed.
 *
 * @note The resolution setting takes effect immediately and is not persistent to reset.
 *
 * @note The resolution setting is done per client session.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_SetDataResolution
(
    le_gnss_DataType_t dataType,
        ///< [IN] Data type.
    le_gnss_Resolution_t resolution
        ///< [IN] Resolution.
)
{
    return ifgen_le_gnss_SetDataResolution(
        GetCurrentSessionRef(),
        dataType,
        resolution
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function converts a location data parameter from/to multi-coordinate system
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_BAD_PARAMETER Invalid parameter provided.
 *  - LE_UNSUPPORTED request not supported
 *
 * @note The resolution of location data parameter remains unchanged after the conversion.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gnss_ConvertDataCoordinateSystem
(
    le_gnss_CoordinateSystem_t coordinateSrc,
        ///< [IN] Coordinate system to convert from.
    le_gnss_CoordinateSystem_t coordinateDst,
        ///< [IN] Coordinate system to convert to.
    le_gnss_LocationDataType_t locationDataType,
        ///< [IN] Type of location data to convert.
    int64_t locationDataSrc,
        ///< [IN] Data to convert.
    int64_t* locationDataDstPtr
        ///< [OUT] Converted Data.
)
{
    return ifgen_le_gnss_ConvertDataCoordinateSystem(
        GetCurrentSessionRef(),
        coordinateSrc,
        coordinateDst,
        locationDataType,
        locationDataSrc,
        locationDataDstPtr
    );
}