/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "io_interface.h"
#include "io_messages.h"
#include "io_service.h"


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
    io_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for client threads.
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(io_ClientThreadData,
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
    le_result_t result = ifgen_io_OpenSession(sessionRef, isBlocking);
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
    if (ifgen_io_HasLocalBinding())
    {
        return NULL;
    }
    else
    {
        _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

        // If the thread specific data is NULL, then the session ref has not been created.
        LE_FATAL_IF(clientThreadPtr==NULL,
                    "io_ConnectService() not called for current thread");

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
    ifgen_io_InitCommonData();

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_InitStaticPool(io_ClientThreadData,
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
void io_ConnectService
(
    void
)
{
    if (!ifgen_io_HasLocalBinding())
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
le_result_t io_TryConnectService
(
    void
)
{
    if (ifgen_io_HasLocalBinding())
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

    LE_FATAL("Component for io disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void io_SetServerDisconnectHandler
(
    io_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    if (ifgen_io_HasLocalBinding())
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
void io_DisconnectService
(
    void
)
{
    if (ifgen_io_HasLocalBinding())
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
 * Create an input resource, which is used to push data into the Data Hub.
 *
 * Does nothing if the resource already exists.
 *
 * @return
 *  - LE_OK if successful.
 *  - LE_DUPLICATE if a resource by that name exists but with different direction, type or units.
 *  - LE_NO_MEMORY if the client is not permitted to create that many resources.
 */
//--------------------------------------------------------------------------------------------------
le_result_t io_CreateInput
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the client app's resource namespace.
    io_DataType_t dataType,
        ///< [IN] The data type.
    const char* LE_NONNULL units
        ///< [IN] e.g., "degC" (see senml); "" = unspecified.
)
{
    return ifgen_io_CreateInput(
        GetCurrentSessionRef(),
        path,
        dataType,
        units
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set the example value for a JSON-type Input resource.
 *
 * Does nothing if the resource is not found, is not an input, or doesn't have a JSON type.
 */
//--------------------------------------------------------------------------------------------------
void io_SetJsonExample
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the client app's resource namespace.
    const char* LE_NONNULL example
        ///< [IN] The example JSON value string.
)
{
     ifgen_io_SetJsonExample(
        GetCurrentSessionRef(),
        path,
        example
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Create an output resource, which is used to receive data output from the Data Hub.
 *
 * Does nothing if the resource already exists.
 *
 * @return
 *  - LE_OK if successful.
 *  - LE_DUPLICATE if a resource by that name exists but with different direction, type or units.
 *  - LE_NO_MEMORY if the client is not permitted to create that many resources.
 */
//--------------------------------------------------------------------------------------------------
le_result_t io_CreateOutput
(
    const char* LE_NONNULL path,
        ///< [IN] Path within the client app's resource namespace.
    io_DataType_t dataType,
        ///< [IN] The data type.
    const char* LE_NONNULL units
        ///< [IN] e.g., "degC" (see senml); "" = unspecified.
)
{
    return ifgen_io_CreateOutput(
        GetCurrentSessionRef(),
        path,
        dataType,
        units
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Delete a resource.
 *
 * Does nothing if the resource doesn't exist.
 */
//--------------------------------------------------------------------------------------------------
void io_DeleteResource
(
    const char* LE_NONNULL path
        ///< [IN] Resource path within the client app's namespace.
)
{
     ifgen_io_DeleteResource(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Push a trigger type data sample.
 */
//--------------------------------------------------------------------------------------------------
void io_PushTrigger
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double timestamp
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< IO_NOW = now (i.e., generate a timestamp for me).
)
{
     ifgen_io_PushTrigger(
        GetCurrentSessionRef(),
        path,
        timestamp
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Push a Boolean type data sample.
 */
//--------------------------------------------------------------------------------------------------
void io_PushBoolean
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double timestamp,
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< IO_NOW = now (i.e., generate a timestamp for me).
    bool value
        ///< [IN]
)
{
     ifgen_io_PushBoolean(
        GetCurrentSessionRef(),
        path,
        timestamp,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Push a numeric type data sample.
 */
//--------------------------------------------------------------------------------------------------
void io_PushNumeric
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double timestamp,
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< IO_NOW = now (i.e., generate a timestamp for me).
    double value
        ///< [IN]
)
{
     ifgen_io_PushNumeric(
        GetCurrentSessionRef(),
        path,
        timestamp,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Push a string type data sample.
 */
//--------------------------------------------------------------------------------------------------
void io_PushString
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double timestamp,
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< IO_NOW = now (i.e., generate a timestamp for me).
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_io_PushString(
        GetCurrentSessionRef(),
        path,
        timestamp,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Push a JSON data sample.
 */
//--------------------------------------------------------------------------------------------------
void io_PushJson
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double timestamp,
        ///< [IN] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
        ///< IO_NOW = now (i.e., generate a timestamp for me).
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_io_PushJson(
        GetCurrentSessionRef(),
        path,
        timestamp,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'io_TriggerPush'
 */
//--------------------------------------------------------------------------------------------------
io_TriggerPushHandlerRef_t io_AddTriggerPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    io_TriggerPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_io_AddTriggerPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'io_TriggerPush'
 */
//--------------------------------------------------------------------------------------------------
void io_RemoveTriggerPushHandler
(
    io_TriggerPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_io_RemoveTriggerPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'io_BooleanPush'
 */
//--------------------------------------------------------------------------------------------------
io_BooleanPushHandlerRef_t io_AddBooleanPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    io_BooleanPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_io_AddBooleanPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'io_BooleanPush'
 */
//--------------------------------------------------------------------------------------------------
void io_RemoveBooleanPushHandler
(
    io_BooleanPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_io_RemoveBooleanPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'io_NumericPush'
 */
//--------------------------------------------------------------------------------------------------
io_NumericPushHandlerRef_t io_AddNumericPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    io_NumericPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_io_AddNumericPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'io_NumericPush'
 */
//--------------------------------------------------------------------------------------------------
void io_RemoveNumericPushHandler
(
    io_NumericPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_io_RemoveNumericPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'io_StringPush'
 */
//--------------------------------------------------------------------------------------------------
io_StringPushHandlerRef_t io_AddStringPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    io_StringPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_io_AddStringPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'io_StringPush'
 */
//--------------------------------------------------------------------------------------------------
void io_RemoveStringPushHandler
(
    io_StringPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_io_RemoveStringPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'io_JsonPush'
 */
//--------------------------------------------------------------------------------------------------
io_JsonPushHandlerRef_t io_AddJsonPushHandler
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    io_JsonPushHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_io_AddJsonPushHandler(
        GetCurrentSessionRef(),
        path,
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'io_JsonPush'
 */
//--------------------------------------------------------------------------------------------------
void io_RemoveJsonPushHandler
(
    io_JsonPushHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_io_RemoveJsonPushHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Mark an Output resource "optional".  (By default, they are marked "mandatory".)
 */
//--------------------------------------------------------------------------------------------------
void io_MarkOptional
(
    const char* LE_NONNULL path
        ///< [IN] Resource path within the client app's namespace.
)
{
     ifgen_io_MarkOptional(
        GetCurrentSessionRef(),
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set a Boolean type value as the default value of a given resource.
 *
 * @note This will be ignored if the resource already has a default value.
 */
//--------------------------------------------------------------------------------------------------
void io_SetBooleanDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    bool value
        ///< [IN]
)
{
     ifgen_io_SetBooleanDefault(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set a numeric type value as the default value of a given resource.
 *
 * @note This will be ignored if the resource already has a default value.
 */
//--------------------------------------------------------------------------------------------------
void io_SetNumericDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double value
        ///< [IN]
)
{
     ifgen_io_SetNumericDefault(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set a string type value as the default value of a given resource.
 *
 * @note This will be ignored if the resource already has a default value.
 */
//--------------------------------------------------------------------------------------------------
void io_SetStringDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_io_SetStringDefault(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Set a JSON type value as the default value of a given resource.
 *
 * @note This will be ignored if the resource already has a default value.
 */
//--------------------------------------------------------------------------------------------------
void io_SetJsonDefault
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    const char* LE_NONNULL value
        ///< [IN]
)
{
     ifgen_io_SetJsonDefault(
        GetCurrentSessionRef(),
        path,
        value
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Fetch the timestamp of the current value of an Input or Output resource with any data type.
 *
 * @return
 *  - LE_OK if successful.
 *  - LE_NOT_FOUND if the resource does not exist.
 *  - LE_UNAVAILABLE if the resource does not currently have a value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t io_GetTimestamp
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double* timestampPtr
        ///< [OUT] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
)
{
    return ifgen_io_GetTimestamp(
        GetCurrentSessionRef(),
        path,
        timestampPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Fetch the current value of a Boolean type Input or Output resource.
 *
 * @return
 *  - LE_OK if successful.
 *  - LE_NOT_FOUND if the resource does not exist.
 *  - LE_UNAVAILABLE if the resource does not currently have a value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t io_GetBoolean
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double* timestampPtr,
        ///< [OUT] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
    bool* valuePtr
        ///< [OUT]
)
{
    return ifgen_io_GetBoolean(
        GetCurrentSessionRef(),
        path,
        timestampPtr,
        valuePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Fetch the current value of a numeric type Input or Output resource.
 *
 * @return
 *  - LE_OK if successful.
 *  - LE_NOT_FOUND if the resource does not exist.
 *  - LE_UNAVAILABLE if the resource does not currently have a value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t io_GetNumeric
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double* timestampPtr,
        ///< [OUT] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
    double* valuePtr
        ///< [OUT]
)
{
    return ifgen_io_GetNumeric(
        GetCurrentSessionRef(),
        path,
        timestampPtr,
        valuePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Fetch the current value of a string type Input or Output resource.
 *
 * @return
 *  - LE_OK if successful.
 *  - LE_OVERFLOW if the value buffer was too small to hold the value.
 *  - LE_NOT_FOUND if the resource does not exist.
 *  - LE_UNAVAILABLE if the resource does not currently have a value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t io_GetString
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double* timestampPtr,
        ///< [OUT] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
    char* value,
        ///< [OUT]
    size_t valueSize
        ///< [IN]
)
{
    return ifgen_io_GetString(
        GetCurrentSessionRef(),
        path,
        timestampPtr,
        value,
        valueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Fetch the current value of an Input or Output resource (of any data type) in JSON format.
 *
 * @return
 *  - LE_OK if successful.
 *  - LE_OVERFLOW if the value buffer was too small to hold the value.
 *  - LE_NOT_FOUND if the resource does not exist.
 *  - LE_UNAVAILABLE if the resource does not currently have a value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t io_GetJson
(
    const char* LE_NONNULL path,
        ///< [IN] Resource path within the client app's namespace.
    double* timestampPtr,
        ///< [OUT] Timestamp in seconds since the Epoch 1970-01-01 00:00:00 +0000 (UTC).
    char* value,
        ///< [OUT]
    size_t valueSize
        ///< [IN]
)
{
    return ifgen_io_GetJson(
        GetCurrentSessionRef(),
        path,
        timestampPtr,
        value,
        valueSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'io_UpdateStartEnd'
 */
//--------------------------------------------------------------------------------------------------
io_UpdateStartEndHandlerRef_t io_AddUpdateStartEndHandler
(
    io_UpdateStartEndHandlerFunc_t callbackPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_io_AddUpdateStartEndHandler(
        GetCurrentSessionRef(),
        callbackPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'io_UpdateStartEnd'
 */
//--------------------------------------------------------------------------------------------------
void io_RemoveUpdateStartEndHandler
(
    io_UpdateStartEndHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_io_RemoveUpdateStartEndHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}