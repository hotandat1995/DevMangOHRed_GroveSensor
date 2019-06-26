
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#include "le_info_server.h"
#include "le_info_messages.h"
#include "le_info_service.h"


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
LE_MEM_DEFINE_STATIC_POOL(le_info_ServerData, HIGH_SERVER_DATA_COUNT, sizeof(_ServerData_t));

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
LE_REF_DEFINE_STATIC_MAP(le_info_ServerHandlers, HIGH_SERVER_DATA_COUNT);


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
le_msg_ServiceRef_t le_info_GetServiceRef
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
le_msg_SessionRef_t le_info_GetClientSessionRef
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
void le_info_AdvertiseService
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
    _ServerDataPool = le_mem_InitStaticPool(le_info_ServerData,
                                            HIGH_SERVER_DATA_COUNT,
                                            sizeof(_ServerData_t));

    // Create safe reference map for handler references.
    // The size of the map should be based on the number of handlers defined for the server.
    // Don't expect that to be more than 2-3, so use 3 as a reasonable guess.
    _HandlerRefMap = le_ref_InitStaticMap(le_info_ServerHandlers, HIGH_SERVER_DATA_COUNT);

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


static void Handle_le_info_GetImei
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
    size_t imeiSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &imeiSize ))
    {
        goto error_unpack;
    }
    if ( (imeiSize > 15) &&
         (imeiSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting imeiSize from %" PRIuS " to 15", imeiSize);
        imeiSize = 15;
    }
    if (imeiSize >= UINT32_MAX)
    {
        imeiSize = UINT32_MAX;
    }
    else
    {
        imeiSize++;
    }

    // Define storage for output parameters
    char imeiBuffer[16] = { 0 };
    char *imei = imeiBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        imei = NULL;
        imeiSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetImei ( 
        imei, 
        imeiSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (imei)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      imei, 15 ));
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


static void Handle_le_info_GetImeiSv
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
    size_t imeiSvSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &imeiSvSize ))
    {
        goto error_unpack;
    }
    if ( (imeiSvSize > 255) &&
         (imeiSvSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting imeiSvSize from %" PRIuS " to 255", imeiSvSize);
        imeiSvSize = 255;
    }
    if (imeiSvSize >= UINT32_MAX)
    {
        imeiSvSize = UINT32_MAX;
    }
    else
    {
        imeiSvSize++;
    }

    // Define storage for output parameters
    char imeiSvBuffer[256] = { 0 };
    char *imeiSv = imeiSvBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        imeiSv = NULL;
        imeiSvSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetImeiSv ( 
        imeiSv, 
        imeiSvSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (imeiSv)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      imeiSv, 255 ));
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


static void Handle_le_info_GetFirmwareVersion
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
    size_t versionSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &versionSize ))
    {
        goto error_unpack;
    }
    if ( (versionSize > 256) &&
         (versionSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting versionSize from %" PRIuS " to 256", versionSize);
        versionSize = 256;
    }
    if (versionSize >= UINT32_MAX)
    {
        versionSize = UINT32_MAX;
    }
    else
    {
        versionSize++;
    }

    // Define storage for output parameters
    char versionBuffer[257] = { 0 };
    char *version = versionBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        version = NULL;
        versionSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetFirmwareVersion ( 
        version, 
        versionSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (version)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      version, 256 ));
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


static void Handle_le_info_GetResetInformation
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
    size_t resetSpecificInfoStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &resetSpecificInfoStrSize ))
    {
        goto error_unpack;
    }
    if ( (resetSpecificInfoStrSize > 50) &&
         (resetSpecificInfoStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting resetSpecificInfoStrSize from %" PRIuS " to 50", resetSpecificInfoStrSize);
        resetSpecificInfoStrSize = 50;
    }
    if (resetSpecificInfoStrSize >= UINT32_MAX)
    {
        resetSpecificInfoStrSize = UINT32_MAX;
    }
    else
    {
        resetSpecificInfoStrSize++;
    }

    // Define storage for output parameters
    le_info_Reset_t resetBuffer = (le_info_Reset_t) 0;
    le_info_Reset_t *resetPtr = &resetBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        resetPtr = NULL;
    }
    char resetSpecificInfoStrBuffer[51] = { 0 };
    char *resetSpecificInfoStr = resetSpecificInfoStrBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        resetSpecificInfoStr = NULL;
        resetSpecificInfoStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetResetInformation ( 
        resetPtr, 
        resetSpecificInfoStr, 
        resetSpecificInfoStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (resetPtr)
    {
        LE_ASSERT(le_info_PackReset( &_msgBufPtr,
                                                      *resetPtr ));
    }
    if (resetSpecificInfoStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      resetSpecificInfoStr, 50 ));
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


static void Handle_le_info_GetBootloaderVersion
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
    size_t versionSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &versionSize ))
    {
        goto error_unpack;
    }
    if ( (versionSize > 256) &&
         (versionSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting versionSize from %" PRIuS " to 256", versionSize);
        versionSize = 256;
    }
    if (versionSize >= UINT32_MAX)
    {
        versionSize = UINT32_MAX;
    }
    else
    {
        versionSize++;
    }

    // Define storage for output parameters
    char versionBuffer[257] = { 0 };
    char *version = versionBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        version = NULL;
        versionSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetBootloaderVersion ( 
        version, 
        versionSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (version)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      version, 256 ));
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


static void Handle_le_info_GetDeviceModel
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
    size_t modelPtrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &modelPtrSize ))
    {
        goto error_unpack;
    }
    if ( (modelPtrSize > 256) &&
         (modelPtrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting modelPtrSize from %" PRIuS " to 256", modelPtrSize);
        modelPtrSize = 256;
    }
    if (modelPtrSize >= UINT32_MAX)
    {
        modelPtrSize = UINT32_MAX;
    }
    else
    {
        modelPtrSize++;
    }

    // Define storage for output parameters
    char modelPtrBuffer[257] = { 0 };
    char *modelPtr = modelPtrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        modelPtr = NULL;
        modelPtrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetDeviceModel ( 
        modelPtr, 
        modelPtrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (modelPtr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      modelPtr, 256 ));
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


static void Handle_le_info_GetMeid
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
    size_t meidStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &meidStrSize ))
    {
        goto error_unpack;
    }
    if ( (meidStrSize > 32) &&
         (meidStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting meidStrSize from %" PRIuS " to 32", meidStrSize);
        meidStrSize = 32;
    }
    if (meidStrSize >= UINT32_MAX)
    {
        meidStrSize = UINT32_MAX;
    }
    else
    {
        meidStrSize++;
    }

    // Define storage for output parameters
    char meidStrBuffer[33] = { 0 };
    char *meidStr = meidStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        meidStr = NULL;
        meidStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetMeid ( 
        meidStr, 
        meidStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (meidStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      meidStr, 32 ));
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


static void Handle_le_info_GetEsn
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
    size_t esnStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &esnStrSize ))
    {
        goto error_unpack;
    }
    if ( (esnStrSize > 32) &&
         (esnStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting esnStrSize from %" PRIuS " to 32", esnStrSize);
        esnStrSize = 32;
    }
    if (esnStrSize >= UINT32_MAX)
    {
        esnStrSize = UINT32_MAX;
    }
    else
    {
        esnStrSize++;
    }

    // Define storage for output parameters
    char esnStrBuffer[33] = { 0 };
    char *esnStr = esnStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        esnStr = NULL;
        esnStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetEsn ( 
        esnStr, 
        esnStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (esnStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      esnStr, 32 ));
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


static void Handle_le_info_GetMdn
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
    size_t mdnStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &mdnStrSize ))
    {
        goto error_unpack;
    }
    if ( (mdnStrSize > 32) &&
         (mdnStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting mdnStrSize from %" PRIuS " to 32", mdnStrSize);
        mdnStrSize = 32;
    }
    if (mdnStrSize >= UINT32_MAX)
    {
        mdnStrSize = UINT32_MAX;
    }
    else
    {
        mdnStrSize++;
    }

    // Define storage for output parameters
    char mdnStrBuffer[33] = { 0 };
    char *mdnStr = mdnStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        mdnStr = NULL;
        mdnStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetMdn ( 
        mdnStr, 
        mdnStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (mdnStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      mdnStr, 32 ));
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


static void Handle_le_info_GetPrlVersion
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
    uint16_t prlVersionBuffer = 0;
    uint16_t *prlVersionPtr = &prlVersionBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        prlVersionPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetPrlVersion ( 
        prlVersionPtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (prlVersionPtr)
    {
        LE_ASSERT(le_pack_PackUint16( &_msgBufPtr,
                                                      *prlVersionPtr ));
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


static void Handle_le_info_GetPrlOnlyPreference
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
    bool prlOnlyPreferenceBuffer = false;
    bool *prlOnlyPreferencePtr = &prlOnlyPreferenceBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        prlOnlyPreferencePtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetPrlOnlyPreference ( 
        prlOnlyPreferencePtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (prlOnlyPreferencePtr)
    {
        LE_ASSERT(le_pack_PackBool( &_msgBufPtr,
                                                      *prlOnlyPreferencePtr ));
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


static void Handle_le_info_GetMin
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
    size_t msisdnStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &msisdnStrSize ))
    {
        goto error_unpack;
    }
    if ( (msisdnStrSize > 32) &&
         (msisdnStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting msisdnStrSize from %" PRIuS " to 32", msisdnStrSize);
        msisdnStrSize = 32;
    }
    if (msisdnStrSize >= UINT32_MAX)
    {
        msisdnStrSize = UINT32_MAX;
    }
    else
    {
        msisdnStrSize++;
    }

    // Define storage for output parameters
    char msisdnStrBuffer[33] = { 0 };
    char *msisdnStr = msisdnStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        msisdnStr = NULL;
        msisdnStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetMin ( 
        msisdnStr, 
        msisdnStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (msisdnStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      msisdnStr, 32 ));
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


static void Handle_le_info_GetNai
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
    size_t naiStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &naiStrSize ))
    {
        goto error_unpack;
    }
    if ( (naiStrSize > 72) &&
         (naiStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting naiStrSize from %" PRIuS " to 72", naiStrSize);
        naiStrSize = 72;
    }
    if (naiStrSize >= UINT32_MAX)
    {
        naiStrSize = UINT32_MAX;
    }
    else
    {
        naiStrSize++;
    }

    // Define storage for output parameters
    char naiStrBuffer[73] = { 0 };
    char *naiStr = naiStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        naiStr = NULL;
        naiStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetNai ( 
        naiStr, 
        naiStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (naiStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      naiStr, 72 ));
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


static void Handle_le_info_GetManufacturerName
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
    size_t mfrNameStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &mfrNameStrSize ))
    {
        goto error_unpack;
    }
    if ( (mfrNameStrSize > 128) &&
         (mfrNameStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting mfrNameStrSize from %" PRIuS " to 128", mfrNameStrSize);
        mfrNameStrSize = 128;
    }
    if (mfrNameStrSize >= UINT32_MAX)
    {
        mfrNameStrSize = UINT32_MAX;
    }
    else
    {
        mfrNameStrSize++;
    }

    // Define storage for output parameters
    char mfrNameStrBuffer[129] = { 0 };
    char *mfrNameStr = mfrNameStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        mfrNameStr = NULL;
        mfrNameStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetManufacturerName ( 
        mfrNameStr, 
        mfrNameStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (mfrNameStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      mfrNameStr, 128 ));
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


static void Handle_le_info_GetPriId
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
    size_t priIdPnStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &priIdPnStrSize ))
    {
        goto error_unpack;
    }
    if ( (priIdPnStrSize > 16) &&
         (priIdPnStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting priIdPnStrSize from %" PRIuS " to 16", priIdPnStrSize);
        priIdPnStrSize = 16;
    }
    if (priIdPnStrSize >= UINT32_MAX)
    {
        priIdPnStrSize = UINT32_MAX;
    }
    else
    {
        priIdPnStrSize++;
    }
    size_t priIdRevStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &priIdRevStrSize ))
    {
        goto error_unpack;
    }
    if ( (priIdRevStrSize > 16) &&
         (priIdRevStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting priIdRevStrSize from %" PRIuS " to 16", priIdRevStrSize);
        priIdRevStrSize = 16;
    }
    if (priIdRevStrSize >= UINT32_MAX)
    {
        priIdRevStrSize = UINT32_MAX;
    }
    else
    {
        priIdRevStrSize++;
    }

    // Define storage for output parameters
    char priIdPnStrBuffer[17] = { 0 };
    char *priIdPnStr = priIdPnStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        priIdPnStr = NULL;
        priIdPnStrSize = 0;
    }
    char priIdRevStrBuffer[17] = { 0 };
    char *priIdRevStr = priIdRevStrBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        priIdRevStr = NULL;
        priIdRevStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetPriId ( 
        priIdPnStr, 
        priIdPnStrSize, 
        priIdRevStr, 
        priIdRevStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (priIdPnStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      priIdPnStr, 16 ));
    }
    if (priIdRevStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      priIdRevStr, 16 ));
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


static void Handle_le_info_GetCarrierPri
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
    size_t capriNameStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &capriNameStrSize ))
    {
        goto error_unpack;
    }
    if ( (capriNameStrSize > 16) &&
         (capriNameStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting capriNameStrSize from %" PRIuS " to 16", capriNameStrSize);
        capriNameStrSize = 16;
    }
    if (capriNameStrSize >= UINT32_MAX)
    {
        capriNameStrSize = UINT32_MAX;
    }
    else
    {
        capriNameStrSize++;
    }
    size_t capriRevStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &capriRevStrSize ))
    {
        goto error_unpack;
    }
    if ( (capriRevStrSize > 12) &&
         (capriRevStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting capriRevStrSize from %" PRIuS " to 12", capriRevStrSize);
        capriRevStrSize = 12;
    }
    if (capriRevStrSize >= UINT32_MAX)
    {
        capriRevStrSize = UINT32_MAX;
    }
    else
    {
        capriRevStrSize++;
    }

    // Define storage for output parameters
    char capriNameStrBuffer[17] = { 0 };
    char *capriNameStr = capriNameStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        capriNameStr = NULL;
        capriNameStrSize = 0;
    }
    char capriRevStrBuffer[13] = { 0 };
    char *capriRevStr = capriRevStrBuffer;
    if (!(_requiredOutputs & (1u << 1)))
    {
        capriRevStr = NULL;
        capriRevStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetCarrierPri ( 
        capriNameStr, 
        capriNameStrSize, 
        capriRevStr, 
        capriRevStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (capriNameStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      capriNameStr, 16 ));
    }
    if (capriRevStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      capriRevStr, 12 ));
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


static void Handle_le_info_GetPlatformSerialNumber
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
    size_t platformSerialNumberStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &platformSerialNumberStrSize ))
    {
        goto error_unpack;
    }
    if ( (platformSerialNumberStrSize > 14) &&
         (platformSerialNumberStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting platformSerialNumberStrSize from %" PRIuS " to 14", platformSerialNumberStrSize);
        platformSerialNumberStrSize = 14;
    }
    if (platformSerialNumberStrSize >= UINT32_MAX)
    {
        platformSerialNumberStrSize = UINT32_MAX;
    }
    else
    {
        platformSerialNumberStrSize++;
    }

    // Define storage for output parameters
    char platformSerialNumberStrBuffer[15] = { 0 };
    char *platformSerialNumberStr = platformSerialNumberStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        platformSerialNumberStr = NULL;
        platformSerialNumberStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetPlatformSerialNumber ( 
        platformSerialNumberStr, 
        platformSerialNumberStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (platformSerialNumberStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      platformSerialNumberStr, 14 ));
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


static void Handle_le_info_GetRfDeviceStatus
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
    size_t manufacturedIdSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &manufacturedIdSize ))
    {
        goto error_unpack;
    }
    if ( (manufacturedIdSize > 16) )
    {
        LE_DEBUG("Adjusting manufacturedIdSize from %" PRIuS " to 16", manufacturedIdSize);
        manufacturedIdSize = 16;
    }
    size_t productIdSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &productIdSize ))
    {
        goto error_unpack;
    }
    if ( (productIdSize > 16) )
    {
        LE_DEBUG("Adjusting productIdSize from %" PRIuS " to 16", productIdSize);
        productIdSize = 16;
    }
    size_t statusSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &statusSize ))
    {
        goto error_unpack;
    }
    if ( (statusSize > 16) )
    {
        LE_DEBUG("Adjusting statusSize from %" PRIuS " to 16", statusSize);
        statusSize = 16;
    }

    // Define storage for output parameters
    uint16_t manufacturedIdBuffer[16] = { 0 };
    uint16_t *manufacturedIdPtr = manufacturedIdBuffer;
    size_t *manufacturedIdSizePtr = &manufacturedIdSize;
    if (!(_requiredOutputs & (1u << 0)))
    {
        manufacturedIdPtr = NULL;
    }
    uint8_t productIdBuffer[16] = { 0 };
    uint8_t *productIdPtr = productIdBuffer;
    size_t *productIdSizePtr = &productIdSize;
    if (!(_requiredOutputs & (1u << 1)))
    {
        productIdPtr = NULL;
    }
    bool statusBuffer[16] = { false };
    bool *statusPtr = statusBuffer;
    size_t *statusSizePtr = &statusSize;
    if (!(_requiredOutputs & (1u << 2)))
    {
        statusPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetRfDeviceStatus ( 
        manufacturedIdPtr, 
        &manufacturedIdSize, 
        productIdPtr, 
        &productIdSize, 
        statusPtr, 
        &statusSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (manufacturedIdPtr)
    {
        bool manufacturedIdResult;
            LE_PACK_PACKARRAY( &_msgBufPtr,
                           manufacturedIdPtr, (*manufacturedIdSizePtr),
                           16, le_pack_PackUint16,
                           &manufacturedIdResult );
        LE_ASSERT(manufacturedIdResult);
    }
    if (productIdPtr)
    {
        bool productIdResult;
            LE_PACK_PACKARRAY( &_msgBufPtr,
                           productIdPtr, (*productIdSizePtr),
                           16, le_pack_PackUint8,
                           &productIdResult );
        LE_ASSERT(productIdResult);
    }
    if (statusPtr)
    {
        bool statusResult;
            LE_PACK_PACKARRAY( &_msgBufPtr,
                           statusPtr, (*statusSizePtr),
                           16, le_pack_PackBool,
                           &statusResult );
        LE_ASSERT(statusResult);
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


static void Handle_le_info_GetSku
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
    size_t skuIdStrSize = 0;
    if (!le_pack_UnpackSize( &_msgBufPtr,
                             &skuIdStrSize ))
    {
        goto error_unpack;
    }
    if ( (skuIdStrSize > 32) &&
         (skuIdStrSize < UINT32_MAX) )
    {
        LE_DEBUG("Adjusting skuIdStrSize from %" PRIuS " to 32", skuIdStrSize);
        skuIdStrSize = 32;
    }
    if (skuIdStrSize >= UINT32_MAX)
    {
        skuIdStrSize = UINT32_MAX;
    }
    else
    {
        skuIdStrSize++;
    }

    // Define storage for output parameters
    char skuIdStrBuffer[33] = { 0 };
    char *skuIdStr = skuIdStrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        skuIdStr = NULL;
        skuIdStrSize = 0;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetSku ( 
        skuIdStr, 
        skuIdStrSize );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (skuIdStr)
    {
        LE_ASSERT(le_pack_PackString( &_msgBufPtr,
                                      skuIdStr, 32 ));
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


static void Handle_le_info_GetExpectedResetsCount
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
    uint64_t resetsCountPtrBuffer = 0;
    uint64_t *resetsCountPtrPtr = &resetsCountPtrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        resetsCountPtrPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetExpectedResetsCount ( 
        resetsCountPtrPtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (resetsCountPtrPtr)
    {
        LE_ASSERT(le_pack_PackUint64( &_msgBufPtr,
                                                      *resetsCountPtrPtr ));
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


static void Handle_le_info_GetUnexpectedResetsCount
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
    uint64_t resetsCountPtrBuffer = 0;
    uint64_t *resetsCountPtrPtr = &resetsCountPtrBuffer;
    if (!(_requiredOutputs & (1u << 0)))
    {
        resetsCountPtrPtr = NULL;
    }

    // Call the function
    le_result_t _result;
    _result  = le_info_GetUnexpectedResetsCount ( 
        resetsCountPtrPtr );

    // Re-use the message buffer for the response
    _msgBufPtr = _msgBufStartPtr;

    // Pack the result first
    LE_ASSERT(le_pack_PackResult( &_msgBufPtr, _result ));

    // Pack any "out" parameters
    if (resetsCountPtrPtr)
    {
        LE_ASSERT(le_pack_PackUint64( &_msgBufPtr,
                                                      *resetsCountPtrPtr ));
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
        case _MSGID_le_info_GetImei :
            Handle_le_info_GetImei(msgRef);
            break;
        case _MSGID_le_info_GetImeiSv :
            Handle_le_info_GetImeiSv(msgRef);
            break;
        case _MSGID_le_info_GetFirmwareVersion :
            Handle_le_info_GetFirmwareVersion(msgRef);
            break;
        case _MSGID_le_info_GetResetInformation :
            Handle_le_info_GetResetInformation(msgRef);
            break;
        case _MSGID_le_info_GetBootloaderVersion :
            Handle_le_info_GetBootloaderVersion(msgRef);
            break;
        case _MSGID_le_info_GetDeviceModel :
            Handle_le_info_GetDeviceModel(msgRef);
            break;
        case _MSGID_le_info_GetMeid :
            Handle_le_info_GetMeid(msgRef);
            break;
        case _MSGID_le_info_GetEsn :
            Handle_le_info_GetEsn(msgRef);
            break;
        case _MSGID_le_info_GetMdn :
            Handle_le_info_GetMdn(msgRef);
            break;
        case _MSGID_le_info_GetPrlVersion :
            Handle_le_info_GetPrlVersion(msgRef);
            break;
        case _MSGID_le_info_GetPrlOnlyPreference :
            Handle_le_info_GetPrlOnlyPreference(msgRef);
            break;
        case _MSGID_le_info_GetMin :
            Handle_le_info_GetMin(msgRef);
            break;
        case _MSGID_le_info_GetNai :
            Handle_le_info_GetNai(msgRef);
            break;
        case _MSGID_le_info_GetManufacturerName :
            Handle_le_info_GetManufacturerName(msgRef);
            break;
        case _MSGID_le_info_GetPriId :
            Handle_le_info_GetPriId(msgRef);
            break;
        case _MSGID_le_info_GetCarrierPri :
            Handle_le_info_GetCarrierPri(msgRef);
            break;
        case _MSGID_le_info_GetPlatformSerialNumber :
            Handle_le_info_GetPlatformSerialNumber(msgRef);
            break;
        case _MSGID_le_info_GetRfDeviceStatus :
            Handle_le_info_GetRfDeviceStatus(msgRef);
            break;
        case _MSGID_le_info_GetSku :
            Handle_le_info_GetSku(msgRef);
            break;
        case _MSGID_le_info_GetExpectedResetsCount :
            Handle_le_info_GetExpectedResetsCount(msgRef);
            break;
        case _MSGID_le_info_GetUnexpectedResetsCount :
            Handle_le_info_GetUnexpectedResetsCount(msgRef);
            break;

        default: LE_ERROR("Unknowm msg id = %" PRIu32 , msgPtr->id);
    }

    // Clear the client session ref associated with the current message, since the message
    // has now been processed.
    LE_CDATA_THIS->_ClientSessionRef = 0;
}