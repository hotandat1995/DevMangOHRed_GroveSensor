
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#include "le_sim_server.h"
#include "le_sim_messages.h"
#include "le_sim_service.h"


//--------------------------------------------------------------------------------------------------
// Generic Server Types, Variables and Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Type definition for generic function to remove a handler, given the handler ref.
 */
//--------------------------------------------------------------------------------------------------
typedef void(* RemoveHandlerFunc_t)(void *handlerRef);


//--------------------------------------------------------------------------------------------------
/**
 * Server Data Objects
 *
 * This object is used to store additional context info for each request
 */
//--------------------------------------------------------------------------------------------------
typedef struct
{
    le_msg_SessionRef_t   clientSessionRef;     ///< The client to send the response to
    void*                 contextPtr;           ///< ContextPtr registered with handler
    le_event_HandlerRef_t handlerRef;           ///< HandlerRef for the registered handler
    RemoveHandlerFunc_t   removeHandlerFunc;    ///< Function to remove the registered handler
}
_ServerData_t;


//--------------------------------------------------------------------------------------------------
/**
 * Expected number of simultaneous server data objects.
 */
//--------------------------------------------------------------------------------------------------
#define HIGH_SERVER_DATA_COUNT            3


//--------------------------------------------------------------------------------------------------
/**
 * Static pool for server data objects
 */
//--------------------------------------------------------------------------------------------------
LE_MEM_DEFINE_STATIC_POOL(le_sim_ServerData, HIGH_SERVER_DATA_COUNT, sizeof(_ServerData_t));

//--------------------------------------------------------------------------------------------------
/**
 * The memory pool for server data objects
 */
//--------------------------------------------------------------------------------------------------
static le_mem_PoolRef_t _ServerDataPool;


//--------------------------------------------------------------------------------------------------
/**
 *  Static safe reference map for use with Add/Remove handler references
 */
//--------------------------------------------------------------------------------------------------
LE_REF_DEFINE_STATIC_MAP(le_sim_ServerHandlers, HIGH_SERVER_DATA_COUNT);


//--------------------------------------------------------------------------------------------------
/**
 * Safe Reference Map for use with Add/Remove handler references
 *
 * @warning Use _Mutex, defined below, to protect accesses to this data.
 */
//--------------------------------------------------------------------------------------------------
static le_ref_MapRef_t _HandlerRefMap;

//--------------------------------------------------------------------------------------------------
/**
 * Mutex and associated macros for use with the above HandlerRefMap.
 *
 * Unused attribute is needed because this variable may not always get used.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((unused)) static pthread_mutex_t _Mutex = PTHREAD_MUTEX_INITIALIZER;   // POSIX "Fast" mutex.

/// Locks the mutex.
#define _LOCK    LE_ASSERT(pthread_mutex_lock(&_Mutex) == 0);

/// Unlocks the mutex.
#define _UNLOCK  LE_ASSERT(pthread_mutex_unlock(&_Mutex) == 0);


//--------------------------------------------------------------------------------------------------
/**
 * Forward declaration needed by StartServer
 */
//--------------------------------------------------------------------------------------------------
static void ServerMsgRecvHandler
(
    le_msg_MessageRef_t msgRef,
    void*               contextPtr
);


//--------------------------------------------------------------------------------------------------
/**
 * Per-server data:
 *  - Server service reference
 *  - Server thread reference
 *  - Client session reference
 */
//--------------------------------------------------------------------------------------------------
LE_CDATA_DECLARE({le_msg_ServiceRef_t _ServerServiceRef;
        le_thread_Ref_t _ServerThreadRef;
        le_msg_SessionRef_t _ClientSessionRef;});

//--------------------------------------------------------------------------------------------------
/**
 * Trace reference used for controlling tracing in this module.
 */
//--------------------------------------------------------------------------------------------------
#if defined(MK_TOOLS_BUILD) && !defined(NO_LOG_SESSION)

static le_log_TraceRef_t TraceRef;

/// Macro used to generate trace output in this module.
/// Takes the same parameters as LE_DEBUG() et. al.
#define TRACE(...) LE_TRACE(TraceRef, ##__VA_ARGS__)

/// Macro used to query current trace state in this module
#define IS_TRACE_ENABLED LE_IS_TRACE_ENABLED(TraceRef)

#else

#define TRACE(...)
#define IS_TRACE_ENABLED 0

#endif
//--------------------------------------------------------------------------------------------------
/**
 * Cleanup client data if the client is no longer connected
 */
//--------------------------------------------------------------------------------------------------
__attribute__((unused)) static void CleanupClientData
(
    le_msg_SessionRef_t sessionRef,
    void *contextPtr
)
{
    LE_DEBUG("Client %p is closed !!!", sessionRef);

    // Iterate over the server data reference map and remove anything that matches
    // the client session.
    _LOCK

    // Store the client session ref so it can be retrieved by the server using the
    // GetClientSessionRef() function, if it's needed inside handler removal functions.
    LE_CDATA_THIS->_ClientSessionRef = sessionRef;

    le_ref_IterRef_t iterRef = le_ref_GetIterator(_HandlerRefMap);
    le_result_t result = le_ref_NextNode(iterRef);
    _ServerData_t const* serverDataPtr;

    while ( result == LE_OK )
    {
        serverDataPtr =  le_ref_GetValue(iterRef);

        if ( sessionRef != serverDataPtr->clientSessionRef )
        {
            LE_DEBUG("Found session ref %p; does not match",
                     serverDataPtr->clientSessionRef);
        }
        else
        {
            LE_DEBUG("Found session ref %p; match found, so needs cleanup",
                     serverDataPtr->clientSessionRef);

            // Remove the handler, if the Remove handler functions exists.
            if ( serverDataPtr->removeHandlerFunc != NULL )
            {
                serverDataPtr->removeHandlerFunc( serverDataPtr->handlerRef );
            }

            // Release the server data block
            le_mem_Release((void*)serverDataPtr);

            // Delete the associated safeRef
            le_ref_DeleteRef( _HandlerRefMap, (void*)le_ref_GetSafeRef(iterRef) );
        }

        // Get the next value in the reference mpa
        result = le_ref_NextNode(iterRef);
    }

    // Clear the client session ref, since the event has now been processed.
    LE_CDATA_THIS->_ClientSessionRef = 0;

    _UNLOCK
}


//--------------------------------------------------------------------------------------------------
/**
 * Send the message to the client (queued version)
 *
 * This is a wrapper around le_msg_Send() with an extra parameter so that it can be used
 * with le_event_QueueFunctionToThread().
 */
//--------------------------------------------------------------------------------------------------
__attribute__((unused)) static void SendMsgToClientQueued
(
    void*  msgRef,  ///< [in] Reference to the message.
    void*  unused   ///< [in] Not used
)
{
    le_msg_Send(msgRef);
}


//--------------------------------------------------------------------------------------------------
/**
 * Send the message to the client.
 */
//--------------------------------------------------------------------------------------------------
__attribute__((unused)) static void SendMsgToClient
(
    le_msg_MessageRef_t msgRef      ///< [in] Reference to the message.
)
{
    /*
     * If called from a thread other than the server thread, queue the message onto the server
     * thread.  This is necessary to allow async response/handler functions to be called from any
     * thread, whereas messages to the client can only be sent from the server thread.
     */
    if ( le_thread_GetCurrent() != LE_CDATA_THIS->_ServerThreadRef )
    {
        le_event_QueueFunctionToThread(LE_CDATA_THIS->_ServerThreadRef,
                                       SendMsgToClientQueued,
                                       msgRef,
                                       NULL);
    }
    else
    {
        le_msg_Send(msgRef);
    }
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_sim_GetServiceRef
(
    void
)
{
    return LE_CDATA_THIS->_ServerServiceRef;
}

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_sim_GetClientSessionRef
(
    void
)
{
    return LE_CDATA_THIS->_ClientSessionRef;
}


//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_sim_AdvertiseService
(
    void
)
{
    LE_DEBUG("======= Starting Server %s ========", SERVICE_INSTANCE_NAME);

    // Get a reference to the trace keyword that is used to control tracing in this module.
#if defined(MK_TOOLS_BUILD) && !defined(NO_LOG_SESSION)
    TraceRef = le_log_GetTraceRef("ipc");
#endif

    // Create the server data pool
    _ServerDataPool = le_mem_InitStaticPool(le_sim_ServerData,
                                            HIGH_SERVER_DATA_COUNT,
                                            sizeof(_ServerData_t));

    // Create safe reference map for handler references.
    // The size of the map should be based on the number of handlers defined for the server.
    // Don't expect that to be more than 2-3, so use 3 as a reasonable guess.
    _HandlerRefMap = le_ref_InitStaticMap(le_sim_ServerHandlers, HIGH_SERVER_DATA_COUNT);

    // Start the server side of the service
    le_msg_ProtocolRef_t protocolRef;

    protocolRef = le_msg_GetProtocolRef(PROTOCOL_ID_STR, sizeof(_Message_t));
    LE_CDATA_THIS->_ServerServiceRef = le_msg_CreateService(protocolRef, SERVICE_INSTANCE_NAME);
    le_msg_SetServiceRecvHandler(LE_CDATA_THIS->_ServerServiceRef, ServerMsgRecvHandler, NULL);
    le_msg_AdvertiseService(LE_CDATA_THIS->_ServerServiceRef);

    // Register for client sessions being closed
    le_msg_AddServiceCloseHandler(LE_CDATA_THIS->_ServerServiceRef, CleanupClientData, NULL);

    // Need to keep track of the thread that is registered to provide this service.
    LE_CDATA_THIS->_ServerThreadRef = le_thread_GetCurrent();
}


//--------------------------------------------------------------------------------------------------
// Client Specific Server Code
//--------------------------------------------------------------------------------------------------


static void AsyncResponse_le_sim_AddNewStateHandler
(
    le_sim_Id_t simId,
    le_sim_States_t simState,
    void* contextPtr
)
{
    le_msg_MessageRef_t _msgRef;
    _Message_t* _msgPtr;
    _ServerData_t* serverDataPtr = (_ServerData_t*)contextPtr;

    // Will not be used if no data is sent back to client
    __attribute__((unused)) uint8_t* _msgBufPtr;

    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(serverDataPtr->clientSessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_AddNewStateHandler;
    _msgBufPtr = _msgPtr->buffer;

    // Always pack the client context pointer first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, serverDataPtr->contextPtr ))

    // Pack the input parameters
    
    LE_ASSERT(le_sim_PackId( &_msgBufPtr,
                                                  simId ));
    LE_ASSERT(le_sim_PackStates( &_msgBufPtr,
                                                  simState ));

    // Send the async response to the client
    TRACE("Sending message to client session %p : %ti bytes sent",
          serverDataPtr->clientSessionRef,
          _msgBufPtr-_msgPtr->buffer);

    SendMsgToClient(_msgRef);
}


static void Handle_le_sim_AddNewStateHandler
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    void *contextPtr = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr, &contextPtr ))
    {
        goto error_unpack;
    }

    // Create a new server data object and fill it in
    _ServerData_t* serverDataPtr = le_mem_ForceAlloc(_ServerDataPool);
    serverDataPtr->clientSessionRef = le_msg_GetSession(_msgRef);
    serverDataPtr->contextPtr = contextPtr;
    serverDataPtr->handlerRef = NULL;
    serverDataPtr->removeHandlerFunc = NULL;
    contextPtr = serverDataPtr;

    // Define storage for output parameters

    // Call the function
    le_sim_NewStateHandlerRef_t _result;
    _result  = le_sim_AddNewStateHandler ( AsyncResponse_le_sim_AddNewStateHandler, 
        contextPtr );

    if (_result)
    {
        // Put the handler reference result and a pointer to the associated remove function
        // into the server data object.  This function pointer is needed in case the client
        // is closed and the handlers need to be removed.
        serverDataPtr->handlerRef = (le_event_HandlerRef_t)_result;
        serverDataPtr->removeHandlerFunc =
            (RemoveHandlerFunc_t)le_sim_RemoveNewStateHandler;

        // Return a safe reference to the server data object as the reference.
        _LOCK
        _result = le_ref_CreateRef(_HandlerRefMap, serverDataPtr);
        _UNLOCK
    }
    else
    {
        // Adding handler failed; release serverDataPtr and return NULL back to the client.
        le_mem_Release(serverDataPtr);
    }

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_RemoveNewStateHandler
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_NewStateHandlerRef_t handlerRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                  &handlerRef ))
    {
        goto error_unpack;
    }
    // The passed in handlerRef is a safe reference for the server data object.  Need to get the
    // real handlerRef from the server data object and then delete both the safe reference and
    // the object since they are no longer needed.
    _LOCK
    _ServerData_t* serverDataPtr = le_ref_Lookup(_HandlerRefMap,
                                                 handlerRef);
    if ( serverDataPtr == NULL )
    {
        _UNLOCK
        LE_KILL_CLIENT("Invalid reference");
        return;
    }
    le_ref_DeleteRef(_HandlerRefMap, handlerRef);
    _UNLOCK
    handlerRef = (le_sim_NewStateHandlerRef_t)serverDataPtr->handlerRef;
    le_mem_Release(serverDataPtr);

    // Define storage for output parameters

    // Call the function
    le_sim_RemoveNewStateHandler ( 
        handlerRef );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void AsyncResponse_le_sim_AddProfileUpdateHandler
(
    le_sim_Id_t simId,
    le_sim_StkEvent_t stkEvent,
    void* contextPtr
)
{
    le_msg_MessageRef_t _msgRef;
    _Message_t* _msgPtr;
    _ServerData_t* serverDataPtr = (_ServerData_t*)contextPtr;

    // Will not be used if no data is sent back to client
    __attribute__((unused)) uint8_t* _msgBufPtr;

    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(serverDataPtr->clientSessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_AddProfileUpdateHandler;
    _msgBufPtr = _msgPtr->buffer;

    // Always pack the client context pointer first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, serverDataPtr->contextPtr ))

    // Pack the input parameters
    
    LE_ASSERT(le_sim_PackId( &_msgBufPtr,
                                                  simId ));
    LE_ASSERT(le_sim_PackStkEvent( &_msgBufPtr,
                                                  stkEvent ));

    // Send the async response to the client
    TRACE("Sending message to client session %p : %ti bytes sent",
          serverDataPtr->clientSessionRef,
          _msgBufPtr-_msgPtr->buffer);

    SendMsgToClient(_msgRef);
}


static void Handle_le_sim_AddProfileUpdateHandler
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    void *contextPtr = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr, &contextPtr ))
    {
        goto error_unpack;
    }

    // Create a new server data object and fill it in
    _ServerData_t* serverDataPtr = le_mem_ForceAlloc(_ServerDataPool);
    serverDataPtr->clientSessionRef = le_msg_GetSession(_msgRef);
    serverDataPtr->contextPtr = contextPtr;
    serverDataPtr->handlerRef = NULL;
    serverDataPtr->removeHandlerFunc = NULL;
    contextPtr = serverDataPtr;

    // Define storage for output parameters

    // Call the function
    le_sim_ProfileUpdateHandlerRef_t _result;
    _result  = le_sim_AddProfileUpdateHandler ( AsyncResponse_le_sim_AddProfileUpdateHandler, 
        contextPtr );

    if (_result)
    {
        // Put the handler reference result and a pointer to the associated remove function
        // into the server data object.  This function pointer is needed in case the client
        // is closed and the handlers need to be removed.
        serverDataPtr->handlerRef = (le_event_HandlerRef_t)_result;
        serverDataPtr->removeHandlerFunc =
            (RemoveHandlerFunc_t)le_sim_RemoveProfileUpdateHandler;

        // Return a safe reference to the server data object as the reference.
        _LOCK
        _result = le_ref_CreateRef(_HandlerRefMap, serverDataPtr);
        _UNLOCK
    }
    else
    {
        // Adding handler failed; release serverDataPtr and return NULL back to the client.
        le_mem_Release(serverDataPtr);
    }

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_RemoveProfileUpdateHandler
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_ProfileUpdateHandlerRef_t handlerRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                  &handlerRef ))
    {
        goto error_unpack;
    }
    // The passed in handlerRef is a safe reference for the server data object.  Need to get the
    // real handlerRef from the server data object and then delete both the safe reference and
    // the object since they are no longer needed.
    _LOCK
    _ServerData_t* serverDataPtr = le_ref_Lookup(_HandlerRefMap,
                                                 handlerRef);
    if ( serverDataPtr == NULL )
    {
        _UNLOCK
        LE_KILL_CLIENT("Invalid reference");
        return;
    }
    le_ref_DeleteRef(_HandlerRefMap, handlerRef);
    _UNLOCK
    handlerRef = (le_sim_ProfileUpdateHandlerRef_t)serverDataPtr->handlerRef;
    le_mem_Release(serverDataPtr);

    // Define storage for output parameters

    // Call the function
    le_sim_RemoveProfileUpdateHandler ( 
        handlerRef );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void AsyncResponse_le_sim_AddSimToolkitEventHandler
(
    le_sim_Id_t simId,
    le_sim_StkEvent_t stkEvent,
    void* contextPtr
)
{
    le_msg_MessageRef_t _msgRef;
    _Message_t* _msgPtr;
    _ServerData_t* serverDataPtr = (_ServerData_t*)contextPtr;

    // Will not be used if no data is sent back to client
    __attribute__((unused)) uint8_t* _msgBufPtr;

    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(serverDataPtr->clientSessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_AddSimToolkitEventHandler;
    _msgBufPtr = _msgPtr->buffer;

    // Always pack the client context pointer first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, serverDataPtr->contextPtr ))

    // Pack the input parameters
    
    LE_ASSERT(le_sim_PackId( &_msgBufPtr,
                                                  simId ));
    LE_ASSERT(le_sim_PackStkEvent( &_msgBufPtr,
                                                  stkEvent ));

    // Send the async response to the client
    TRACE("Sending message to client session %p : %ti bytes sent",
          serverDataPtr->clientSessionRef,
          _msgBufPtr-_msgPtr->buffer);

    SendMsgToClient(_msgRef);
}


static void Handle_le_sim_AddSimToolkitEventHandler
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    void *contextPtr = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr, &contextPtr ))
    {
        goto error_unpack;
    }

    // Create a new server data object and fill it in
    _ServerData_t* serverDataPtr = le_mem_ForceAlloc(_ServerDataPool);
    serverDataPtr->clientSessionRef = le_msg_GetSession(_msgRef);
    serverDataPtr->contextPtr = contextPtr;
    serverDataPtr->handlerRef = NULL;
    serverDataPtr->removeHandlerFunc = NULL;
    contextPtr = serverDataPtr;

    // Define storage for output parameters

    // Call the function
    le_sim_SimToolkitEventHandlerRef_t _result;
    _result  = le_sim_AddSimToolkitEventHandler ( AsyncResponse_le_sim_AddSimToolkitEventHandler, 
        contextPtr );

    if (_result)
    {
        // Put the handler reference result and a pointer to the associated remove function
        // into the server data object.  This function pointer is needed in case the client
        // is closed and the handlers need to be removed.
        serverDataPtr->handlerRef = (le_event_HandlerRef_t)_result;
        serverDataPtr->removeHandlerFunc =
            (RemoveHandlerFunc_t)le_sim_RemoveSimToolkitEventHandler;

        // Return a safe reference to the server data object as the reference.
        _LOCK
        _result = le_ref_CreateRef(_HandlerRefMap, serverDataPtr);
        _UNLOCK
    }
    else
    {
        // Adding handler failed; release serverDataPtr and return NULL back to the client.
        le_mem_Release(serverDataPtr);
    }

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_RemoveSimToolkitEventHandler
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_SimToolkitEventHandlerRef_t handlerRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                  &handlerRef ))
    {
        goto error_unpack;
    }
    // The passed in handlerRef is a safe reference for the server data object.  Need to get the
    // real handlerRef from the server data object and then delete both the safe reference and
    // the object since they are no longer needed.
    _LOCK
    _ServerData_t* serverDataPtr = le_ref_Lookup(_HandlerRefMap,
                                                 handlerRef);
    if ( serverDataPtr == NULL )
    {
        _UNLOCK
        LE_KILL_CLIENT("Invalid reference");
        return;
    }
    le_ref_DeleteRef(_HandlerRefMap, handlerRef);
    _UNLOCK
    handlerRef = (le_sim_SimToolkitEventHandlerRef_t)serverDataPtr->handlerRef;
    le_mem_Release(serverDataPtr);

    // Define storage for output parameters

    // Call the function
    le_sim_RemoveSimToolkitEventHandler ( 
        handlerRef );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void AsyncResponse_le_sim_AddIccidChangeHandler
(
    le_sim_Id_t simId,
    const char* LE_NONNULL iccid,
    void* contextPtr
)
{
    le_msg_MessageRef_t _msgRef;
    _Message_t* _msgPtr;
    _ServerData_t* serverDataPtr = (_ServerData_t*)contextPtr;

    // Will not be used if no data is sent back to client
    __attribute__((unused)) uint8_t* _msgBufPtr;

    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(serverDataPtr->clientSessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_sim_AddIccidChangeHandler;
    _msgBufPtr = _msgPtr->buffer;

    // Always pack the client context pointer first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, serverDataPtr->contextPtr ))

    // Pack the input parameters
    
    LE_ASSERT(le_sim_PackId( &_msgBufPtr,
                                                  simId ));
    LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                  iccid, 20 ));

    // Send the async response to the client
    TRACE("Sending message to client session %p : %ti bytes sent",
          serverDataPtr->clientSessionRef,
          _msgBufPtr-_msgPtr->buffer);

    SendMsgToClient(_msgRef);
}


static void Handle_le_sim_AddIccidChangeHandler
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    void *contextPtr = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr, &contextPtr ))
    {
        goto error_unpack;
    }

    // Create a new server data object and fill it in
    _ServerData_t* serverDataPtr = le_mem_ForceAlloc(_ServerDataPool);
    serverDataPtr->clientSessionRef = le_msg_GetSession(_msgRef);
    serverDataPtr->contextPtr = contextPtr;
    serverDataPtr->handlerRef = NULL;
    serverDataPtr->removeHandlerFunc = NULL;
    contextPtr = serverDataPtr;

    // Define storage for output parameters

    // Call the function
    le_sim_IccidChangeHandlerRef_t _result;
    _result  = le_sim_AddIccidChangeHandler ( AsyncResponse_le_sim_AddIccidChangeHandler, 
        contextPtr );

    if (_result)
    {
        // Put the handler reference result and a pointer to the associated remove function
        // into the server data object.  This function pointer is needed in case the client
        // is closed and the handlers need to be removed.
        serverDataPtr->handlerRef = (le_event_HandlerRef_t)_result;
        serverDataPtr->removeHandlerFunc =
            (RemoveHandlerFunc_t)le_sim_RemoveIccidChangeHandler;

        // Return a safe reference to the server data object as the reference.
        _LOCK
        _result = le_ref_CreateRef(_HandlerRefMap, serverDataPtr);
        _UNLOCK
    }
    else
    {
        // Adding handler failed; release serverDataPtr and return NULL back to the client.
        le_mem_Release(serverDataPtr);
    }

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_RemoveIccidChangeHandler
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_IccidChangeHandlerRef_t handlerRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                  &handlerRef ))
    {
        goto error_unpack;
    }
    // The passed in handlerRef is a safe reference for the server data object.  Need to get the
    // real handlerRef from the server data object and then delete both the safe reference and
    // the object since they are no longer needed.
    _LOCK
    _ServerData_t* serverDataPtr = le_ref_Lookup(_HandlerRefMap,
                                                 handlerRef);
    if ( serverDataPtr == NULL )
    {
        _UNLOCK
        LE_KILL_CLIENT("Invalid reference");
        return;
    }
    le_ref_DeleteRef(_HandlerRefMap, handlerRef);
    _UNLOCK
    handlerRef = (le_sim_IccidChangeHandlerRef_t)serverDataPtr->handlerRef;
    le_mem_Release(serverDataPtr);

    // Define storage for output parameters

    // Call the function
    le_sim_RemoveIccidChangeHandler ( 
        handlerRef );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetSelectedCard
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message

    // Define storage for output parameters

    // Call the function
    le_sim_Id_t _result;
    _result  = le_sim_GetSelectedCard (  );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_sim_PackId( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;
}


static void Handle_le_sim_SelectCard
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_SelectCard ( 
        simId );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetICCID
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    size_t iccidSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &iccidSize ))
    {
        goto error_unpack;
    }
    if ( (iccidSize > 20) &&
         (iccidSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting iccidSize from %" PRIuS " to 20", iccidSize);
        iccidSize = 20;
    }
    if (iccidSize >= UINT32_MAX)
    {
        iccidSize = UINT32_MAX;
    }
    else
    {
        iccidSize++;
    }

    // Define storage for output parameters
    char iccidBuffer[21] = { 0 };
    char *iccid = iccidBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        iccid = NULL;
        iccidSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetICCID ( 
        simId, 
        iccid, 
        iccidSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (iccid)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      iccid, 20 ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetEID
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    size_t eidSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &eidSize ))
    {
        goto error_unpack;
    }
    if ( (eidSize > 32) &&
         (eidSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting eidSize from %" PRIuS " to 32", eidSize);
        eidSize = 32;
    }
    if (eidSize >= UINT32_MAX)
    {
        eidSize = UINT32_MAX;
    }
    else
    {
        eidSize++;
    }

    // Define storage for output parameters
    char eidBuffer[33] = { 0 };
    char *eid = eidBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        eid = NULL;
        eidSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetEID ( 
        simId, 
        eid, 
        eidSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (eid)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      eid, 32 ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetIMSI
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    size_t imsiSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &imsiSize ))
    {
        goto error_unpack;
    }
    if ( (imsiSize > 15) &&
         (imsiSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting imsiSize from %" PRIuS " to 15", imsiSize);
        imsiSize = 15;
    }
    if (imsiSize >= UINT32_MAX)
    {
        imsiSize = UINT32_MAX;
    }
    else
    {
        imsiSize++;
    }

    // Define storage for output parameters
    char imsiBuffer[16] = { 0 };
    char *imsi = imsiBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        imsi = NULL;
        imsiSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetIMSI ( 
        simId, 
        imsi, 
        imsiSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (imsi)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      imsi, 15 ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_IsPresent
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    bool _result;
    _result  = le_sim_IsPresent ( 
        simId );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackBool( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_IsReady
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    bool _result;
    _result  = le_sim_IsReady ( 
        simId );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackBool( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_EnterPIN
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    char pin[9] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               pin,
                               sizeof(pin),
                               8 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_EnterPIN ( 
        simId, 
        pin );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_ChangePIN
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    char oldpin[9] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               oldpin,
                               sizeof(oldpin),
                               8 ))
    {
        goto error_unpack;
    }
    char newpin[9] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               newpin,
                               sizeof(newpin),
                               8 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_ChangePIN ( 
        simId, 
        oldpin, 
        newpin );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetRemainingPINTries
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    int32_t _result;
    _result  = le_sim_GetRemainingPINTries ( 
        simId );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackInt32( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetRemainingPUKTries
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters
    uint32_t remainingPukTriesPtrBuffer = 0;
    uint32_t *remainingPukTriesPtrPtr = &remainingPukTriesPtrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        remainingPukTriesPtrPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetRemainingPUKTries ( 
        simId, 
        remainingPukTriesPtrPtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (remainingPukTriesPtrPtr)
    {
        LE_ASSERT(le_pack_PackUint32( &_msgBufPtr,
                                                      *remainingPukTriesPtrPtr ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_Unlock
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    char pin[9] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               pin,
                               sizeof(pin),
                               8 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_Unlock ( 
        simId, 
        pin );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_Lock
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    char pin[9] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               pin,
                               sizeof(pin),
                               8 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_Lock ( 
        simId, 
        pin );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_Unblock
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    char puk[9] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               puk,
                               sizeof(puk),
                               8 ))
    {
        goto error_unpack;
    }
    char newpin[9] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               newpin,
                               sizeof(newpin),
                               8 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_Unblock ( 
        simId, 
        puk, 
        newpin );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetState
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_sim_States_t _result;
    _result  = le_sim_GetState ( 
        simId );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_sim_PackStates( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetSubscriberPhoneNumber
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    size_t phoneNumberStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &phoneNumberStrSize ))
    {
        goto error_unpack;
    }
    if ( (phoneNumberStrSize > 17) &&
         (phoneNumberStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting phoneNumberStrSize from %" PRIuS " to 17", phoneNumberStrSize);
        phoneNumberStrSize = 17;
    }
    if (phoneNumberStrSize >= UINT32_MAX)
    {
        phoneNumberStrSize = UINT32_MAX;
    }
    else
    {
        phoneNumberStrSize++;
    }

    // Define storage for output parameters
    char phoneNumberStrBuffer[18] = { 0 };
    char *phoneNumberStr = phoneNumberStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        phoneNumberStr = NULL;
        phoneNumberStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetSubscriberPhoneNumber ( 
        simId, 
        phoneNumberStr, 
        phoneNumberStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (phoneNumberStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      phoneNumberStr, 17 ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetHomeNetworkOperator
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    size_t nameStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &nameStrSize ))
    {
        goto error_unpack;
    }
    if ( (nameStrSize > 100) &&
         (nameStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting nameStrSize from %" PRIuS " to 100", nameStrSize);
        nameStrSize = 100;
    }
    if (nameStrSize >= UINT32_MAX)
    {
        nameStrSize = UINT32_MAX;
    }
    else
    {
        nameStrSize++;
    }

    // Define storage for output parameters
    char nameStrBuffer[101] = { 0 };
    char *nameStr = nameStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        nameStr = NULL;
        nameStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetHomeNetworkOperator ( 
        simId, 
        nameStr, 
        nameStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (nameStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      nameStr, 100 ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetHomeNetworkMccMnc
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    size_t mccPtrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &mccPtrSize ))
    {
        goto error_unpack;
    }
    if ( (mccPtrSize > 3) &&
         (mccPtrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting mccPtrSize from %" PRIuS " to 3", mccPtrSize);
        mccPtrSize = 3;
    }
    if (mccPtrSize >= UINT32_MAX)
    {
        mccPtrSize = UINT32_MAX;
    }
    else
    {
        mccPtrSize++;
    }
    size_t mncPtrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &mncPtrSize ))
    {
        goto error_unpack;
    }
    if ( (mncPtrSize > 3) &&
         (mncPtrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting mncPtrSize from %" PRIuS " to 3", mncPtrSize);
        mncPtrSize = 3;
    }
    if (mncPtrSize >= UINT32_MAX)
    {
        mncPtrSize = UINT32_MAX;
    }
    else
    {
        mncPtrSize++;
    }

    // Define storage for output parameters
    char mccPtrBuffer[4] = { 0 };
    char *mccPtr = mccPtrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        mccPtr = NULL;
        mccPtrSize = 0;
    }
    char mncPtrBuffer[4] = { 0 };
    char *mncPtr = mncPtrBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        mncPtr = NULL;
        mncPtrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetHomeNetworkMccMnc ( 
        simId, 
        mccPtr, 
        mccPtrSize, 
        mncPtr, 
        mncPtrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (mccPtr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      mccPtr, 3 ));
    }
    if (mncPtr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      mncPtr, 3 ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_LocalSwapToEmergencyCallSubscription
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    le_sim_Manufacturer_t manufacturer = (le_sim_Manufacturer_t) 0;
    if (!le_sim_UnpackManufacturer( &_msgBufPtr,
                                               &manufacturer ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_LocalSwapToEmergencyCallSubscription ( 
        simId, 
        manufacturer );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_LocalSwapToCommercialSubscription
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    le_sim_Manufacturer_t manufacturer = (le_sim_Manufacturer_t) 0;
    if (!le_sim_UnpackManufacturer( &_msgBufPtr,
                                               &manufacturer ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_LocalSwapToCommercialSubscription ( 
        simId, 
        manufacturer );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_IsEmergencyCallSubscriptionSelected
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters
    bool isEcsBuffer = false;
    bool *isEcsPtr = &isEcsBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        isEcsPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_IsEmergencyCallSubscriptionSelected ( 
        simId, 
        isEcsPtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (isEcsPtr)
    {
        LE_ASSERT(le_pack_PackBool( &_msgBufPtr,
                                                      *isEcsPtr ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_AcceptSimToolkitCommand
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_AcceptSimToolkitCommand ( 
        simId );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_RejectSimToolkitCommand
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_RejectSimToolkitCommand ( 
        simId );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetSimToolkitRefreshMode
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters
    le_sim_StkRefreshMode_t refreshModeBuffer = (le_sim_StkRefreshMode_t) 0;
    le_sim_StkRefreshMode_t *refreshModePtr = &refreshModeBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        refreshModePtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetSimToolkitRefreshMode ( 
        simId, 
        refreshModePtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (refreshModePtr)
    {
        LE_ASSERT(le_sim_PackStkRefreshMode( &_msgBufPtr,
                                                      *refreshModePtr ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetSimToolkitRefreshStage
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters
    le_sim_StkRefreshStage_t refreshStageBuffer = (le_sim_StkRefreshStage_t) 0;
    le_sim_StkRefreshStage_t *refreshStagePtr = &refreshStageBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        refreshStagePtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetSimToolkitRefreshStage ( 
        simId, 
        refreshStagePtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (refreshStagePtr)
    {
        LE_ASSERT(le_sim_PackStkRefreshStage( &_msgBufPtr,
                                                      *refreshStagePtr ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_SendApdu
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    size_t commandApduSize = 0;
    uint8_t
        commandApduPtr[255]
            = { 0 };
    bool commandApduResult;
            LE_PACK_UNPACKARRAY( &_msgBufPtr,
                         commandApduPtr, &commandApduSize,
                         255,
                         le_pack_UnpackUint8,
                         &commandApduResult );
    if (!commandApduResult)
    {
        goto error_unpack;
    }
    size_t responseApduSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &responseApduSize ))
    {
        goto error_unpack;
    }
    if ( (responseApduSize > 256) )
    {
        LE_DEBUG("Adjusting responseApduSize from %" PRIuS " to 256", responseApduSize);
        responseApduSize = 256;
    }

    // Define storage for output parameters
    uint8_t responseApduBuffer[256] = { 0 };
    uint8_t *responseApduPtr = responseApduBuffer;
    size_t *responseApduSizePtr = &responseApduSize;
    if (!(_requiredOutputs & (1u << 0)))
    {
        responseApduPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_SendApdu ( 
        simId, 
        commandApduPtr, 
        commandApduSize, 
        responseApduPtr, 
        &responseApduSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (responseApduPtr)
    {
        bool responseApduResult;
            LE_PACK_PACKARRAY( &_msgBufPtr,
                           responseApduPtr, (*responseApduSizePtr),
                           256, le_pack_PackUint8,
                           &responseApduResult );
        LE_ASSERT(responseApduResult);
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_SendCommand
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    le_sim_Command_t command = (le_sim_Command_t) 0;
    if (!le_sim_UnpackCommand( &_msgBufPtr,
                                               &command ))
    {
        goto error_unpack;
    }
    char fileIdentifier[5] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               fileIdentifier,
                               sizeof(fileIdentifier),
                               4 ))
    {
        goto error_unpack;
    }
    uint8_t p1 = 0;
    if (!le_pack_UnpackUint8( &_msgBufPtr,
                                               &p1 ))
    {
        goto error_unpack;
    }
    uint8_t p2 = 0;
    if (!le_pack_UnpackUint8( &_msgBufPtr,
                                               &p2 ))
    {
        goto error_unpack;
    }
    uint8_t p3 = 0;
    if (!le_pack_UnpackUint8( &_msgBufPtr,
                                               &p3 ))
    {
        goto error_unpack;
    }
    size_t dataSize = 0;
    uint8_t
        dataPtr[100]
            = { 0 };
    bool dataResult;
            LE_PACK_UNPACKARRAY( &_msgBufPtr,
                         dataPtr, &dataSize,
                         100,
                         le_pack_UnpackUint8,
                         &dataResult );
    if (!dataResult)
    {
        goto error_unpack;
    }
    char path[101] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               path,
                               sizeof(path),
                               100 ))
    {
        goto error_unpack;
    }
    size_t responseSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &responseSize ))
    {
        goto error_unpack;
    }
    if ( (responseSize > 256) )
    {
        LE_DEBUG("Adjusting responseSize from %" PRIuS " to 256", responseSize);
        responseSize = 256;
    }

    // Define storage for output parameters
    uint8_t sw1Buffer = 0;
    uint8_t *sw1Ptr = &sw1Buffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        sw1Ptr = NULL;
    }
    uint8_t sw2Buffer = 0;
    uint8_t *sw2Ptr = &sw2Buffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        sw2Ptr = NULL;
    }
    uint8_t responseBuffer[256] = { 0 };
    uint8_t *responsePtr = responseBuffer;
    size_t *responseSizePtr = &responseSize;
    if (!(_requiredOutputs & (1u << 2)))
    {
        responsePtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_SendCommand ( 
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
        &responseSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (sw1Ptr)
    {
        LE_ASSERT(le_pack_PackUint8( &_msgBufPtr,
                                                      *sw1Ptr ));
    }
    if (sw2Ptr)
    {
        LE_ASSERT(le_pack_PackUint8( &_msgBufPtr,
                                                      *sw2Ptr ));
    }
    if (responsePtr)
    {
        bool responseResult;
            LE_PACK_PACKARRAY( &_msgBufPtr,
                           responsePtr, (*responseSizePtr),
                           256, le_pack_PackUint8,
                           &responseResult );
        LE_ASSERT(responseResult);
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_SetAutomaticSelection
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    bool enable = false;
    if (!le_pack_UnpackBool( &_msgBufPtr,
                                               &enable ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_SetAutomaticSelection ( 
        enable );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetAutomaticSelection
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message

    // Define storage for output parameters
    bool enableBuffer = false;
    bool *enablePtr = &enableBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        enablePtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetAutomaticSelection ( 
        enablePtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (enablePtr)
    {
        LE_ASSERT(le_pack_PackBool( &_msgBufPtr,
                                                      *enablePtr ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_Reset
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_Reset ( 
        simId );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_CreateFPLMNList
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message

    // Define storage for output parameters

    // Call the function
    le_sim_FPLMNListRef_t _result;
    _result  = le_sim_CreateFPLMNList (  );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;
}


static void Handle_le_sim_AddFPLMNOperator
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_FPLMNListRef_t FPLMNListRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &FPLMNListRef ))
    {
        goto error_unpack;
    }
    char mcc[4] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               mcc,
                               sizeof(mcc),
                               3 ))
    {
        goto error_unpack;
    }
    char mnc[4] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               mnc,
                               sizeof(mnc),
                               3 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_AddFPLMNOperator ( 
        FPLMNListRef, 
        mcc, 
        mnc );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_WriteFPLMNList
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    le_sim_FPLMNListRef_t FPLMNListRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &FPLMNListRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_WriteFPLMNList ( 
        simId, 
        FPLMNListRef );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_ReadFPLMNList
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_sim_FPLMNListRef_t _result;
    _result  = le_sim_ReadFPLMNList ( 
        simId );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetFirstFPLMNOperator
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_FPLMNListRef_t FPLMNListRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &FPLMNListRef ))
    {
        goto error_unpack;
    }
    size_t mccPtrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &mccPtrSize ))
    {
        goto error_unpack;
    }
    if ( (mccPtrSize > 3) &&
         (mccPtrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting mccPtrSize from %" PRIuS " to 3", mccPtrSize);
        mccPtrSize = 3;
    }
    if (mccPtrSize >= UINT32_MAX)
    {
        mccPtrSize = UINT32_MAX;
    }
    else
    {
        mccPtrSize++;
    }
    size_t mncPtrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &mncPtrSize ))
    {
        goto error_unpack;
    }
    if ( (mncPtrSize > 3) &&
         (mncPtrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting mncPtrSize from %" PRIuS " to 3", mncPtrSize);
        mncPtrSize = 3;
    }
    if (mncPtrSize >= UINT32_MAX)
    {
        mncPtrSize = UINT32_MAX;
    }
    else
    {
        mncPtrSize++;
    }

    // Define storage for output parameters
    char mccPtrBuffer[4] = { 0 };
    char *mccPtr = mccPtrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        mccPtr = NULL;
        mccPtrSize = 0;
    }
    char mncPtrBuffer[4] = { 0 };
    char *mncPtr = mncPtrBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        mncPtr = NULL;
        mncPtrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetFirstFPLMNOperator ( 
        FPLMNListRef, 
        mccPtr, 
        mccPtrSize, 
        mncPtr, 
        mncPtrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (mccPtr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      mccPtr, 3 ));
    }
    if (mncPtr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      mncPtr, 3 ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_GetNextFPLMNOperator
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_FPLMNListRef_t FPLMNListRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &FPLMNListRef ))
    {
        goto error_unpack;
    }
    size_t mccPtrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &mccPtrSize ))
    {
        goto error_unpack;
    }
    if ( (mccPtrSize > 3) &&
         (mccPtrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting mccPtrSize from %" PRIuS " to 3", mccPtrSize);
        mccPtrSize = 3;
    }
    if (mccPtrSize >= UINT32_MAX)
    {
        mccPtrSize = UINT32_MAX;
    }
    else
    {
        mccPtrSize++;
    }
    size_t mncPtrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &mncPtrSize ))
    {
        goto error_unpack;
    }
    if ( (mncPtrSize > 3) &&
         (mncPtrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting mncPtrSize from %" PRIuS " to 3", mncPtrSize);
        mncPtrSize = 3;
    }
    if (mncPtrSize >= UINT32_MAX)
    {
        mncPtrSize = UINT32_MAX;
    }
    else
    {
        mncPtrSize++;
    }

    // Define storage for output parameters
    char mccPtrBuffer[4] = { 0 };
    char *mccPtr = mccPtrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        mccPtr = NULL;
        mccPtrSize = 0;
    }
    char mncPtrBuffer[4] = { 0 };
    char *mncPtr = mncPtrBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        mncPtr = NULL;
        mncPtrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_GetNextFPLMNOperator ( 
        FPLMNListRef, 
        mccPtr, 
        mccPtrSize, 
        mncPtr, 
        mncPtrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (mccPtr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      mccPtr, 3 ));
    }
    if (mncPtr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      mncPtr, 3 ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_DeleteFPLMNList
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_FPLMNListRef_t FPLMNListRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &FPLMNListRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_sim_DeleteFPLMNList ( 
        FPLMNListRef );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_OpenLogicalChannel
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message

    // Define storage for output parameters
    uint8_t channelPtrBuffer = 0;
    uint8_t *channelPtrPtr = &channelPtrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        channelPtrPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_OpenLogicalChannel ( 
        channelPtrPtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (channelPtrPtr)
    {
        LE_ASSERT(le_pack_PackUint8( &_msgBufPtr,
                                                      *channelPtrPtr ));
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_CloseLogicalChannel
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    uint8_t channel = 0;
    if (!le_pack_UnpackUint8( &_msgBufPtr,
                                               &channel ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_CloseLogicalChannel ( 
        channel );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_SendApduOnChannel
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed
    uint32_t _requiredOutputs = 0;
    if (!le_pack_UnpackUint32(&_msgBufPtr, &_requiredOutputs))
    {
        goto error_unpack;
    }

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    uint8_t channel = 0;
    if (!le_pack_UnpackUint8( &_msgBufPtr,
                                               &channel ))
    {
        goto error_unpack;
    }
    size_t commandApduSize = 0;
    uint8_t
        commandApduPtr[255]
            = { 0 };
    bool commandApduResult;
            LE_PACK_UNPACKARRAY( &_msgBufPtr,
                         commandApduPtr, &commandApduSize,
                         255,
                         le_pack_UnpackUint8,
                         &commandApduResult );
    if (!commandApduResult)
    {
        goto error_unpack;
    }
    size_t responseApduSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &responseApduSize ))
    {
        goto error_unpack;
    }
    if ( (responseApduSize > 256) )
    {
        LE_DEBUG("Adjusting responseApduSize from %" PRIuS " to 256", responseApduSize);
        responseApduSize = 256;
    }

    // Define storage for output parameters
    uint8_t responseApduBuffer[256] = { 0 };
    uint8_t *responseApduPtr = responseApduBuffer;
    size_t *responseApduSizePtr = &responseApduSize;
    if (!(_requiredOutputs & (1u << 0)))
    {
        responseApduPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_sim_SendApduOnChannel ( 
        simId, 
        channel, 
        commandApduPtr, 
        commandApduSize, 
        responseApduPtr, 
        &responseApduSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (responseApduPtr)
    {
        bool responseApduResult;
            LE_PACK_PACKARRAY( &_msgBufPtr,
                           responseApduPtr, (*responseApduSizePtr),
                           256, le_pack_PackUint8,
                           &responseApduResult );
        LE_ASSERT(responseApduResult);
    }

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void Handle_le_sim_SetPower
(
    le_msg_MessageRef_t _msgRef

)
{
    // Get the message buffer pointer
    __attribute__((unused)) uint8_t* _msgBufPtr =
        ((_Message_t*)le_msg_GetPayloadPtr(_msgRef))->buffer;

    // Needed if we are returning a result or output values
    uint8_t* _msgBufStartPtr = _msgBufPtr;

    // Unpack which outputs are needed

    // Unpack the input parameters from the message
    le_sim_Id_t simId = (le_sim_Id_t) 0;
    if (!le_sim_UnpackId( &_msgBufPtr,
                                               &simId ))
    {
        goto error_unpack;
    }
    le_onoff_t power = LE_OFF;
    if (!le_pack_UnpackOnOff( &_msgBufPtr,
                                               &power ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_sim_SetPower ( 
        simId, 
        power );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;

error_unpack:
    LE_KILL_CLIENT("Error unpacking message");
}


static void ServerMsgRecvHandler
(
    le_msg_MessageRef_t msgRef,
    void*               contextPtr
)
{
    // Get the message payload so that we can get the message "id"
    _Message_t* msgPtr = le_msg_GetPayloadPtr(msgRef);

    // Get the client session ref for the current message.  This ref is used by the server to
    // get info about the client process, such as user id.  If there are multiple clients, then
    // the session ref may be different for each message, hence it has to be queried each time.
    LE_CDATA_THIS->_ClientSessionRef = le_msg_GetSession(msgRef);

    // Dispatch to appropriate message handler and get response
    switch (msgPtr->id)
    {
        case _MSGID_le_sim_AddNewStateHandler :
            Handle_le_sim_AddNewStateHandler(msgRef);
            break;
        case _MSGID_le_sim_RemoveNewStateHandler :
            Handle_le_sim_RemoveNewStateHandler(msgRef);
            break;
        case _MSGID_le_sim_AddProfileUpdateHandler :
            Handle_le_sim_AddProfileUpdateHandler(msgRef);
            break;
        case _MSGID_le_sim_RemoveProfileUpdateHandler :
            Handle_le_sim_RemoveProfileUpdateHandler(msgRef);
            break;
        case _MSGID_le_sim_AddSimToolkitEventHandler :
            Handle_le_sim_AddSimToolkitEventHandler(msgRef);
            break;
        case _MSGID_le_sim_RemoveSimToolkitEventHandler :
            Handle_le_sim_RemoveSimToolkitEventHandler(msgRef);
            break;
        case _MSGID_le_sim_AddIccidChangeHandler :
            Handle_le_sim_AddIccidChangeHandler(msgRef);
            break;
        case _MSGID_le_sim_RemoveIccidChangeHandler :
            Handle_le_sim_RemoveIccidChangeHandler(msgRef);
            break;
        case _MSGID_le_sim_GetSelectedCard :
            Handle_le_sim_GetSelectedCard(msgRef);
            break;
        case _MSGID_le_sim_SelectCard :
            Handle_le_sim_SelectCard(msgRef);
            break;
        case _MSGID_le_sim_GetICCID :
            Handle_le_sim_GetICCID(msgRef);
            break;
        case _MSGID_le_sim_GetEID :
            Handle_le_sim_GetEID(msgRef);
            break;
        case _MSGID_le_sim_GetIMSI :
            Handle_le_sim_GetIMSI(msgRef);
            break;
        case _MSGID_le_sim_IsPresent :
            Handle_le_sim_IsPresent(msgRef);
            break;
        case _MSGID_le_sim_IsReady :
            Handle_le_sim_IsReady(msgRef);
            break;
        case _MSGID_le_sim_EnterPIN :
            Handle_le_sim_EnterPIN(msgRef);
            break;
        case _MSGID_le_sim_ChangePIN :
            Handle_le_sim_ChangePIN(msgRef);
            break;
        case _MSGID_le_sim_GetRemainingPINTries :
            Handle_le_sim_GetRemainingPINTries(msgRef);
            break;
        case _MSGID_le_sim_GetRemainingPUKTries :
            Handle_le_sim_GetRemainingPUKTries(msgRef);
            break;
        case _MSGID_le_sim_Unlock :
            Handle_le_sim_Unlock(msgRef);
            break;
        case _MSGID_le_sim_Lock :
            Handle_le_sim_Lock(msgRef);
            break;
        case _MSGID_le_sim_Unblock :
            Handle_le_sim_Unblock(msgRef);
            break;
        case _MSGID_le_sim_GetState :
            Handle_le_sim_GetState(msgRef);
            break;
        case _MSGID_le_sim_GetSubscriberPhoneNumber :
            Handle_le_sim_GetSubscriberPhoneNumber(msgRef);
            break;
        case _MSGID_le_sim_GetHomeNetworkOperator :
            Handle_le_sim_GetHomeNetworkOperator(msgRef);
            break;
        case _MSGID_le_sim_GetHomeNetworkMccMnc :
            Handle_le_sim_GetHomeNetworkMccMnc(msgRef);
            break;
        case _MSGID_le_sim_LocalSwapToEmergencyCallSubscription :
            Handle_le_sim_LocalSwapToEmergencyCallSubscription(msgRef);
            break;
        case _MSGID_le_sim_LocalSwapToCommercialSubscription :
            Handle_le_sim_LocalSwapToCommercialSubscription(msgRef);
            break;
        case _MSGID_le_sim_IsEmergencyCallSubscriptionSelected :
            Handle_le_sim_IsEmergencyCallSubscriptionSelected(msgRef);
            break;
        case _MSGID_le_sim_AcceptSimToolkitCommand :
            Handle_le_sim_AcceptSimToolkitCommand(msgRef);
            break;
        case _MSGID_le_sim_RejectSimToolkitCommand :
            Handle_le_sim_RejectSimToolkitCommand(msgRef);
            break;
        case _MSGID_le_sim_GetSimToolkitRefreshMode :
            Handle_le_sim_GetSimToolkitRefreshMode(msgRef);
            break;
        case _MSGID_le_sim_GetSimToolkitRefreshStage :
            Handle_le_sim_GetSimToolkitRefreshStage(msgRef);
            break;
        case _MSGID_le_sim_SendApdu :
            Handle_le_sim_SendApdu(msgRef);
            break;
        case _MSGID_le_sim_SendCommand :
            Handle_le_sim_SendCommand(msgRef);
            break;
        case _MSGID_le_sim_SetAutomaticSelection :
            Handle_le_sim_SetAutomaticSelection(msgRef);
            break;
        case _MSGID_le_sim_GetAutomaticSelection :
            Handle_le_sim_GetAutomaticSelection(msgRef);
            break;
        case _MSGID_le_sim_Reset :
            Handle_le_sim_Reset(msgRef);
            break;
        case _MSGID_le_sim_CreateFPLMNList :
            Handle_le_sim_CreateFPLMNList(msgRef);
            break;
        case _MSGID_le_sim_AddFPLMNOperator :
            Handle_le_sim_AddFPLMNOperator(msgRef);
            break;
        case _MSGID_le_sim_WriteFPLMNList :
            Handle_le_sim_WriteFPLMNList(msgRef);
            break;
        case _MSGID_le_sim_ReadFPLMNList :
            Handle_le_sim_ReadFPLMNList(msgRef);
            break;
        case _MSGID_le_sim_GetFirstFPLMNOperator :
            Handle_le_sim_GetFirstFPLMNOperator(msgRef);
            break;
        case _MSGID_le_sim_GetNextFPLMNOperator :
            Handle_le_sim_GetNextFPLMNOperator(msgRef);
            break;
        case _MSGID_le_sim_DeleteFPLMNList :
            Handle_le_sim_DeleteFPLMNList(msgRef);
            break;
        case _MSGID_le_sim_OpenLogicalChannel :
            Handle_le_sim_OpenLogicalChannel(msgRef);
            break;
        case _MSGID_le_sim_CloseLogicalChannel :
            Handle_le_sim_CloseLogicalChannel(msgRef);
            break;
        case _MSGID_le_sim_SendApduOnChannel :
            Handle_le_sim_SendApduOnChannel(msgRef);
            break;
        case _MSGID_le_sim_SetPower :
            Handle_le_sim_SetPower(msgRef);
            break;

        default: LE_ERROR("Unknowm msg id = %" PRIu32 , msgPtr->id);
    }

    // Clear the client session ref associated with the current message, since the message
    // has now been processed.
    LE_CDATA_THIS->_ClientSessionRef = 0;
}