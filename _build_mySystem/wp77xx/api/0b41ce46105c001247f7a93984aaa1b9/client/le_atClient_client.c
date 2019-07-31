/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "le_atClient_interface.h"
#include "le_atClient_messages.h"
#include "le_atClient_service.h"


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
    le_atClient_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for client threads.
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(le_atClient_ClientThreadData,
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
    le_result_t result = ifgen_le_atClient_OpenSession(sessionRef, isBlocking);
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
    if (ifgen_le_atClient_HasLocalBinding())
    {
        return NULL;
    }
    else
    {
        _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

        // If the thread specific data is NULL, then the session ref has not been created.
        LE_FATAL_IF(clientThreadPtr==NULL,
                    "le_atClient_ConnectService() not called for current thread");

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
    ifgen_le_atClient_InitCommonData();

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_InitStaticPool(le_atClient_ClientThreadData,
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
void le_atClient_ConnectService
(
    void
)
{
    if (!ifgen_le_atClient_HasLocalBinding())
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
le_result_t le_atClient_TryConnectService
(
    void
)
{
    if (ifgen_le_atClient_HasLocalBinding())
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

    LE_FATAL("Component for le_atClient disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void le_atClient_SetServerDisconnectHandler
(
    le_atClient_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    if (ifgen_le_atClient_HasLocalBinding())
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
void le_atClient_DisconnectService
(
    void
)
{
    if (ifgen_le_atClient_HasLocalBinding())
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
 * This function must be called to start a ATClient session on a specified device.
 *
 * @return reference on a device context
 */
//--------------------------------------------------------------------------------------------------
le_atClient_DeviceRef_t le_atClient_Start
(
    int fd
        ///< [IN] File descriptor.
)
{
    return ifgen_le_atClient_Start(
        GetCurrentSessionRef(),
        fd
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to stop the ATClient session on the specified device.
 *
 * @return
 *      - LE_FAULT when function failed
 *      - LE_OK when function succeed
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_Stop
(
    le_atClient_DeviceRef_t device
        ///< [IN] Device reference
)
{
    return ifgen_le_atClient_Stop(
        GetCurrentSessionRef(),
        device
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to create a new AT command.
 *
 * @return pointer to the new AT Command reference
 */
//--------------------------------------------------------------------------------------------------
le_atClient_CmdRef_t le_atClient_Create
(
    void
)
{
    return ifgen_le_atClient_Create(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to delete an AT command reference.
 *
 * @return
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_Delete
(
    le_atClient_CmdRef_t cmdRef
        ///< [IN] AT Command
)
{
    return ifgen_le_atClient_Delete(
        GetCurrentSessionRef(),
        cmdRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to set the AT command string to be sent.
 *
 * @return
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_SetCommand
(
    le_atClient_CmdRef_t cmdRef,
        ///< [IN] AT Command
    const char* LE_NONNULL command
        ///< [IN] Set Command
)
{
    return ifgen_le_atClient_SetCommand(
        GetCurrentSessionRef(),
        cmdRef,
        command
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to set the waiting intermediate responses.
 * Several intermediate responses can be specified separated by a '|' character into the string
 * given in parameter.
 *
 * @return
 *      - LE_FAULT when function failed
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference or set intermediate response is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_SetIntermediateResponse
(
    le_atClient_CmdRef_t cmdRef,
        ///< [IN] AT Command
    const char* LE_NONNULL intermediate
        ///< [IN] Set Intermediate
)
{
    return ifgen_le_atClient_SetIntermediateResponse(
        GetCurrentSessionRef(),
        cmdRef,
        intermediate
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to set the final response(s) of the AT command execution.
 * Several final responses can be specified separated by a '|' character into the string given in
 * parameter.
 *
 * @return
 *      - LE_FAULT when function failed
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference or set response is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_SetFinalResponse
(
    le_atClient_CmdRef_t cmdRef,
        ///< [IN] AT Command
    const char* LE_NONNULL response
        ///< [IN] Set Response
)
{
    return ifgen_le_atClient_SetFinalResponse(
        GetCurrentSessionRef(),
        cmdRef,
        response
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to set the text when the prompt is expected.
 *
 * @return
 *      - LE_FAULT when function failed
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_SetText
(
    le_atClient_CmdRef_t cmdRef,
        ///< [IN] AT Command
    const char* LE_NONNULL text
        ///< [IN] The AT Data to send
)
{
    return ifgen_le_atClient_SetText(
        GetCurrentSessionRef(),
        cmdRef,
        text
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to set the timeout of the AT command execution.
 *
 * @return
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_SetTimeout
(
    le_atClient_CmdRef_t cmdRef,
        ///< [IN] AT Command
    uint32_t timer
        ///< [IN] The timeout value in milliseconds.
)
{
    return ifgen_le_atClient_SetTimeout(
        GetCurrentSessionRef(),
        cmdRef,
        timer
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to set the device where the AT command will be sent.
 *
 * @return
 *      - LE_FAULT when function failed
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_SetDevice
(
    le_atClient_CmdRef_t cmdRef,
        ///< [IN] AT Command
    le_atClient_DeviceRef_t devRef
        ///< [IN] Device where the AT command has to be sent
)
{
    return ifgen_le_atClient_SetDevice(
        GetCurrentSessionRef(),
        cmdRef,
        devRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to send an AT Command and wait for response.
 *
 * @return
 *      - LE_FAULT when function failed
 *      - LE_TIMEOUT when a timeout occur
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_Send
(
    le_atClient_CmdRef_t cmdRef
        ///< [IN] AT Command
)
{
    return ifgen_le_atClient_Send(
        GetCurrentSessionRef(),
        cmdRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function is used to get the first intermediate response.
 *
 * @return
 *      - LE_FAULT when function failed
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_GetFirstIntermediateResponse
(
    le_atClient_CmdRef_t cmdRef,
        ///< [IN] AT Command
    char* intermediateRsp,
        ///< [OUT] First intermediate result code
    size_t intermediateRspSize
        ///< [IN]
)
{
    return ifgen_le_atClient_GetFirstIntermediateResponse(
        GetCurrentSessionRef(),
        cmdRef,
        intermediateRsp,
        intermediateRspSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function is used to get the next intermediate response.
 *
 * @return
 *      - LE_NOT_FOUND when there are no further results
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_GetNextIntermediateResponse
(
    le_atClient_CmdRef_t cmdRef,
        ///< [IN] AT Command
    char* intermediateRsp,
        ///< [OUT] Get Next intermediate result
        ///< code.
    size_t intermediateRspSize
        ///< [IN]
)
{
    return ifgen_le_atClient_GetNextIntermediateResponse(
        GetCurrentSessionRef(),
        cmdRef,
        intermediateRsp,
        intermediateRspSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function is used to get the final response
 *
 * @return
 *      - LE_FAULT when function failed
 *      - LE_OK when function succeed
 *
 * @note If the AT Command reference is invalid, a fatal error occurs,
 *       the function won't return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_GetFinalResponse
(
    le_atClient_CmdRef_t cmdRef,
        ///< [IN] AT Command
    char* finalRsp,
        ///< [OUT] Get Final Line
    size_t finalRspSize
        ///< [IN]
)
{
    return ifgen_le_atClient_GetFinalResponse(
        GetCurrentSessionRef(),
        cmdRef,
        finalRsp,
        finalRspSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to automatically set and send an AT Command.
 *
 * @return
 *      - LE_FAULT when function failed
 *      - LE_TIMEOUT when a timeout occur
 *      - LE_OK when function succeed
 *
 * @note This command creates a command reference when called
 *
 * @note In case of an Error the command reference will be deleted and though
 *       not usable. Make sure to test the return code and not use the reference
 *       in other functions.
 *
 * @note If the AT command is invalid, a fatal error occurs,
 *        the function won't return.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_atClient_SetCommandAndSend
(
    le_atClient_CmdRef_t* cmdRefPtr,
        ///< [OUT] Cmd reference
    le_atClient_DeviceRef_t devRef,
        ///< [IN] Dev reference
    const char* LE_NONNULL command,
        ///< [IN] AT Command
    const char* LE_NONNULL interResp,
        ///< [IN] Expected intermediate response
    const char* LE_NONNULL finalResp,
        ///< [IN] Expected final response
    uint32_t timeout
        ///< [IN] Timeout value in milliseconds.
)
{
    return ifgen_le_atClient_SetCommandAndSend(
        GetCurrentSessionRef(),
        cmdRefPtr,
        devRef,
        command,
        interResp,
        finalResp,
        timeout
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_atClient_UnsolicitedResponse'
 *
 * This event provides information on a subscribed unsolicited response when this unsolicited
 * response is received.
 *
 */
//--------------------------------------------------------------------------------------------------
le_atClient_UnsolicitedResponseHandlerRef_t le_atClient_AddUnsolicitedResponseHandler
(
    const char* LE_NONNULL unsolRsp,
        ///< [IN] Pattern to match
    le_atClient_DeviceRef_t devRef,
        ///< [IN] Device to listen
    le_atClient_UnsolicitedResponseHandlerFunc_t handlerPtr,
        ///< [IN] unsolicited handler
    void* contextPtr,
        ///< [IN]
    uint32_t lineCount
        ///< [IN] Indicate the number of line of
        ///< the unsolicited
)
{
    return ifgen_le_atClient_AddUnsolicitedResponseHandler(
        GetCurrentSessionRef(),
        unsolRsp,
        devRef,
        handlerPtr,
        contextPtr,
        lineCount
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_atClient_UnsolicitedResponse'
 */
//--------------------------------------------------------------------------------------------------
void le_atClient_RemoveUnsolicitedResponseHandler
(
    le_atClient_UnsolicitedResponseHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_atClient_RemoveUnsolicitedResponseHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}