/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "le_appCtrl_interface.h"
#include "le_appCtrl_messages.h"
#include "le_appCtrl_service.h"


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
    le_appCtrl_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for client threads.
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(le_appCtrl_ClientThreadData,
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
    le_result_t result = ifgen_le_appCtrl_OpenSession(sessionRef, isBlocking);
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
    if (ifgen_le_appCtrl_HasLocalBinding())
    {
        return NULL;
    }
    else
    {
        _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

        // If the thread specific data is NULL, then the session ref has not been created.
        LE_FATAL_IF(clientThreadPtr==NULL,
                    "le_appCtrl_ConnectService() not called for current thread");

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
    ifgen_le_appCtrl_InitCommonData();

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_InitStaticPool(le_appCtrl_ClientThreadData,
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
void le_appCtrl_ConnectService
(
    void
)
{
    if (!ifgen_le_appCtrl_HasLocalBinding())
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
le_result_t le_appCtrl_TryConnectService
(
    void
)
{
    if (ifgen_le_appCtrl_HasLocalBinding())
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

    LE_FATAL("Component for le_appCtrl disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void le_appCtrl_SetServerDisconnectHandler
(
    le_appCtrl_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    if (ifgen_le_appCtrl_HasLocalBinding())
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
void le_appCtrl_DisconnectService
(
    void
)
{
    if (ifgen_le_appCtrl_HasLocalBinding())
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
 * Gets a reference to an app.
 *
 * @return
 *      Reference to the named app.
 *      NULL on error (check logs for errors).
 */
//--------------------------------------------------------------------------------------------------
le_appCtrl_AppRef_t le_appCtrl_GetRef
(
    const char* LE_NONNULL appName
        ///< [IN] Name of the app to get the ref for.
)
{
    return ifgen_le_appCtrl_GetRef(
        GetCurrentSessionRef(),
        appName
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Release the reference to an app, resetting all overrides set for this app using other
 * functions, like SetRun(), SetDevicePerm(), etc.
 */
//--------------------------------------------------------------------------------------------------
void le_appCtrl_ReleaseRef
(
    le_appCtrl_AppRef_t appRef
        ///< [IN] Ref to the app.
)
{
     ifgen_le_appCtrl_ReleaseRef(
        GetCurrentSessionRef(),
        appRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Sets the run flag for a process in an app.
 *
 * If there is an error this function will kill the calling client.
 */
//--------------------------------------------------------------------------------------------------
void le_appCtrl_SetRun
(
    le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
    const char* LE_NONNULL procName,
        ///< [IN] Process name to set the run flag for.
    bool run
        ///< [IN] Flag to run the process or not.
)
{
     ifgen_le_appCtrl_SetRun(
        GetCurrentSessionRef(),
        appRef,
        procName,
        run
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Sets the debug flag for a process in an app.
 *
 * If there is an error this function will kill the calling client.
 */
//--------------------------------------------------------------------------------------------------
void le_appCtrl_SetDebug
(
    le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
    const char* LE_NONNULL procName,
        ///< [IN] Process name to set the run flag for.
    bool debug
        ///< [IN] Flag to debug the process or not.
)
{
     ifgen_le_appCtrl_SetDebug(
        GetCurrentSessionRef(),
        appRef,
        procName,
        debug
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Imports a file into the app's working directory.
 *
 * @return
 *      LE_OK if successfully imported the file.
 *      LE_DUPLICATE if the path conflicts with items already in the app's working directory.
 *      LE_NOT_FOUND if the path does not point to a valid file.
 *      LE_BAD_PARAMETER if the path is formatted incorrectly.
 *      LE_FAULT if there was some other error.
 *
 * @note If the caller is passing an invalid reference to the app, it is a fatal error,
 *       the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_appCtrl_Import
(
    le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
    const char* LE_NONNULL path
        ///< [IN] Absolute path to the file to import.
)
{
    return ifgen_le_appCtrl_Import(
        GetCurrentSessionRef(),
        appRef,
        path
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Sets a device file's permissions.
 *
 * @return
 *      LE_OK if successfully set the device's permissions.
 *      LE_NOT_FOUND if the path does not point to a valid device.
 *      LE_BAD_PARAMETER if the path is formatted incorrectly.
 *      LE_FAULT if there was some other error.
 *
 * @note If the caller is passing an invalid reference to the app, it is a fatal error,
 *       the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_appCtrl_SetDevicePerm
(
    le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
    const char* LE_NONNULL path,
        ///< [IN] Absolute path to the device.
    const char* LE_NONNULL permissions
        ///< [IN] Permission string, "r", "w", "rw".
)
{
    return ifgen_le_appCtrl_SetDevicePerm(
        GetCurrentSessionRef(),
        appRef,
        path,
        permissions
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_appCtrl_TraceAttach'
 *
 * Event that indicates the process is blocked and can be attached to.
 */
//--------------------------------------------------------------------------------------------------
le_appCtrl_TraceAttachHandlerRef_t le_appCtrl_AddTraceAttachHandler
(
    le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
    le_appCtrl_TraceAttachHandlerFunc_t attachToPidPtr,
        ///< [IN] Attach handler to register.
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_appCtrl_AddTraceAttachHandler(
        GetCurrentSessionRef(),
        appRef,
        attachToPidPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_appCtrl_TraceAttach'
 */
//--------------------------------------------------------------------------------------------------
void le_appCtrl_RemoveTraceAttachHandler
(
    le_appCtrl_TraceAttachHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_appCtrl_RemoveTraceAttachHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Unblocks the traced process.  This should normally be done once the tracer has successfully
 * attached to the process.
 *
 * @note If the caller is passing an invalid reference to the app, it is a fatal error,
 *       the function will not return.
 */
//--------------------------------------------------------------------------------------------------
void le_appCtrl_TraceUnblock
(
    le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
    int32_t pid
        ///< [IN] PID of the process to unblock.
)
{
     ifgen_le_appCtrl_TraceUnblock(
        GetCurrentSessionRef(),
        appRef,
        pid
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Starts an app.
 *
 * @return
 *      LE_OK if the app is successfully started.
 *      LE_DUPLICATE if the app is already running.
 *      LE_NOT_FOUND if the app isn't installed.
 *      LE_FAULT if there was an error and the app could not be launched.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_appCtrl_Start
(
    const char* LE_NONNULL appName
        ///< [IN] Name of the app to start.
)
{
    return ifgen_le_appCtrl_Start(
        GetCurrentSessionRef(),
        appName
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Stops an app.
 *
 * @return
 *      LE_OK if successful.
 *      LE_NOT_FOUND if the app could not be found.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_appCtrl_Stop
(
    const char* LE_NONNULL appName
        ///< [IN] Name of the app to stop.
)
{
    return ifgen_le_appCtrl_Stop(
        GetCurrentSessionRef(),
        appName
    );
}