/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "admin_interface.h"
#include "admin_messages.h"
#include "admin_service.h"


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
    admin_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for client threads.
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(admin_ClientThreadData,
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
    le_result_t result = ifgen_admin_OpenSession(sessionRef, isBlocking);
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
    if (ifgen_admin_HasLocalBinding())
    {
        return NULL;
    }
    else
    {
        _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

        // If the thread specific data is NULL, then the session ref has not been created.
        LE_FATAL_IF(clientThreadPtr==NULL,
                    "admin_ConnectService() not called for current thread");

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
    ifgen_admin_InitCommonData();

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_InitStaticPool(admin_ClientThreadData,
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
void admin_ConnectService
(
    void
)
{
    if (!ifgen_admin_HasLocalBinding())
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
le_result_t admin_TryConnectService
(
    void
)
{
    if (ifgen_admin_HasLocalBinding())
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

    LE_FATAL("Component for admin disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetServerDisconnectHandler
(
    admin_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    if (ifgen_admin_HasLocalBinding())
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
void admin_DisconnectService
(
    void
)
{
    if (ifgen_admin_HasLocalBinding())
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
 * Push a trigger type data sample to a resource.
 *
 * @note If the resource doesn't exist, the push will be ignored.  This will not cause a
 *       Placeholder resource to be created.
 */
//--------------------------------------------------------------------------------------------------
void admin_PushTrigger
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute resource tree path.
    double timestamp
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< Zero = now (i.e., generate a timestamp for me).
)
{
     ifgen_admin_PushTrigger(
        GetCurrentSessionRef(),
        path,
        timestamp
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Push a Boolean type data sample to a resource.
 *
 * @note If the resource doesn't exist, the push will be ignored.  This will not cause a
 *       Placeholder resource to be created.
 */
//--------------------------------------------------------------------------------------------------
void admin_PushBoolean
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute resource tree path.
    double timestamp,
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< Zero = now (i.e., generate a timestamp for me).
    bool value
        ///< [IN]
)
{
     ifgen_admin_PushBoolean(
        GetCurrentSessionRef(),
        path,
        timestamp,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Push a numeric type data sample to a resource.
 *
 * @note If the resource doesn't exist, the push will be ignored.  This will not cause a
 *       Placeholder resource to be created.
 */
//--------------------------------------------------------------------------------------------------
void admin_PushNumeric
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute resource tree path.
    double timestamp,
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< Zero = now (i.e., generate a timestamp for me).
    double value
        ///< [IN]
)
{
     ifgen_admin_PushNumeric(
        GetCurrentSessionRef(),
        path,
        timestamp,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Push a string type data sample to a resource.
 *
 * @note If the resource doesn't exist, the push will be ignored.  This will not cause a
 *       Placeholder resource to be created.
 */
//--------------------------------------------------------------------------------------------------
void admin_PushString
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute resource tree path.
    double timestamp,
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< Zero = now (i.e., generate a timestamp for me).
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_admin_PushString(
        GetCurrentSessionRef(),
        path,
        timestamp,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Push a JSON data sample to a resource.
 *
 * @note If the resource doesn't exist, the push will be ignored.  This will not cause a
 *       Placeholder resource to be created.
 */
//--------------------------------------------------------------------------------------------------
void admin_PushJson
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute resource tree path.
    double timestamp,
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< Zero = now (i.e., generate a timestamp for me).
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_admin_PushJson(
        GetCurrentSessionRef(),
        path,
        timestamp,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'admin_TriggerPush'
 */
//--------------------------------------------------------------------------------------------------
admin_TriggerPushHandlerRef_t admin_AddTriggerPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of resource.
    admin_TriggerPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_admin_AddTriggerPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'admin_TriggerPush'
 */
//--------------------------------------------------------------------------------------------------
void admin_RemoveTriggerPushHandler
(
    admin_TriggerPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_admin_RemoveTriggerPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'admin_BooleanPush'
 */
//--------------------------------------------------------------------------------------------------
admin_BooleanPushHandlerRef_t admin_AddBooleanPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of resource.
    admin_BooleanPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_admin_AddBooleanPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'admin_BooleanPush'
 */
//--------------------------------------------------------------------------------------------------
void admin_RemoveBooleanPushHandler
(
    admin_BooleanPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_admin_RemoveBooleanPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'admin_NumericPush'
 */
//--------------------------------------------------------------------------------------------------
admin_NumericPushHandlerRef_t admin_AddNumericPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of resource.
    admin_NumericPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_admin_AddNumericPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'admin_NumericPush'
 */
//--------------------------------------------------------------------------------------------------
void admin_RemoveNumericPushHandler
(
    admin_NumericPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_admin_RemoveNumericPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'admin_StringPush'
 */
//--------------------------------------------------------------------------------------------------
admin_StringPushHandlerRef_t admin_AddStringPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of resource.
    admin_StringPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_admin_AddStringPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'admin_StringPush'
 */
//--------------------------------------------------------------------------------------------------
void admin_RemoveStringPushHandler
(
    admin_StringPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_admin_RemoveStringPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'admin_JsonPush'
 */
//--------------------------------------------------------------------------------------------------
admin_JsonPushHandlerRef_t admin_AddJsonPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of resource.
    admin_JsonPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_admin_AddJsonPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'admin_JsonPush'
 */
//--------------------------------------------------------------------------------------------------
void admin_RemoveJsonPushHandler
(
    admin_JsonPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_admin_RemoveJsonPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Creates a data flow route from one resource to another by setting the data source for the
 * destination resource.  If the destination resource already has a source resource, it will be
 * replaced. Does nothing if the route already exists.
 *
 * Creates Placeholders for any source or destination resource that doesn't yet exist in the
 * resource tree.
 *
 * @note While an Input can have a source configured, it will ignore anything pushed to it
 *       from other resources via that route. Inputs only accept values pushed by the app that
 *       created them or from the administrator pushed directly to them via one of the
 *       @ref c_dataHubAdmin_Pushing "Push functions".
 *
 * @return
 *  - LE_OK if route already existed or new route was successfully created.
 *  - LE_BAD_PARAMETER if one of the paths is invalid.
 *  - LE_DUPLICATE if the addition of this route would result in a loop.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_SetSource
(
    const char* LE_NONNULL destPath,
        ///< [IN] Absolute path of destination resource.
    const char* LE_NONNULL srcPath
        ///< [IN] Absolute path of source resource.
)
{
    return ifgen_admin_SetSource(
        GetCurrentSessionRef(),
        destPath,
        srcPath
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Fetches the data flow source resource from which a given resource expects to receive data
 * samples.
 *
 * @note While an Input can have a source configured, it will ignore anything pushed to it
 *       from other resources via that route. Inputs only accept values pushed by the app that
 *       created them or from the administrator pushed directly to them via one of the
 *       @ref c_dataHubAdmin_Pushing "Push functions".
 *
 * @return
 *  - LE_OK if successful.
 *  - LE_BAD_PARAMETER if the path is invalid.
 *  - LE_NOT_FOUND if the resource doesn't exist or doesn't have a source.
 *  - LE_OVERFLOW if the path of the source resource won't fit in the string buffer provided.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_GetSource
(
    const char* LE_NONNULL destPath,
        ///< [IN] Absolute path of destination resource.
    char* srcPath,
        ///< [OUT] Absolute path of source resource.
    size_t srcPathSize
        ///< [IN]
)
{
    return ifgen_admin_GetSource(
        GetCurrentSessionRef(),
        destPath,
        srcPath,
        srcPathSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove the data flow route into a resource.
 */
//--------------------------------------------------------------------------------------------------
void admin_RemoveSource
(
    const char* LE_NONNULL destPath
        ///< [IN] Absolute path of destination resource.
)
{
     ifgen_admin_RemoveSource(
        GetCurrentSessionRef(),
        destPath
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Create an Observation in the /obs/ namespace.
 *
 *  @return
 *  - LE_OK if the observation was created or it already existed.
 *  - LE_BAD_PARAMETER if the path is invalid.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_CreateObs
(
    const char* LE_NONNULL path
        ///< [IN] Path within the /obs/ namespace.
)
{
    return ifgen_admin_CreateObs(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Delete an Observation in the /obs/ namespace.
 */
//--------------------------------------------------------------------------------------------------
void admin_DeleteObs
(
    const char* LE_NONNULL path
        ///< [IN] Path within the /obs/ namespace.
)
{
     ifgen_admin_DeleteObs(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the minimum period between data samples accepted by a given Observation.
 *
 * This is used to throttle the rate of data passing into and through an Observation.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetMinPeriod
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the /obs/ namespace.
    double minPeriod
        ///< [IN] The minimum period, in seconds.
)
{
     ifgen_admin_SetMinPeriod(
        GetCurrentSessionRef(),
        path,
        minPeriod
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the minimum period between data samples accepted by a given Observation.
 *
 * @return The value, or NAN (not a number) if not set.
 */
//--------------------------------------------------------------------------------------------------
double admin_GetMinPeriod
(
    const char* LE_NONNULL path
        ///< [IN] Path within the /obs/ namespace.
)
{
    return ifgen_admin_GetMinPeriod(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the highest value in a range that will be accepted by a given Observation.
 *
 * Ignored for all non-numeric types except Boolean for which non-zero = true and zero = false.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetHighLimit
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the /obs/ namespace.
    double highLimit
        ///< [IN] The highest value in the range, or NAN (not a number) to remove limit.
)
{
     ifgen_admin_SetHighLimit(
        GetCurrentSessionRef(),
        path,
        highLimit
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the highest value in a range that will be accepted by a given Observation.
 *
 * @return The value, or NAN (not a number) if not set.
 */
//--------------------------------------------------------------------------------------------------
double admin_GetHighLimit
(
    const char* LE_NONNULL path
        ///< [IN] Path within the /obs/ namespace.
)
{
    return ifgen_admin_GetHighLimit(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the lowest value in a range that will be accepted by a given Observation.
 *
 * Ignored for all non-numeric types except Boolean for which non-zero = true and zero = false.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetLowLimit
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the /obs/ namespace.
    double lowLimit
        ///< [IN] The lowest value in the range, or NAN (not a number) to remove limit.
)
{
     ifgen_admin_SetLowLimit(
        GetCurrentSessionRef(),
        path,
        lowLimit
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the lowest value in a range that will be accepted by a given Observation.
 *
 * @return The value, or NAN (not a number) if not set.
 */
//--------------------------------------------------------------------------------------------------
double admin_GetLowLimit
(
    const char* LE_NONNULL path
        ///< [IN] Path within the /obs/ namespace.
)
{
    return ifgen_admin_GetLowLimit(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the magnitude that a new value must vary from the current value to be accepted by
 * a given Observation.
 *
 * Ignored for trigger types.
 *
 * For all other types, any non-zero value means accept any change, but drop if the same as current.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetChangeBy
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the /obs/ namespace.
    double change
        ///< [IN] The magnitude, or either zero or NAN (not a number) to remove limit.
)
{
     ifgen_admin_SetChangeBy(
        GetCurrentSessionRef(),
        path,
        change
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the magnitude that a new value must vary from the current value to be accepted by
 * a given Observation.
 *
 * @return The value, or NAN (not a number) if not set.
 */
//--------------------------------------------------------------------------------------------------
double admin_GetChangeBy
(
    const char* LE_NONNULL path
        ///< [IN] Path within the /obs/ namespace.
)
{
    return ifgen_admin_GetChangeBy(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Perform a transform on an observation's buffered data. Value of the observation will be
 * the output of the transform
 *
 * Ignored for all non-numeric types except Boolean for which non-zero = true and zero = false.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetTransform
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the /obs/ namespace.
    admin_TransformType_t transformType,
        ///< [IN] Type of transform to apply
    const double* paramsPtr,
        ///< [IN] Optional parameter list
    size_t paramsSize
        ///< [IN]
)
{
     ifgen_admin_SetTransform(
        GetCurrentSessionRef(),
        path,
        transformType,
        paramsPtr,
        paramsSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the type of transform currently applied to an Observation.
 *
 * @return The TransformType
 */
//--------------------------------------------------------------------------------------------------
admin_TransformType_t admin_GetTransform
(
    const char* LE_NONNULL path
        ///< [IN] Path within the /obs/ namespace.
)
{
    return ifgen_admin_GetTransform(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the JSON member/element specifier for extraction of data from within a structured JSON
 * value received by a given Observation.
 *
 * If this is set, all non-JSON data will be ignored, and all JSON data that does not contain the
 * the specified object member or array element will also be ignored.
 *
 * To clear, set to an empty string.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetJsonExtraction
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the /obs/ namespace.
    const char* LE_NONNULL extractionSpec
        ///< [IN] str specifying member/element to extract.
)
{
     ifgen_admin_SetJsonExtraction(
        GetCurrentSessionRef(),
        path,
        extractionSpec
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the JSON member/element specifier for extraction of data from within a structured JSON
 * value received by a given Observation.
 *
 * @return
 *  - LE_OK if successful
 *  - LE_NOT_FOUND if the resource doesn't exist or doesn't have a JSON extraction specifier set.
 *  - LE_OVERFLOW if the JSON extraction specifier won't fit in the string buffer provided.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_GetJsonExtraction
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the /obs/ namespace.
    char* result,
        ///< [OUT] Buffer where result goes if LE_OK returned.
    size_t resultSize
        ///< [IN]
)
{
    return ifgen_admin_GetJsonExtraction(
        GetCurrentSessionRef(),
        path,
        result,
        resultSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the maximum number of data samples to buffer in a given Observation.  Buffers are FIFO
 * circular buffers. When full, the buffer drops the oldest value to make room for a new addition.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetBufferMaxCount
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the /obs/ namespace.
    uint32_t count
        ///< [IN] The number of samples to buffer (0 = remove setting).
)
{
     ifgen_admin_SetBufferMaxCount(
        GetCurrentSessionRef(),
        path,
        count
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the buffer size setting for a given Observation.
 *
 * @return The buffer size (in number of samples) or 0 if not set or the Observation does not exist.
 */
//--------------------------------------------------------------------------------------------------
uint32_t admin_GetBufferMaxCount
(
    const char* LE_NONNULL path
        ///< [IN] Path within the /obs/ namespace.
)
{
    return ifgen_admin_GetBufferMaxCount(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the minimum time between backups of an Observation's buffer to non-volatile storage.
 * If the buffer's size is non-zero and the backup period is non-zero, then the buffer will be
 * backed-up to non-volatile storage when it changes, but never more often than this period setting
 * specifies.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetBufferBackupPeriod
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the /obs/ namespace.
    uint32_t seconds
        ///< [IN] The minimum number of seconds between backups (0 = disable backups)
)
{
     ifgen_admin_SetBufferBackupPeriod(
        GetCurrentSessionRef(),
        path,
        seconds
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the minimum time between backups of an Observation's buffer to non-volatile storage.
 * See admin_SetBufferBackupPeriod() for more information.
 *
 * @return The buffer backup period (in seconds) or 0 if backups are disabled or the Observation
 *         does not exist.
 */
//--------------------------------------------------------------------------------------------------
uint32_t admin_GetBufferBackupPeriod
(
    const char* LE_NONNULL path
        ///< [IN] Path within the /obs/ namespace.
)
{
    return ifgen_admin_GetBufferBackupPeriod(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the default value of a resource to a Boolean value.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetBooleanDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    bool value
        ///< [IN]
)
{
     ifgen_admin_SetBooleanDefault(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the default value of a resource to a numeric value.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetNumericDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    double value
        ///< [IN]
)
{
     ifgen_admin_SetNumericDefault(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the default value of a resource to a string value.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetStringDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_admin_SetStringDefault(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the default value of a resource to a JSON value.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetJsonDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_admin_SetJsonDefault(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Discover whether a given resource has a default value.
 *
 * @return true if there is a default value set, false if not.
 */
//--------------------------------------------------------------------------------------------------
bool admin_HasDefault
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_HasDefault(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the data type of the default value that is currently set on a given resource.
 *
 * @return The data type, or IO_DATA_TYPE_TRIGGER if not set.
 */
//--------------------------------------------------------------------------------------------------
io_DataType_t admin_GetDefaultDataType
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_GetDefaultDataType(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the default value of a resource, if it is Boolean.
 *
 * @return the default value, or false if not set or set to another data type.
 */
//--------------------------------------------------------------------------------------------------
bool admin_GetBooleanDefault
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_GetBooleanDefault(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the default value, if it is numeric.
 *
 * @return the default value, or NAN (not a number) if not set or set to another data type.
 */
//--------------------------------------------------------------------------------------------------
double admin_GetNumericDefault
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_GetNumericDefault(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the default value, if it is a string.
 *
 * @return
 *  - LE_OK if successful,
 *  - LE_OVERFLOW if the buffer provided is too small to hold the value.
 *  - LE_NOT_FOUND if the resource doesn't have a string default value set.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_GetStringDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    char* value,
        ///< [OUT]
    size_t valueSize
        ///< [IN]
)
{
    return ifgen_admin_GetStringDefault(
        GetCurrentSessionRef(),
        path,
        value,
        valueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the default value, in JSON format.
 *
 * @note This works for any type of default value.
 *
 * @return
 *  - LE_OK if successful,
 *  - LE_OVERFLOW if the buffer provided is too small to hold the value.
 *  - LE_NOT_FOUND if the resource doesn't have a default value set.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_GetJsonDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    char* value,
        ///< [OUT]
    size_t valueSize
        ///< [IN]
)
{
    return ifgen_admin_GetJsonDefault(
        GetCurrentSessionRef(),
        path,
        value,
        valueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove any default value on a given resource.
 */
//--------------------------------------------------------------------------------------------------
void admin_RemoveDefault
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
     ifgen_admin_RemoveDefault(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set an override of Boolean type on a given resource.
 *
 * @note Override will be ignored by an Input or Output resource if the override's data type
 *       does not match the data type of the Input or Output.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetBooleanOverride
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    bool value
        ///< [IN]
)
{
     ifgen_admin_SetBooleanOverride(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set an override of numeric type on a given resource.
 *
 * @note Override will be ignored by an Input or Output resource if the override's data type
 *       does not match the data type of the Input or Output.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetNumericOverride
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    double value
        ///< [IN]
)
{
     ifgen_admin_SetNumericOverride(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set an override of string type on a given resource.
 *
 * @note Override will be ignored by an Input or Output resource if the override's data type
 *       does not match the data type of the Input or Output.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetStringOverride
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_admin_SetStringOverride(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set an override of JSON type on a given resource.
 *
 * @note Override will be ignored by an Input or Output resource if the override's data type
 *       does not match the data type of the Input or Output.
 */
//--------------------------------------------------------------------------------------------------
void admin_SetJsonOverride
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_admin_SetJsonOverride(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Find out whether the resource currently has an override set.
 *
 * @return true if the resource has an override, false otherwise.
 *
 * @note It's possible for an Input or Output to have an override set, but not be overridden.
 *       This is because setting an override to a data type that does not match the Input or
 *       Output resource's data type will result in the override being ignored.  Observations
 *       (and Placeholders) have flexible data types, so if they have an override set, they will
 *       definitely be overridden.
 */
//--------------------------------------------------------------------------------------------------
bool admin_HasOverride
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_HasOverride(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the data type of the override value that is currently set on a given resource.
 *
 * @return The data type, or IO_DATA_TYPE_TRIGGER if not set.
 */
//--------------------------------------------------------------------------------------------------
io_DataType_t admin_GetOverrideDataType
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_GetOverrideDataType(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the override value of a resource, if it is Boolean.
 *
 * @return the override value, or false if not set or set to another data type.
 */
//--------------------------------------------------------------------------------------------------
bool admin_GetBooleanOverride
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_GetBooleanOverride(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the override value, if it is numeric.
 *
 * @return the override value, or NAN (not a number) if not set or set to another data type.
 */
//--------------------------------------------------------------------------------------------------
double admin_GetNumericOverride
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_GetNumericOverride(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the override value, if it is a string.
 *
 * @return
 *  - LE_OK if successful,
 *  - LE_OVERFLOW if the buffer provided is too small to hold the value.
 *  - LE_NOT_FOUND if the resource doesn't have a string override value set.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_GetStringOverride
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    char* value,
        ///< [OUT]
    size_t valueSize
        ///< [IN]
)
{
    return ifgen_admin_GetStringOverride(
        GetCurrentSessionRef(),
        path,
        value,
        valueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the override value, in JSON format.
 *
 * @note This works for any type of override value.
 *
 * @return
 *  - LE_OK if successful,
 *  - LE_OVERFLOW if the buffer provided is too small to hold the value.
 *  - LE_NOT_FOUND if the resource doesn't have an override value set.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_GetJsonOverride
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    char* value,
        ///< [OUT]
    size_t valueSize
        ///< [IN]
)
{
    return ifgen_admin_GetJsonOverride(
        GetCurrentSessionRef(),
        path,
        value,
        valueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove any override on a given resource.
 */
//--------------------------------------------------------------------------------------------------
void admin_RemoveOverride
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
     ifgen_admin_RemoveOverride(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the name of the first child entry under a given parent entry in the resource tree.
 *
 * @return
 *  - LE_OK if successful
 *  - LE_OVERFLOW if the buffer provided is too small to hold the child's path.
 *  - LE_NOT_FOUND if the resource doesn't have any children.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_GetFirstChild
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    char* child,
        ///< [OUT] Absolute path of the first child resource.
    size_t childSize
        ///< [IN]
)
{
    return ifgen_admin_GetFirstChild(
        GetCurrentSessionRef(),
        path,
        child,
        childSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the name of the next child entry under the same parent as a given entry in the resource tree.
 *
 * @return
 *  - LE_OK if successful
 *  - LE_OVERFLOW if the buffer provided is too small to hold the next sibling's path.
 *  - LE_NOT_FOUND if the resource is the last child in its parent's list of children.
 */
//--------------------------------------------------------------------------------------------------
le_result_t admin_GetNextSibling
(
    const char* LE_NONNULL path,
        ///< [IN] Absolute path of the resource.
    char* sibling,
        ///< [OUT] Absolute path of the next sibling resource.
    size_t siblingSize
        ///< [IN]
)
{
    return ifgen_admin_GetNextSibling(
        GetCurrentSessionRef(),
        path,
        sibling,
        siblingSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Find out what type of entry lives at a given path in the resource tree.
 *
 * @return The entry type. ADMIN_ENTRY_TYPE_NONE if there's no entry at the given path.
 */
//--------------------------------------------------------------------------------------------------
admin_EntryType_t admin_GetEntryType
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_GetEntryType(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Check if a given resource is a mandatory output.  If so, it means that this is an output resource
 * that must have a value before the related app function will begin working.
 *
 * @return true if a mandatory output, false if it's an optional output or not an output at all.
 */
//--------------------------------------------------------------------------------------------------
bool admin_IsMandatory
(
    const char* LE_NONNULL path
        ///< [IN] Absolute path of the resource.
)
{
    return ifgen_admin_IsMandatory(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'admin_ResourceTreeChange'
 */
//--------------------------------------------------------------------------------------------------
admin_ResourceTreeChangeHandlerRef_t admin_AddResourceTreeChangeHandler
(
    admin_ResourceTreeChangeHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_admin_AddResourceTreeChangeHandler(
        GetCurrentSessionRef(),
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'admin_ResourceTreeChange'
 */
//--------------------------------------------------------------------------------------------------
void admin_RemoveResourceTreeChangeHandler
(
    admin_ResourceTreeChangeHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_admin_RemoveResourceTreeChangeHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Signal to the Data Hub that administrative changes are about to be performed.
 *
 * This will result in call-backs to any handlers registered using io_AddUpdateStartEndHandler().
 */
//--------------------------------------------------------------------------------------------------
void admin_StartUpdate
(
    void
)
{
     ifgen_admin_StartUpdate(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Signal to the Data Hub that all pending administrative changes have been applied and that
 * normal operation may resume.
 *
 * This may trigger clean-up actions, such as deleting non-volatile backups of any Observations
 * that do not exist at the time this function is called.
 *
 * This will also result in call-backs to any handlers registered using
 * io_AddUpdateStartEndHandler().
 */
//--------------------------------------------------------------------------------------------------
void admin_EndUpdate
(
    void
)
{
     ifgen_admin_EndUpdate(
        GetCurrentSessionRef()
    );
}