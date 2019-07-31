
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#include "le_mdc_server.h"
#include "le_mdc_messages.h"
#include "le_mdc_service.h"


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
LE_MEM_DEFINE_STATIC_POOL(le_mdc_ServerData, HIGH_SERVER_DATA_COUNT, sizeof(_ServerData_t));

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
LE_REF_DEFINE_STATIC_MAP(le_mdc_ServerHandlers, HIGH_SERVER_DATA_COUNT);


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
le_msg_ServiceRef_t le_mdc_GetServiceRef
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
le_msg_SessionRef_t le_mdc_GetClientSessionRef
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
void le_mdc_AdvertiseService
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
    _ServerDataPool = le_mem_InitStaticPool(le_mdc_ServerData,
                                            HIGH_SERVER_DATA_COUNT,
                                            sizeof(_ServerData_t));

    // Create safe reference map for handler references.
    // The size of the map should be based on the number of handlers defined for the server.
    // Don't expect that to be more than 2-3, so use 3 as a reasonable guess.
    _HandlerRefMap = le_ref_InitStaticMap(le_mdc_ServerHandlers, HIGH_SERVER_DATA_COUNT);

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


static void AsyncResponse_le_mdc_AddSessionStateHandler
(
    le_mdc_ProfileRef_t profileRef,
    le_mdc_ConState_t ConnectionState,
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
    _msgPtr->id = _MSGID_le_mdc_AddSessionStateHandler;
    _msgBufPtr = _msgPtr->buffer;

    // Always pack the client context pointer first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, serverDataPtr->contextPtr ))

    // Pack the input parameters
    
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr,
                                                  profileRef ));
    LE_ASSERT(le_mdc_PackConState( &_msgBufPtr,
                                                  ConnectionState ));

    // Send the async response to the client
    TRACE("Sending message to client session %p : %ti bytes sent",
          serverDataPtr->clientSessionRef,
          _msgBufPtr-_msgPtr->buffer);

    SendMsgToClient(_msgRef);
}


static void Handle_le_mdc_AddSessionStateHandler
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
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
    le_mdc_SessionStateHandlerRef_t _result;
    _result  = le_mdc_AddSessionStateHandler ( 
        profileRef, AsyncResponse_le_mdc_AddSessionStateHandler, 
        contextPtr );

    if (_result)
    {
        // Put the handler reference result and a pointer to the associated remove function
        // into the server data object.  This function pointer is needed in case the client
        // is closed and the handlers need to be removed.
        serverDataPtr->handlerRef = (le_event_HandlerRef_t)_result;
        serverDataPtr->removeHandlerFunc =
            (RemoveHandlerFunc_t)le_mdc_RemoveSessionStateHandler;

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


static void Handle_le_mdc_RemoveSessionStateHandler
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
    le_mdc_SessionStateHandlerRef_t handlerRef = NULL;
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
    handlerRef = (le_mdc_SessionStateHandlerRef_t)serverDataPtr->handlerRef;
    le_mem_Release(serverDataPtr);

    // Define storage for output parameters

    // Call the function
    le_mdc_RemoveSessionStateHandler ( 
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


static void AsyncResponse_le_mdc_AddMtPdpSessionStateHandler
(
    le_mdc_ProfileRef_t profileRef,
    le_mdc_ConState_t ConnectionState,
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
    _msgPtr->id = _MSGID_le_mdc_AddMtPdpSessionStateHandler;
    _msgBufPtr = _msgPtr->buffer;

    // Always pack the client context pointer first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, serverDataPtr->contextPtr ))

    // Pack the input parameters
    
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr,
                                                  profileRef ));
    LE_ASSERT(le_mdc_PackConState( &_msgBufPtr,
                                                  ConnectionState ));

    // Send the async response to the client
    TRACE("Sending message to client session %p : %ti bytes sent",
          serverDataPtr->clientSessionRef,
          _msgBufPtr-_msgPtr->buffer);

    SendMsgToClient(_msgRef);
}


static void Handle_le_mdc_AddMtPdpSessionStateHandler
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
    le_mdc_MtPdpSessionStateHandlerRef_t _result;
    _result  = le_mdc_AddMtPdpSessionStateHandler ( AsyncResponse_le_mdc_AddMtPdpSessionStateHandler, 
        contextPtr );

    if (_result)
    {
        // Put the handler reference result and a pointer to the associated remove function
        // into the server data object.  This function pointer is needed in case the client
        // is closed and the handlers need to be removed.
        serverDataPtr->handlerRef = (le_event_HandlerRef_t)_result;
        serverDataPtr->removeHandlerFunc =
            (RemoveHandlerFunc_t)le_mdc_RemoveMtPdpSessionStateHandler;

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


static void Handle_le_mdc_RemoveMtPdpSessionStateHandler
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
    le_mdc_MtPdpSessionStateHandlerRef_t handlerRef = NULL;
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
    handlerRef = (le_mdc_MtPdpSessionStateHandlerRef_t)serverDataPtr->handlerRef;
    le_mem_Release(serverDataPtr);

    // Define storage for output parameters

    // Call the function
    le_mdc_RemoveMtPdpSessionStateHandler ( 
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


static void Handle_le_mdc_GetProfile
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
    uint32_t index = 0;
    if (!le_pack_UnpackUint32( &_msgBufPtr,
                                               &index ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_mdc_ProfileRef_t _result;
    _result  = le_mdc_GetProfile ( 
        index );

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


static void Handle_le_mdc_GetProfileIndex
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    uint32_t _result;
    _result  = le_mdc_GetProfileIndex ( 
        profileRef );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, _result ));

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


static void Handle_le_mdc_StartSession
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_mdc_StartSession ( 
        profileRef );

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


static void AsyncResponse_le_mdc_StartSessionAsync
(
    le_mdc_ProfileRef_t profileRef,
    le_result_t result,
    void* contextPtr
)
{
    le_msg_MessageRef_t _msgRef;
    _Message_t* _msgPtr;
    _ServerData_t* serverDataPtr = (_ServerData_t*)contextPtr;

    // This is a one-time handler; if the server accidently calls it a second time, then
    // the client sesssion ref would be NULL.
    if ( serverDataPtr->clientSessionRef == NULL )
    {
        LE_FATAL("Handler passed to le_mdc_StartSessionAsync() can't be called more than once");
    }
    

    // Will not be used if no data is sent back to client
    __attribute__((unused)) uint8_t* _msgBufPtr;

    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(serverDataPtr->clientSessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_mdc_StartSessionAsync;
    _msgBufPtr = _msgPtr->buffer;

    // Always pack the client context pointer first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, serverDataPtr->contextPtr ))

    // Pack the input parameters
    
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr,
                                                  profileRef ));
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr,
                                                  result ));

    // Send the async response to the client
    TRACE("Sending message to client session %p : %ti bytes sent",
          serverDataPtr->clientSessionRef,
          _msgBufPtr-_msgPtr->buffer);

    SendMsgToClient(_msgRef);

    // The registered handler has been called, so no longer need the server data.
    // Explicitly set clientSessionRef to NULL, so that we can catch if this function gets
    // accidently called again.
    serverDataPtr->clientSessionRef = NULL;
    le_mem_Release(serverDataPtr);
}


static void Handle_le_mdc_StartSessionAsync
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
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
    le_mdc_StartSessionAsync ( 
        profileRef, AsyncResponse_le_mdc_StartSessionAsync, 
        contextPtr );

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


static void Handle_le_mdc_StopSession
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_mdc_StopSession ( 
        profileRef );

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


static void AsyncResponse_le_mdc_StopSessionAsync
(
    le_mdc_ProfileRef_t profileRef,
    le_result_t result,
    void* contextPtr
)
{
    le_msg_MessageRef_t _msgRef;
    _Message_t* _msgPtr;
    _ServerData_t* serverDataPtr = (_ServerData_t*)contextPtr;

    // This is a one-time handler; if the server accidently calls it a second time, then
    // the client sesssion ref would be NULL.
    if ( serverDataPtr->clientSessionRef == NULL )
    {
        LE_FATAL("Handler passed to le_mdc_StopSessionAsync() can't be called more than once");
    }
    

    // Will not be used if no data is sent back to client
    __attribute__((unused)) uint8_t* _msgBufPtr;

    // Create a new message object and get the message buffer
    _msgRef = le_msg_CreateMsg(serverDataPtr->clientSessionRef);
    _msgPtr = le_msg_GetPayloadPtr(_msgRef);
    _msgPtr->id = _MSGID_le_mdc_StopSessionAsync;
    _msgBufPtr = _msgPtr->buffer;

    // Always pack the client context pointer first
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr, serverDataPtr->contextPtr ))

    // Pack the input parameters
    
    LE_ASSERT(le_pack_PackReference( &_msgBufPtr,
                                                  profileRef ));
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr,
                                                  result ));

    // Send the async response to the client
    TRACE("Sending message to client session %p : %ti bytes sent",
          serverDataPtr->clientSessionRef,
          _msgBufPtr-_msgPtr->buffer);

    SendMsgToClient(_msgRef);

    // The registered handler has been called, so no longer need the server data.
    // Explicitly set clientSessionRef to NULL, so that we can catch if this function gets
    // accidently called again.
    serverDataPtr->clientSessionRef = NULL;
    le_mem_Release(serverDataPtr);
}


static void Handle_le_mdc_StopSessionAsync
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
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
    le_mdc_StopSessionAsync ( 
        profileRef, AsyncResponse_le_mdc_StopSessionAsync, 
        contextPtr );

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


static void Handle_le_mdc_RejectMtPdpSession
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_mdc_RejectMtPdpSession ( 
        profileRef );

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


static void Handle_le_mdc_GetSessionState
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters
    le_mdc_ConState_t connectionStateBuffer = (le_mdc_ConState_t) 0;
    le_mdc_ConState_t *connectionStatePtr = &connectionStateBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        connectionStatePtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetSessionState ( 
        profileRef, 
        connectionStatePtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (connectionStatePtr)
    {
        LE_ASSERT(le_mdc_PackConState( &_msgBufPtr,
                                                      *connectionStatePtr ));
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


static void Handle_le_mdc_GetInterfaceName
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    size_t interfaceNameSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &interfaceNameSize ))
    {
        goto error_unpack;
    }
    if ( (interfaceNameSize > 20) &&
         (interfaceNameSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting interfaceNameSize from %" PRIuS " to 20", interfaceNameSize);
        interfaceNameSize = 20;
    }
    if (interfaceNameSize >= UINT32_MAX)
    {
        interfaceNameSize = UINT32_MAX;
    }
    else
    {
        interfaceNameSize++;
    }

    // Define storage for output parameters
    char interfaceNameBuffer[21] = { 0 };
    char *interfaceName = interfaceNameBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        interfaceName = NULL;
        interfaceNameSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetInterfaceName ( 
        profileRef, 
        interfaceName, 
        interfaceNameSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (interfaceName)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      interfaceName, 20 ));
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


static void Handle_le_mdc_GetIPv4Address
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    size_t ipAddrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &ipAddrSize ))
    {
        goto error_unpack;
    }
    if ( (ipAddrSize > 15) &&
         (ipAddrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting ipAddrSize from %" PRIuS " to 15", ipAddrSize);
        ipAddrSize = 15;
    }
    if (ipAddrSize >= UINT32_MAX)
    {
        ipAddrSize = UINT32_MAX;
    }
    else
    {
        ipAddrSize++;
    }

    // Define storage for output parameters
    char ipAddrBuffer[16] = { 0 };
    char *ipAddr = ipAddrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        ipAddr = NULL;
        ipAddrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetIPv4Address ( 
        profileRef, 
        ipAddr, 
        ipAddrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (ipAddr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      ipAddr, 15 ));
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


static void Handle_le_mdc_GetIPv4GatewayAddress
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    size_t gatewayAddrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &gatewayAddrSize ))
    {
        goto error_unpack;
    }
    if ( (gatewayAddrSize > 15) &&
         (gatewayAddrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting gatewayAddrSize from %" PRIuS " to 15", gatewayAddrSize);
        gatewayAddrSize = 15;
    }
    if (gatewayAddrSize >= UINT32_MAX)
    {
        gatewayAddrSize = UINT32_MAX;
    }
    else
    {
        gatewayAddrSize++;
    }

    // Define storage for output parameters
    char gatewayAddrBuffer[16] = { 0 };
    char *gatewayAddr = gatewayAddrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        gatewayAddr = NULL;
        gatewayAddrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetIPv4GatewayAddress ( 
        profileRef, 
        gatewayAddr, 
        gatewayAddrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (gatewayAddr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      gatewayAddr, 15 ));
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


static void Handle_le_mdc_GetIPv4DNSAddresses
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    size_t dns1AddrStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &dns1AddrStrSize ))
    {
        goto error_unpack;
    }
    if ( (dns1AddrStrSize > 15) &&
         (dns1AddrStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting dns1AddrStrSize from %" PRIuS " to 15", dns1AddrStrSize);
        dns1AddrStrSize = 15;
    }
    if (dns1AddrStrSize >= UINT32_MAX)
    {
        dns1AddrStrSize = UINT32_MAX;
    }
    else
    {
        dns1AddrStrSize++;
    }
    size_t dns2AddrStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &dns2AddrStrSize ))
    {
        goto error_unpack;
    }
    if ( (dns2AddrStrSize > 15) &&
         (dns2AddrStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting dns2AddrStrSize from %" PRIuS " to 15", dns2AddrStrSize);
        dns2AddrStrSize = 15;
    }
    if (dns2AddrStrSize >= UINT32_MAX)
    {
        dns2AddrStrSize = UINT32_MAX;
    }
    else
    {
        dns2AddrStrSize++;
    }

    // Define storage for output parameters
    char dns1AddrStrBuffer[16] = { 0 };
    char *dns1AddrStr = dns1AddrStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        dns1AddrStr = NULL;
        dns1AddrStrSize = 0;
    }
    char dns2AddrStrBuffer[16] = { 0 };
    char *dns2AddrStr = dns2AddrStrBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        dns2AddrStr = NULL;
        dns2AddrStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetIPv4DNSAddresses ( 
        profileRef, 
        dns1AddrStr, 
        dns1AddrStrSize, 
        dns2AddrStr, 
        dns2AddrStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (dns1AddrStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      dns1AddrStr, 15 ));
    }
    if (dns2AddrStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      dns2AddrStr, 15 ));
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


static void Handle_le_mdc_GetIPv6Address
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    size_t ipAddrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &ipAddrSize ))
    {
        goto error_unpack;
    }
    if ( (ipAddrSize > 45) &&
         (ipAddrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting ipAddrSize from %" PRIuS " to 45", ipAddrSize);
        ipAddrSize = 45;
    }
    if (ipAddrSize >= UINT32_MAX)
    {
        ipAddrSize = UINT32_MAX;
    }
    else
    {
        ipAddrSize++;
    }

    // Define storage for output parameters
    char ipAddrBuffer[46] = { 0 };
    char *ipAddr = ipAddrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        ipAddr = NULL;
        ipAddrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetIPv6Address ( 
        profileRef, 
        ipAddr, 
        ipAddrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (ipAddr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      ipAddr, 45 ));
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


static void Handle_le_mdc_GetIPv6GatewayAddress
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    size_t gatewayAddrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &gatewayAddrSize ))
    {
        goto error_unpack;
    }
    if ( (gatewayAddrSize > 45) &&
         (gatewayAddrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting gatewayAddrSize from %" PRIuS " to 45", gatewayAddrSize);
        gatewayAddrSize = 45;
    }
    if (gatewayAddrSize >= UINT32_MAX)
    {
        gatewayAddrSize = UINT32_MAX;
    }
    else
    {
        gatewayAddrSize++;
    }

    // Define storage for output parameters
    char gatewayAddrBuffer[46] = { 0 };
    char *gatewayAddr = gatewayAddrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        gatewayAddr = NULL;
        gatewayAddrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetIPv6GatewayAddress ( 
        profileRef, 
        gatewayAddr, 
        gatewayAddrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (gatewayAddr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      gatewayAddr, 45 ));
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


static void Handle_le_mdc_GetIPv6DNSAddresses
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    size_t dns1AddrStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &dns1AddrStrSize ))
    {
        goto error_unpack;
    }
    if ( (dns1AddrStrSize > 45) &&
         (dns1AddrStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting dns1AddrStrSize from %" PRIuS " to 45", dns1AddrStrSize);
        dns1AddrStrSize = 45;
    }
    if (dns1AddrStrSize >= UINT32_MAX)
    {
        dns1AddrStrSize = UINT32_MAX;
    }
    else
    {
        dns1AddrStrSize++;
    }
    size_t dns2AddrStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &dns2AddrStrSize ))
    {
        goto error_unpack;
    }
    if ( (dns2AddrStrSize > 45) &&
         (dns2AddrStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting dns2AddrStrSize from %" PRIuS " to 45", dns2AddrStrSize);
        dns2AddrStrSize = 45;
    }
    if (dns2AddrStrSize >= UINT32_MAX)
    {
        dns2AddrStrSize = UINT32_MAX;
    }
    else
    {
        dns2AddrStrSize++;
    }

    // Define storage for output parameters
    char dns1AddrStrBuffer[46] = { 0 };
    char *dns1AddrStr = dns1AddrStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        dns1AddrStr = NULL;
        dns1AddrStrSize = 0;
    }
    char dns2AddrStrBuffer[46] = { 0 };
    char *dns2AddrStr = dns2AddrStrBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        dns2AddrStr = NULL;
        dns2AddrStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetIPv6DNSAddresses ( 
        profileRef, 
        dns1AddrStr, 
        dns1AddrStrSize, 
        dns2AddrStr, 
        dns2AddrStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (dns1AddrStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      dns1AddrStr, 45 ));
    }
    if (dns2AddrStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      dns2AddrStr, 45 ));
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


static void Handle_le_mdc_IsIPv4
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    bool _result;
    _result  = le_mdc_IsIPv4 ( 
        profileRef );

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


static void Handle_le_mdc_IsIPv6
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    bool _result;
    _result  = le_mdc_IsIPv6 ( 
        profileRef );

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


static void Handle_le_mdc_GetDataBearerTechnology
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters
    le_mdc_DataBearerTechnology_t downlinkDataBearerTechPtrBuffer = (le_mdc_DataBearerTechnology_t) 0;
    le_mdc_DataBearerTechnology_t *downlinkDataBearerTechPtrPtr = &downlinkDataBearerTechPtrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        downlinkDataBearerTechPtrPtr = NULL;
    }
    le_mdc_DataBearerTechnology_t uplinkDataBearerTechPtrBuffer = (le_mdc_DataBearerTechnology_t) 0;
    le_mdc_DataBearerTechnology_t *uplinkDataBearerTechPtrPtr = &uplinkDataBearerTechPtrBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        uplinkDataBearerTechPtrPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetDataBearerTechnology ( 
        profileRef, 
        downlinkDataBearerTechPtrPtr, 
        uplinkDataBearerTechPtrPtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (downlinkDataBearerTechPtrPtr)
    {
        LE_ASSERT(le_mdc_PackDataBearerTechnology( &_msgBufPtr,
                                                      *downlinkDataBearerTechPtrPtr ));
    }
    if (uplinkDataBearerTechPtrPtr)
    {
        LE_ASSERT(le_mdc_PackDataBearerTechnology( &_msgBufPtr,
                                                      *uplinkDataBearerTechPtrPtr ));
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


static void Handle_le_mdc_GetBytesCounters
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
    uint64_t rxBytesBuffer = 0;
    uint64_t *rxBytesPtr = &rxBytesBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        rxBytesPtr = NULL;
    }
    uint64_t txBytesBuffer = 0;
    uint64_t *txBytesPtr = &txBytesBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        txBytesPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetBytesCounters ( 
        rxBytesPtr, 
        txBytesPtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (rxBytesPtr)
    {
        LE_ASSERT(le_pack_PackUint64( &_msgBufPtr,
                                                      *rxBytesPtr ));
    }
    if (txBytesPtr)
    {
        LE_ASSERT(le_pack_PackUint64( &_msgBufPtr,
                                                      *txBytesPtr ));
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


static void Handle_le_mdc_ResetBytesCounter
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
    le_result_t _result;
    _result  = le_mdc_ResetBytesCounter (  );

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
}


static void Handle_le_mdc_StopBytesCounter
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
    le_result_t _result;
    _result  = le_mdc_StopBytesCounter (  );

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
}


static void Handle_le_mdc_StartBytesCounter
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
    le_result_t _result;
    _result  = le_mdc_StartBytesCounter (  );

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
}


static void Handle_le_mdc_SetPDP
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    le_mdc_Pdp_t pdp = (le_mdc_Pdp_t) 0;
    if (!le_mdc_UnpackPdp( &_msgBufPtr,
                                               &pdp ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_mdc_SetPDP ( 
        profileRef, 
        pdp );

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


static void Handle_le_mdc_GetPDP
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_mdc_Pdp_t _result;
    _result  = le_mdc_GetPDP ( 
        profileRef );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_mdc_PackPdp( &_msgBufPtr, _result ));

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


static void Handle_le_mdc_SetAPN
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    char apnStr[101] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               apnStr,
                               sizeof(apnStr),
                               100 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_mdc_SetAPN ( 
        profileRef, 
        apnStr );

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


static void Handle_le_mdc_SetDefaultAPN
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_mdc_SetDefaultAPN ( 
        profileRef );

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


static void Handle_le_mdc_GetAPN
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    size_t apnStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &apnStrSize ))
    {
        goto error_unpack;
    }
    if ( (apnStrSize > 100) &&
         (apnStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting apnStrSize from %" PRIuS " to 100", apnStrSize);
        apnStrSize = 100;
    }
    if (apnStrSize >= UINT32_MAX)
    {
        apnStrSize = UINT32_MAX;
    }
    else
    {
        apnStrSize++;
    }

    // Define storage for output parameters
    char apnStrBuffer[101] = { 0 };
    char *apnStr = apnStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        apnStr = NULL;
        apnStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetAPN ( 
        profileRef, 
        apnStr, 
        apnStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (apnStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      apnStr, 100 ));
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


static void Handle_le_mdc_SetAuthentication
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    le_mdc_Auth_t type = (le_mdc_Auth_t) 0;
    if (!le_mdc_UnpackAuth( &_msgBufPtr,
                                               &type ))
    {
        goto error_unpack;
    }
    char userName[65] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               userName,
                               sizeof(userName),
                               64 ))
    {
        goto error_unpack;
    }
    char password[101] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               password,
                               sizeof(password),
                               100 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_mdc_SetAuthentication ( 
        profileRef, 
        type, 
        userName, 
        password );

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


static void Handle_le_mdc_GetAuthentication
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    size_t userNameSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &userNameSize ))
    {
        goto error_unpack;
    }
    if ( (userNameSize > 64) &&
         (userNameSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting userNameSize from %" PRIuS " to 64", userNameSize);
        userNameSize = 64;
    }
    if (userNameSize >= UINT32_MAX)
    {
        userNameSize = UINT32_MAX;
    }
    else
    {
        userNameSize++;
    }
    size_t passwordSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &passwordSize ))
    {
        goto error_unpack;
    }
    if ( (passwordSize > 100) &&
         (passwordSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting passwordSize from %" PRIuS " to 100", passwordSize);
        passwordSize = 100;
    }
    if (passwordSize >= UINT32_MAX)
    {
        passwordSize = UINT32_MAX;
    }
    else
    {
        passwordSize++;
    }

    // Define storage for output parameters
    le_mdc_Auth_t typeBuffer = (le_mdc_Auth_t) 0;
    le_mdc_Auth_t *typePtr = &typeBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        typePtr = NULL;
    }
    char userNameBuffer[65] = { 0 };
    char *userName = userNameBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        userName = NULL;
        userNameSize = 0;
    }
    char passwordBuffer[101] = { 0 };
    char *password = passwordBuffer;
    if (!(_requiredOutputs & (1u << 2)))
    {
        password = NULL;
        passwordSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetAuthentication ( 
        profileRef, 
        typePtr, 
        userName, 
        userNameSize, 
        password, 
        passwordSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (typePtr)
    {
        LE_ASSERT(le_mdc_PackAuth( &_msgBufPtr,
                                                      *typePtr ));
    }
    if (userName)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      userName, 64 ));
    }
    if (password)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      password, 100 ));
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


static void Handle_le_mdc_NumProfiles
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
    uint32_t _result;
    _result  = le_mdc_NumProfiles (  );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackUint32( &_msgBufPtr, _result ));

    // Pack any "out" parameters

    // Return the response
    TRACE("Sending response to client session %p : %ti bytes sent",
          le_msg_GetSession(_msgRef),
          _msgBufPtr-_msgBufStartPtr);


    le_msg_Respond(_msgRef);

    return;
}


static void Handle_le_mdc_GetProfileFromApn
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
    char apnStr[101] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               apnStr,
                               sizeof(apnStr),
                               100 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters
    le_mdc_ProfileRef_t profileRefBuffer = NULL;
    le_mdc_ProfileRef_t *profileRefPtr = &profileRefBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        profileRefPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetProfileFromApn ( 
        apnStr, 
        profileRefPtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (profileRefPtr)
    {
        LE_ASSERT(le_pack_PackReference( &_msgBufPtr,
                                                      *profileRefPtr ));
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


static void Handle_le_mdc_GetDisconnectionReason
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_mdc_DisconnectionReason_t _result;
    _result  = le_mdc_GetDisconnectionReason ( 
        profileRef );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_mdc_PackDisconnectionReason( &_msgBufPtr, _result ));

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


static void Handle_le_mdc_GetPlatformSpecificDisconnectionCode
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    int32_t _result;
    _result  = le_mdc_GetPlatformSpecificDisconnectionCode ( 
        profileRef );

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


static void Handle_le_mdc_GetPlatformSpecificFailureConnectionReason
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters
    int32_t failureTypeBuffer = 0;
    int32_t *failureTypePtr = &failureTypeBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        failureTypePtr = NULL;
    }
    int32_t failureCodeBuffer = 0;
    int32_t *failureCodePtr = &failureCodeBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        failureCodePtr = NULL;
    }

    // Call the function
    le_mdc_GetPlatformSpecificFailureConnectionReason ( 
        profileRef, 
        failureTypePtr, 
        failureCodePtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack any "out" parameters
    if (failureTypePtr)
    {
        LE_ASSERT(le_pack_PackInt32( &_msgBufPtr,
                                                      *failureTypePtr ));
    }
    if (failureCodePtr)
    {
        LE_ASSERT(le_pack_PackInt32( &_msgBufPtr,
                                                      *failureCodePtr ));
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


static void Handle_le_mdc_GetDisconnectionReasonExt
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    le_mdc_Pdp_t pdpType = (le_mdc_Pdp_t) 0;
    if (!le_mdc_UnpackPdp( &_msgBufPtr,
                                               &pdpType ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_mdc_DisconnectionReason_t _result;
    _result  = le_mdc_GetDisconnectionReasonExt ( 
        profileRef, 
        pdpType );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_mdc_PackDisconnectionReason( &_msgBufPtr, _result ));

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


static void Handle_le_mdc_GetPlatformSpecificDisconnectionCodeExt
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    le_mdc_Pdp_t pdpType = (le_mdc_Pdp_t) 0;
    if (!le_mdc_UnpackPdp( &_msgBufPtr,
                                               &pdpType ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    int32_t _result;
    _result  = le_mdc_GetPlatformSpecificDisconnectionCodeExt ( 
        profileRef, 
        pdpType );

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


static void Handle_le_mdc_GetPlatformSpecificFailureConnectionReasonExt
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    le_mdc_Pdp_t pdpType = (le_mdc_Pdp_t) 0;
    if (!le_mdc_UnpackPdp( &_msgBufPtr,
                                               &pdpType ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters
    int32_t failureTypeBuffer = 0;
    int32_t *failureTypePtr = &failureTypeBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        failureTypePtr = NULL;
    }
    int32_t failureCodeBuffer = 0;
    int32_t *failureCodePtr = &failureCodeBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        failureCodePtr = NULL;
    }

    // Call the function
    le_mdc_GetPlatformSpecificFailureConnectionReasonExt ( 
        profileRef, 
        pdpType, 
        failureTypePtr, 
        failureCodePtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack any "out" parameters
    if (failureTypePtr)
    {
        LE_ASSERT(le_pack_PackInt32( &_msgBufPtr,
                                                      *failureTypePtr ));
    }
    if (failureCodePtr)
    {
        LE_ASSERT(le_pack_PackInt32( &_msgBufPtr,
                                                      *failureCodePtr ));
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


static void Handle_le_mdc_MapProfileOnNetworkInterface
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
    le_mdc_ProfileRef_t profileRef = NULL;
    if (!le_pack_UnpackReference( &_msgBufPtr,
                                               &profileRef ))
    {
        goto error_unpack;
    }
    char interfaceName[21] = {0};
    if (!le_pack_UnpackString( &_msgBufPtr,
                               interfaceName,
                               sizeof(interfaceName),
                               20 ))
    {
        goto error_unpack;
    }

    // Define storage for output parameters

    // Call the function
    le_result_t _result;
    _result  = le_mdc_MapProfileOnNetworkInterface ( 
        profileRef, 
        interfaceName );

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


static void Handle_le_mdc_GetProfileList
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
    size_t profileListSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &profileListSize ))
    {
        goto error_unpack;
    }
    if ( (profileListSize > 36) )
    {
        LE_DEBUG("Adjusting profileListSize from %" PRIuS " to 36", profileListSize);
        profileListSize = 36;
    }

    // Define storage for output parameters
    le_mdc_ProfileInfo_t profileListBuffer[36] = { { } };
    le_mdc_ProfileInfo_t *profileListPtr = profileListBuffer;
    size_t *profileListSizePtr = &profileListSize;
    if (!(_requiredOutputs & (1u << 0)))
    {
        profileListPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_mdc_GetProfileList ( 
        profileListPtr, 
        &profileListSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (profileListPtr)
    {
        bool profileListResult;
            LE_PACK_PACKSTRUCTARRAY( &_msgBufPtr,
                           profileListPtr, (*profileListSizePtr),
                           36, le_mdc_PackProfileInfo,
                           &profileListResult );
        LE_ASSERT(profileListResult);
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
        case _MSGID_le_mdc_AddSessionStateHandler :
            Handle_le_mdc_AddSessionStateHandler(msgRef);
            break;
        case _MSGID_le_mdc_RemoveSessionStateHandler :
            Handle_le_mdc_RemoveSessionStateHandler(msgRef);
            break;
        case _MSGID_le_mdc_AddMtPdpSessionStateHandler :
            Handle_le_mdc_AddMtPdpSessionStateHandler(msgRef);
            break;
        case _MSGID_le_mdc_RemoveMtPdpSessionStateHandler :
            Handle_le_mdc_RemoveMtPdpSessionStateHandler(msgRef);
            break;
        case _MSGID_le_mdc_GetProfile :
            Handle_le_mdc_GetProfile(msgRef);
            break;
        case _MSGID_le_mdc_GetProfileIndex :
            Handle_le_mdc_GetProfileIndex(msgRef);
            break;
        case _MSGID_le_mdc_StartSession :
            Handle_le_mdc_StartSession(msgRef);
            break;
        case _MSGID_le_mdc_StartSessionAsync :
            Handle_le_mdc_StartSessionAsync(msgRef);
            break;
        case _MSGID_le_mdc_StopSession :
            Handle_le_mdc_StopSession(msgRef);
            break;
        case _MSGID_le_mdc_StopSessionAsync :
            Handle_le_mdc_StopSessionAsync(msgRef);
            break;
        case _MSGID_le_mdc_RejectMtPdpSession :
            Handle_le_mdc_RejectMtPdpSession(msgRef);
            break;
        case _MSGID_le_mdc_GetSessionState :
            Handle_le_mdc_GetSessionState(msgRef);
            break;
        case _MSGID_le_mdc_GetInterfaceName :
            Handle_le_mdc_GetInterfaceName(msgRef);
            break;
        case _MSGID_le_mdc_GetIPv4Address :
            Handle_le_mdc_GetIPv4Address(msgRef);
            break;
        case _MSGID_le_mdc_GetIPv4GatewayAddress :
            Handle_le_mdc_GetIPv4GatewayAddress(msgRef);
            break;
        case _MSGID_le_mdc_GetIPv4DNSAddresses :
            Handle_le_mdc_GetIPv4DNSAddresses(msgRef);
            break;
        case _MSGID_le_mdc_GetIPv6Address :
            Handle_le_mdc_GetIPv6Address(msgRef);
            break;
        case _MSGID_le_mdc_GetIPv6GatewayAddress :
            Handle_le_mdc_GetIPv6GatewayAddress(msgRef);
            break;
        case _MSGID_le_mdc_GetIPv6DNSAddresses :
            Handle_le_mdc_GetIPv6DNSAddresses(msgRef);
            break;
        case _MSGID_le_mdc_IsIPv4 :
            Handle_le_mdc_IsIPv4(msgRef);
            break;
        case _MSGID_le_mdc_IsIPv6 :
            Handle_le_mdc_IsIPv6(msgRef);
            break;
        case _MSGID_le_mdc_GetDataBearerTechnology :
            Handle_le_mdc_GetDataBearerTechnology(msgRef);
            break;
        case _MSGID_le_mdc_GetBytesCounters :
            Handle_le_mdc_GetBytesCounters(msgRef);
            break;
        case _MSGID_le_mdc_ResetBytesCounter :
            Handle_le_mdc_ResetBytesCounter(msgRef);
            break;
        case _MSGID_le_mdc_StopBytesCounter :
            Handle_le_mdc_StopBytesCounter(msgRef);
            break;
        case _MSGID_le_mdc_StartBytesCounter :
            Handle_le_mdc_StartBytesCounter(msgRef);
            break;
        case _MSGID_le_mdc_SetPDP :
            Handle_le_mdc_SetPDP(msgRef);
            break;
        case _MSGID_le_mdc_GetPDP :
            Handle_le_mdc_GetPDP(msgRef);
            break;
        case _MSGID_le_mdc_SetAPN :
            Handle_le_mdc_SetAPN(msgRef);
            break;
        case _MSGID_le_mdc_SetDefaultAPN :
            Handle_le_mdc_SetDefaultAPN(msgRef);
            break;
        case _MSGID_le_mdc_GetAPN :
            Handle_le_mdc_GetAPN(msgRef);
            break;
        case _MSGID_le_mdc_SetAuthentication :
            Handle_le_mdc_SetAuthentication(msgRef);
            break;
        case _MSGID_le_mdc_GetAuthentication :
            Handle_le_mdc_GetAuthentication(msgRef);
            break;
        case _MSGID_le_mdc_NumProfiles :
            Handle_le_mdc_NumProfiles(msgRef);
            break;
        case _MSGID_le_mdc_GetProfileFromApn :
            Handle_le_mdc_GetProfileFromApn(msgRef);
            break;
        case _MSGID_le_mdc_GetDisconnectionReason :
            Handle_le_mdc_GetDisconnectionReason(msgRef);
            break;
        case _MSGID_le_mdc_GetPlatformSpecificDisconnectionCode :
            Handle_le_mdc_GetPlatformSpecificDisconnectionCode(msgRef);
            break;
        case _MSGID_le_mdc_GetPlatformSpecificFailureConnectionReason :
            Handle_le_mdc_GetPlatformSpecificFailureConnectionReason(msgRef);
            break;
        case _MSGID_le_mdc_GetDisconnectionReasonExt :
            Handle_le_mdc_GetDisconnectionReasonExt(msgRef);
            break;
        case _MSGID_le_mdc_GetPlatformSpecificDisconnectionCodeExt :
            Handle_le_mdc_GetPlatformSpecificDisconnectionCodeExt(msgRef);
            break;
        case _MSGID_le_mdc_GetPlatformSpecificFailureConnectionReasonExt :
            Handle_le_mdc_GetPlatformSpecificFailureConnectionReasonExt(msgRef);
            break;
        case _MSGID_le_mdc_MapProfileOnNetworkInterface :
            Handle_le_mdc_MapProfileOnNetworkInterface(msgRef);
            break;
        case _MSGID_le_mdc_GetProfileList :
            Handle_le_mdc_GetProfileList(msgRef);
            break;

        default: LE_ERROR("Unknowm msg id = %" PRIu32 , msgPtr->id);
    }

    // Clear the client session ref associated with the current message, since the message
    // has now been processed.
    LE_CDATA_THIS->_ClientSessionRef = 0;
}