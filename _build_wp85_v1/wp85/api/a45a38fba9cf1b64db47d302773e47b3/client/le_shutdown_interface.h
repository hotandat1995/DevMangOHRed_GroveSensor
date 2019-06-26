

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_shutdown Power shutdown
 *
 * @ref le_shutdown_interface.h "API Reference"
 *
 * <HR>
 *
 * This interface provides an API to request a fast shutdown of the module.
 * Fast shutdown acts like an emergency shutdown where only critical procedures are performed
 * before power off. Ongoing write operations in flash, for instance, are completed properly before
 * power off. However, non-critical operations like network detaching are skipped.
 *
 * @section le_shutdown_binding IPC interfaces binding
 *
 * Here's a code sample binding to Power Manager services:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_shutdown -> powerMgr.le_shutdown
   }
   @endverbatim
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @file le_shutdown_interface.h
 *
 * Legato @ref c_shutdown include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_SHUTDOWN_INTERFACE_H_INCLUDE_GUARD
#define LE_SHUTDOWN_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_shutdown_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_shutdown_DisconnectHandler_t)(void *);

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
void le_shutdown_ConnectService
(
    void
);

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
le_result_t le_shutdown_TryConnectService
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Set handler called when server disconnection is detected.
 *
 * When a server connection is lost, call this handler then exit with LE_FATAL.  If a program wants
 * to continue without exiting, it should call longjmp() from inside the handler.
 */
//--------------------------------------------------------------------------------------------------
LE_FULL_API void le_shutdown_SetServerDisconnectHandler
(
    le_shutdown_DisconnectHandler_t disconnectHandler,
    void *contextPtr
);

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
void le_shutdown_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Issue a fast shutdown.
 *
 * @return
 *  - LE_OK            Function success
 *  - LE_FAULT         Internal error
 *  - LE_UNSUPPORTED   Feature not supported
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_shutdown_IssueFast
(
    void
);

#endif // LE_SHUTDOWN_INTERFACE_H_INCLUDE_GUARD