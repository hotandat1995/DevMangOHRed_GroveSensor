/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

#include "le_sim_interface.h"
#include "le_sim_messages.h"
#include "le_sim_service.h"


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
    le_sim_DisconnectHandler_t disconnectHandler; ///< Disconnect handler for this thread
    void*               contextPtr;     ///< Context for disconnect handler
}
_ClientThreadData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for client threads.
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(le_sim_ClientThreadData,
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
    le_result_t result = ifgen_le_sim_OpenSession(sessionRef, isBlocking);
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
    if (ifgen_le_sim_HasLocalBinding())
    {
        return NULL;
    }
    else
    {
        _ClientThreadData_t* clientThreadPtr = GetClientThreadDataPtr();

        // If the thread specific data is NULL, then the session ref has not been created.
        LE_FATAL_IF(clientThreadPtr==NULL,
                    "le_sim_ConnectService() not called for current thread");

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
    ifgen_le_sim_InitCommonData();

    // Allocate the client thread pool
    _ClientThreadDataPool = le_mem_InitStaticPool(le_sim_ClientThreadData,
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
void le_sim_ConnectService
(
    void
)
{
    if (!ifgen_le_sim_HasLocalBinding())
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
le_result_t le_sim_TryConnectService
(
    void
)
{
    if (ifgen_le_sim_HasLocalBinding())
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

    LE_FATAL("Component for le_sim disconnected\n");
}

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
void le_sim_SetServerDisconnectHandler
(
    le_sim_DisconnectHandler_t disconnectHandler,
    void *contextPtr
)
{
    if (ifgen_le_sim_HasLocalBinding())
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
void le_sim_DisconnectService
(
    void
)
{
    if (ifgen_le_sim_HasLocalBinding())
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
 * Add handler function for EVENT 'le_sim_NewState'
 *
 * This event provides information on sim state changes.
 *
 */
//--------------------------------------------------------------------------------------------------
le_sim_NewStateHandlerRef_t le_sim_AddNewStateHandler
(
    le_sim_NewStateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_sim_AddNewStateHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_sim_NewState'
 */
//--------------------------------------------------------------------------------------------------
void le_sim_RemoveNewStateHandler
(
    le_sim_NewStateHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_sim_RemoveNewStateHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_sim_ProfileUpdate'
 *
 * This event provides information on profile update
 *
 */
//--------------------------------------------------------------------------------------------------
le_sim_ProfileUpdateHandlerRef_t le_sim_AddProfileUpdateHandler
(
    le_sim_ProfileUpdateHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_sim_AddProfileUpdateHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_sim_ProfileUpdate'
 */
//--------------------------------------------------------------------------------------------------
void le_sim_RemoveProfileUpdateHandler
(
    le_sim_ProfileUpdateHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_sim_RemoveProfileUpdateHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_sim_SimToolkitEvent'
 *
 * This event provides information on Sim Toolkit application.
 *
 */
//--------------------------------------------------------------------------------------------------
le_sim_SimToolkitEventHandlerRef_t le_sim_AddSimToolkitEventHandler
(
    le_sim_SimToolkitEventHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_sim_AddSimToolkitEventHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_sim_SimToolkitEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_sim_RemoveSimToolkitEventHandler
(
    le_sim_SimToolkitEventHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_sim_RemoveSimToolkitEventHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_sim_IccidChange'
 *
 * This event provides information on ICCID
 *
 */
//--------------------------------------------------------------------------------------------------
le_sim_IccidChangeHandlerRef_t le_sim_AddIccidChangeHandler
(
    le_sim_IccidChangeHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
)
{
    return ifgen_le_sim_AddIccidChangeHandler(
        GetCurrentSessionRef(),
        handlerPtr,
        contextPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_sim_IccidChange'
 */
//--------------------------------------------------------------------------------------------------
void le_sim_RemoveIccidChangeHandler
(
    le_sim_IccidChangeHandlerRef_t handlerRef
        ///< [IN]
)
{
     ifgen_le_sim_RemoveIccidChangeHandler(
        GetCurrentSessionRef(),
        handlerRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the current selected card.
 *
 * @return Number of the current selected SIM card.
 */
//--------------------------------------------------------------------------------------------------
le_sim_Id_t le_sim_GetSelectedCard
(
    void
)
{
    return ifgen_le_sim_GetSelectedCard(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Select a SIM.
 *
 * @return LE_FAULT         Function failed to select the requested SIM
 * @return LE_OK            Function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SelectCard
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
)
{
    return ifgen_le_sim_SelectCard(
        GetCurrentSessionRef(),
        simId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Retrieves the integrated circuit card identifier (ICCID) of the SIM card (20 digits)
 *
 * @return LE_OK             ICCID was successfully retrieved.
 * @return LE_OVERFLOW       iccidPtr buffer was too small for the ICCID.
 * @return LE_BAD_PARAMETER  Invalid parameters.
 * @return LE_FAULT          The ICCID could not be retrieved.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetICCID
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* iccid,
        ///< [OUT] ICCID
    size_t iccidSize
        ///< [IN]
)
{
    return ifgen_le_sim_GetICCID(
        GetCurrentSessionRef(),
        simId,
        iccid,
        iccidSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Retrieves the identifier for the embedded Universal Integrated Circuit Card identifier (EID)
 * (32 digits)
 *
 * @return LE_OK             EID was successfully retrieved.
 * @return LE_OVERFLOW       eidPtr buffer was too small for the EID.
 * @return LE_BAD_PARAMETER  Invalid parameters.
 * @return LE_FAULT          The EID could not be retrieved.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 *
 * @warning le_sim_GetEID() function is platform dependent. Please refer to the
 *          @ref platformConstraintsSim_EID section for full details.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetEID
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* eid,
        ///< [OUT] EID
    size_t eidSize
        ///< [IN]
)
{
    return ifgen_le_sim_GetEID(
        GetCurrentSessionRef(),
        simId,
        eid,
        eidSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Retrieves the identification number (IMSI) of the SIM card. (max 15 digits)
 *
 * @return LE_OVERFLOW      The imsiPtr buffer was too small for the IMSI.
 * @return LE_BAD_PARAMETER The parameters are invalid.
 * @return LE_FAULT         The function failed.
 * @return LE_TIMEOUT       No response was received.
 * @return LE_OK            The function succeeded.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetIMSI
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* imsi,
        ///< [OUT] IMSI
    size_t imsiSize
        ///< [IN]
)
{
    return ifgen_le_sim_GetIMSI(
        GetCurrentSessionRef(),
        simId,
        imsi,
        imsiSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Verify if the SIM card is present or not.
 *
 * @return true   SIM card is present.
 * @return false  SIM card is absent
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_sim_IsPresent
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
)
{
    return ifgen_le_sim_IsPresent(
        GetCurrentSessionRef(),
        simId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Verify if the SIM is ready (PIN code correctly inserted or not
 * required).
 *
 * @return true   PIN is correctly inserted or not required.
 * @return false  PIN must be inserted
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
bool le_sim_IsReady
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
)
{
    return ifgen_le_sim_IsReady(
        GetCurrentSessionRef(),
        simId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to enter the PIN code.
 *
 * @return LE_BAD_PARAMETER The parameters are invalid.
 * @return LE_NOT_FOUND     The function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     The PIN code is not long enough (min 4 digits).
 * @return LE_FAULT         The function failed to enter the PIN code.
 * @return LE_OK            The function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_EnterPIN
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL pin
        ///< [IN] The PIN code.
)
{
    return ifgen_le_sim_EnterPIN(
        GetCurrentSessionRef(),
        simId,
        pin
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Change the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is/are not long enough (min 4 digits).
 * @return LE_FAULT         Function failed to change the PIN code.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_ChangePIN
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL oldpin,
        ///< [IN] The old PIN code.
    const char* LE_NONNULL newpin
        ///< [IN] The new PIN code.
)
{
    return ifgen_le_sim_ChangePIN(
        GetCurrentSessionRef(),
        simId,
        oldpin,
        newpin
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of remaining PIN insertion tries.
 *
 * @return LE_NOT_FOUND     The function failed to select the SIM card for this operation.
 * @return LE_BAD_PARAMETER Invalid SIM identifier.
 * @return LE_FAULT         The function failed to get the number of remaining PIN insertion tries.
 * @return A positive value The function succeeded. The number of remaining PIN insertion tries.
 *
 */
//--------------------------------------------------------------------------------------------------
int32_t le_sim_GetRemainingPINTries
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
)
{
    return ifgen_le_sim_GetRemainingPINTries(
        GetCurrentSessionRef(),
        simId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of remaining PUK insertion tries.
 *
 * @return LE_OK            On success.
 * @return LE_NOT_FOUND     The function failed to select the SIM card for this operation.
 * @return LE_BAD_PARAMETER Invalid SIM identifier.
 * @return LE_FAULT         The function failed to get the number of remaining PUK insertion tries.
 *
 * @note If the caller is passing an null pointer to this function, it is a fatal error
 *       and the function will not return.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetRemainingPUKTries
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    uint32_t* remainingPukTriesPtrPtr
        ///< [OUT] The number of remaining PUK insertion tries.
)
{
    return ifgen_le_sim_GetRemainingPUKTries(
        GetCurrentSessionRef(),
        simId,
        remainingPukTriesPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Unlock the SIM card: it disables the request of the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_FAULT         The function failed to unlock the SIM card.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Unlock
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL pin
        ///< [IN] The PIN code.
)
{
    return ifgen_le_sim_Unlock(
        GetCurrentSessionRef(),
        simId,
        pin
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Lock the SIM card: it enables the request of the PIN code.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_FAULT         The function failed to unlock the SIM card.
 * @return LE_OK            Function succeeded.
 *
 * @note If PIN code is too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Lock
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL pin
        ///< [IN] The PIN code.
)
{
    return ifgen_le_sim_Lock(
        GetCurrentSessionRef(),
        simId,
        pin
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Unblock the SIM card.
 *
 * @return LE_NOT_FOUND     Function failed to select the SIM card for this operation.
 * @return LE_UNDERFLOW     PIN code is not long enough (min 4 digits).
 * @return LE_BAD_PARAMETER Invalid SIM identifier.
 * @return LE_OUT_OF_RANGE  PUK code length is not correct (8 digits).
 * @return LE_FAULT         The function failed to unlock the SIM card.
 * @return LE_OK            Function succeeded.
 *
 * @note If new PIN or puk code are too long (max 8 digits), it is a fatal error, the
 *       function will not return.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Unblock
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const char* LE_NONNULL puk,
        ///< [IN] The PUK code.
    const char* LE_NONNULL newpin
        ///< [IN] The PIN code.
)
{
    return ifgen_le_sim_Unblock(
        GetCurrentSessionRef(),
        simId,
        puk,
        newpin
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the SIM state.
 *
 * @return Current SIM state.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_sim_States_t le_sim_GetState
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
)
{
    return ifgen_le_sim_GetState(
        GetCurrentSessionRef(),
        simId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the SIM Phone Number.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the Phone Number can't fit in phoneNumberStr
 *      - LE_BAD_PARAMETER if a parameter is invalid
 *      - LE_FAULT on any other failure
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetSubscriberPhoneNumber
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* phoneNumberStr,
        ///< [OUT] The phone Number.
    size_t phoneNumberStrSize
        ///< [IN]
)
{
    return ifgen_le_sim_GetSubscriberPhoneNumber(
        GetCurrentSessionRef(),
        simId,
        phoneNumberStr,
        phoneNumberStrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Home Network Name information.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if the Home Network Name can't fit in nameStr
 *      - LE_BAD_PARAMETER if a parameter is invalid
 *      - LE_FAULT on any other failure
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 *
 * @note The home network name can be given even if the device is not registered on the network.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetHomeNetworkOperator
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    char* nameStr,
        ///< [OUT] the home network Name
    size_t nameStrSize
        ///< [IN]
)
{
    return ifgen_le_sim_GetHomeNetworkOperator(
        GetCurrentSessionRef(),
        simId,
        nameStr,
        nameStrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the Home Network MCC MNC.
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if Home Network has not been provisioned
 *      - LE_FAULT for unexpected error
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetHomeNetworkMccMnc
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
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
    return ifgen_le_sim_GetHomeNetworkMccMnc(
        GetCurrentSessionRef(),
        simId,
        mccPtr,
        mccPtrSize,
        mncPtr,
        mncPtrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to request the multi-profile eUICC to swap to ECS and to refresh.
 * The User's application must wait for eUICC reboot to be finished and network connection
 * available.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER invalid SIM identifier
 *      - LE_BUSY when a profile swap is already in progress
 *      - LE_FAULT for unexpected error
 *      - LE_DUPLICATE for duplicate operation
 *
 * @note Please ensure that the eUICC is selected using le_sim_SelectCard() and in a ready state
 *       before attempting a profile swap.
 *
 * @warning If you use a Morpho or Oberthur card, the SIM_REFRESH PRO-ACTIVE command must be
 *          accepted with le_sim_AcceptSimToolkitCommand() in order to complete the profile swap
 *          procedure.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_LocalSwapToEmergencyCallSubscription
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_Manufacturer_t manufacturer
        ///< [IN] The card manufacturer.
)
{
    return ifgen_le_sim_LocalSwapToEmergencyCallSubscription(
        GetCurrentSessionRef(),
        simId,
        manufacturer
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to request the multi-profile eUICC to swap back to commercial
 * subscription and to refresh.
 * The User's application must wait for eUICC reboot to be finished and network connection
 * available.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER invalid SIM identifier
 *      - LE_BUSY when a profile swap is already in progress
 *      - LE_FAULT for unexpected error
 *      - LE_DUPLICATE for duplicate operation
 *
 * @note Please ensure that the eUICC is selected using le_sim_SelectCard() and in a ready state
 *       before attempting a profile swap.
 *
 * @warning If you use a Morpho or Oberthur card, the SIM_REFRESH PRO-ACTIVE command must be
 *          accepted with le_sim_AcceptSimToolkitCommand() in order to complete the profile swap
 *          procedure.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_LocalSwapToCommercialSubscription
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_Manufacturer_t manufacturer
        ///< [IN] The card manufacturer.
)
{
    return ifgen_le_sim_LocalSwapToCommercialSubscription(
        GetCurrentSessionRef(),
        simId,
        manufacturer
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * This function must be called to get the current subscription.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER invalid SIM identifier or null ECS pointer is passed
 *      - LE_NOT_FOUND cannot determine the current selected subscription
 *      - LE_FAULT for unexpected errors
 *
 * @warning There is no standard method to interrogate the current selected subscription. The
 * returned value of this function is based on the last executed local swap command. This means
 * that this function will always return LE_NOT_FOUND error at Legato startup.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_IsEmergencyCallSubscriptionSelected
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier
    bool* isEcsPtr
        ///< [OUT] true if Emergency Call Subscription (ECS) is selected,
        ///<       false if Commercial Subscription is selected
)
{
    return ifgen_le_sim_IsEmergencyCallSubscriptionSelected(
        GetCurrentSessionRef(),
        simId,
        isEcsPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Accept the last SIM Toolkit command.
 *
 * @return
 *  - LE_OK       The function succeeded.
 *  - LE_FAULT    The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_AcceptSimToolkitCommand
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
)
{
    return ifgen_le_sim_AcceptSimToolkitCommand(
        GetCurrentSessionRef(),
        simId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reject the last SIM Toolkit command.
 *
 * @return
 *  - LE_OK       The function succeeded.
 *  - LE_FAULT    The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_RejectSimToolkitCommand
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
)
{
    return ifgen_le_sim_RejectSimToolkitCommand(
        GetCurrentSessionRef(),
        simId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the mode of the last SIM Toolkit Refresh command.
 * The modes are defined in ETSI TS 102 223 sections 6.4.7 and 8.6.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_BAD_PARAMETER  A parameter is invalid.
 *  - LE_UNAVAILABLE    The last SIM Toolkit command is not a Refresh command.
 *  - LE_FAULT          The function failed.
 *  - LE_UNSUPPORTED    The platform does not support this operation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetSimToolkitRefreshMode
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_StkRefreshMode_t* refreshModePtr
        ///< [OUT] The Refresh mode.
)
{
    return ifgen_le_sim_GetSimToolkitRefreshMode(
        GetCurrentSessionRef(),
        simId,
        refreshModePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the stage of the last SIM Toolkit Refresh command.
 *
 * @return
 *  - LE_OK             The function succeeded.
 *  - LE_BAD_PARAMETER  A parameter is invalid.
 *  - LE_UNAVAILABLE    The last SIM Toolkit command is not a Refresh command.
 *  - LE_FAULT          The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetSimToolkitRefreshStage
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_StkRefreshStage_t* refreshStagePtr
        ///< [OUT] The Refresh stage.
)
{
    return ifgen_le_sim_GetSimToolkitRefreshStage(
        GetCurrentSessionRef(),
        simId,
        refreshStagePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Send APDU command to the SIM.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          The function failed.
 *      - LE_BAD_PARAMETER  A parameter is invalid.
 *      - LE_NOT_FOUND      The function failed to select the SIM card for this operation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SendApdu
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    const uint8_t* commandApduPtr,
        ///< [IN] APDU command.
    size_t commandApduSize,
        ///< [IN]
    uint8_t* responseApduPtr,
        ///< [OUT] SIM response.
    size_t* responseApduSizePtr
        ///< [INOUT]
)
{
    return ifgen_le_sim_SendApdu(
        GetCurrentSessionRef(),
        simId,
        commandApduPtr,
        commandApduSize,
        responseApduPtr,
        responseApduSizePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Send a command to the SIM.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          The function failed.
 *      - LE_BAD_PARAMETER  A parameter is invalid.
 *      - LE_NOT_FOUND      - The function failed to select the SIM card for this operation
 *                          - The requested SIM file is not found
 *      - LE_OVERFLOW       Response buffer is too small to copy the SIM answer.
 *      - LE_UNSUPPORTED    The platform does not support this operation.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SendCommand
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_Command_t command,
        ///< [IN] The SIM command.
    const char* LE_NONNULL fileIdentifier,
        ///< [IN] File identifier
    uint8_t p1,
        ///< [IN] Parameter P1 passed to the SIM
    uint8_t p2,
        ///< [IN] Parameter P2 passed to the SIM
    uint8_t p3,
        ///< [IN] Parameter P3 passed to the SIM
    const uint8_t* dataPtr,
        ///< [IN] data command.
    size_t dataSize,
        ///< [IN]
    const char* LE_NONNULL path,
        ///< [IN] path of the elementary file
    uint8_t* sw1Ptr,
        ///< [OUT] Status Word 1 received from the SIM
    uint8_t* sw2Ptr,
        ///< [OUT] Status Word 2 received from the SIM
    uint8_t* responsePtr,
        ///< [OUT] SIM response.
    size_t* responseSizePtr
        ///< [INOUT]
)
{
    return ifgen_le_sim_SendCommand(
        GetCurrentSessionRef(),
        simId,
        command,
        fileIdentifier,
        p1,
        p2,
        p3,
        dataPtr,
        dataSize,
        path,
        sw1Ptr,
        sw2Ptr,
        responsePtr,
        responseSizePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Enable or disable the automatic SIM selection
 *
 * @note Automatic SIM selection uses the following rule: If an external SIM is inserted in
 *       slot 1 then select it. Otherwise, fall back to the internal SIM card.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed to execute.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SetAutomaticSelection
(
    bool enable
        ///< [IN] True if the feature needs to be enabled. False otherwise.
)
{
    return ifgen_le_sim_SetAutomaticSelection(
        GetCurrentSessionRef(),
        enable
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the automatic SIM selection
 *
 * @note When enabled, automatic SIM selection uses the following rule: If an external SIM is
 *       inserted in slot 1 then select it. Otherwise, fall back to the internal SIM card.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_FAULT          Function failed to execute.
 *      - LE_BAD_PARAMETER  Invalid parameter.
 *      - LE_UNSUPPORTED    The platform does not support this operation.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetAutomaticSelection
(
    bool* enablePtr
        ///< [OUT] True if the feature is enabled. False otherwise.
)
{
    return ifgen_le_sim_GetAutomaticSelection(
        GetCurrentSessionRef(),
        enablePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Reset the SIM.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_Reset
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
)
{
    return ifgen_le_sim_Reset(
        GetCurrentSessionRef(),
        simId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Create empty FPLMN list to insert FPLMN operators.
 *
 * @return
 *      - Reference to the List object.
 *      - Null pointer if not able to create list reference.
 */
//--------------------------------------------------------------------------------------------------
le_sim_FPLMNListRef_t le_sim_CreateFPLMNList
(
    void
)
{
    return ifgen_le_sim_CreateFPLMNList(
        GetCurrentSessionRef()
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Add FPLMN network into the newly created FPLMN list.
 * If the FPLMNListRef, mcc or mnc is not valid then this function will kill the calling client.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 *      - LE_OVERFLOW      If FPLMN operator can not be inserted into FPLMN list.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_AddFPLMNOperator
(
    le_sim_FPLMNListRef_t FPLMNListRef,
        ///< [IN] FPLMN list reference.
    const char* LE_NONNULL mcc,
        ///< [IN] Mobile Country Code.
    const char* LE_NONNULL mnc
        ///< [IN] Mobile Network Code.
)
{
    return ifgen_le_sim_AddFPLMNOperator(
        GetCurrentSessionRef(),
        FPLMNListRef,
        mcc,
        mnc
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Write FPLMN list into the SIM.
 * If the FPLMNListRef is not valid then this function will kill the calling client.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 *
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_WriteFPLMNList
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_sim_FPLMNListRef_t FPLMNListRef
        ///< [IN] FPLMN list reference.
)
{
    return ifgen_le_sim_WriteFPLMNList(
        GetCurrentSessionRef(),
        simId,
        FPLMNListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Read FPLMN list from the SIM.
 *
 * @return
 *      - Reference to the List object.
 *      - Null pointer if FPLMN list is not able to read from the SIM.
 */
//--------------------------------------------------------------------------------------------------
le_sim_FPLMNListRef_t le_sim_ReadFPLMNList
(
    le_sim_Id_t simId
        ///< [IN] The SIM identifier.
)
{
    return ifgen_le_sim_ReadFPLMNList(
        GetCurrentSessionRef(),
        simId
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Read the first FPLMN network from the list of FPLMN objects retrieved with
 * le_sim_ReadFPLMNList().
 * If the FPLMNListRef, mccPtr or mncPtr is not valid then this function will kill the calling
 * client.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 *      - LE_OVERFLOW      MCC/MNC string size is greater than string length parameter which has
 *                         been given into this function.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetFirstFPLMNOperator
(
    le_sim_FPLMNListRef_t FPLMNListRef,
        ///< [IN] FPLMN list reference.
    char* mccPtr,
        ///< [OUT] Mobile Country Code.
    size_t mccPtrSize,
        ///< [IN]
    char* mncPtr,
        ///< [OUT] Mobile Network Code.
    size_t mncPtrSize
        ///< [IN]
)
{
    return ifgen_le_sim_GetFirstFPLMNOperator(
        GetCurrentSessionRef(),
        FPLMNListRef,
        mccPtr,
        mccPtrSize,
        mncPtr,
        mncPtrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Read the next FPLMN network from the list of FPLMN objects retrieved with le_sim_ReadFPLMNList().
 * If the FPLMNListRef, mccPtr or mncPtr is not valid then this function will kill the calling
 * client.
 *
 * @return
 *      - LE_FAULT         Function failed.
 *      - LE_OK            Function succeeded.
 *      - LE_OVERFLOW      MCC/MNC string size is greater than string length parameter which has
 *                         been given into this function.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_GetNextFPLMNOperator
(
    le_sim_FPLMNListRef_t FPLMNListRef,
        ///< [IN] FPLMN list reference.
    char* mccPtr,
        ///< [OUT] Mobile Country Code.
    size_t mccPtrSize,
        ///< [IN]
    char* mncPtr,
        ///< [OUT] Mobile Network Code.
    size_t mncPtrSize
        ///< [IN]
)
{
    return ifgen_le_sim_GetNextFPLMNOperator(
        GetCurrentSessionRef(),
        FPLMNListRef,
        mccPtr,
        mccPtrSize,
        mncPtr,
        mncPtrSize
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Delete the FPLMN list created by le_sim_ReadFPLMNList() or le_sim_CreateFPLMNList().
 * If the FPLMNListRef is not valid then this function will kill the calling client.
 *
 * @note
 *      On failure, the process exits, so you don't have to worry about checking the returned
 *      reference for validity.
 */
//--------------------------------------------------------------------------------------------------
void le_sim_DeleteFPLMNList
(
    le_sim_FPLMNListRef_t FPLMNListRef
        ///< [IN] FPLMN list reference.
)
{
     ifgen_le_sim_DeleteFPLMNList(
        GetCurrentSessionRef(),
        FPLMNListRef
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Open a logical channel on the SIM card.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_BAD_PARAMETER Invalid parameter.
 *      - LE_FAULT         Function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_OpenLogicalChannel
(
    uint8_t* channelPtrPtr
        ///< [OUT] The number of the opened logical channel.
)
{
    return ifgen_le_sim_OpenLogicalChannel(
        GetCurrentSessionRef(),
        channelPtrPtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Close a logical channel on the SIM card.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_CloseLogicalChannel
(
    uint8_t channel
        ///< [IN] The number of the logical channel to close.
)
{
    return ifgen_le_sim_CloseLogicalChannel(
        GetCurrentSessionRef(),
        channel
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Send APDU command on a dedicated logical channel.
 *
 * @return
 *      - LE_OK             Function succeeded.
 *      - LE_BAD_PARAMETER  A parameter is invalid.
 *      - LE_NOT_FOUND      The function failed to select the SIM card for this operation.
 *      - LE_FAULT          The function failed.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SendApduOnChannel
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    uint8_t channel,
        ///< [IN] The logical channel number.
    const uint8_t* commandApduPtr,
        ///< [IN] APDU command.
    size_t commandApduSize,
        ///< [IN]
    uint8_t* responseApduPtr,
        ///< [OUT] SIM response.
    size_t* responseApduSizePtr
        ///< [INOUT]
)
{
    return ifgen_le_sim_SendApduOnChannel(
        GetCurrentSessionRef(),
        simId,
        channel,
        commandApduPtr,
        commandApduSize,
        responseApduPtr,
        responseApduSizePtr
    );
}

//--------------------------------------------------------------------------------------------------
/**
 * Power up or down the current SIM.
 *
 * @return LE_OK        Function succeeded.
 * @return LE_FAULT     Function failed.
 *
 * @note For SIM power cycle operation, it must wait until SIM state is LE_SIM_POWER_DOWN
 *       before powering on the SIM, otherwise power up SIM will fail.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_sim_SetPower
(
    le_sim_Id_t simId,
        ///< [IN] The SIM identifier.
    le_onoff_t power
        ///< [IN] The power state.
)
{
    return ifgen_le_sim_SetPower(
        GetCurrentSessionRef(),
        simId,
        power
    );
}