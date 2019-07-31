

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_kernelModule Linux Kernel Module API
 *
 * @ref le_kernelModule_interface.h "API Reference"
 *
 * This API provides a way for applications to manually load and unload modules that were bundled
 * with their system.
 *
 * Module dependencies and running module load scripts are handled automatically.  Only the name of
 * the module in question is required.
 *
 * To load a module, call @c le_kernelModule_Load.  Unloading is similarly handled by
 * @c le_kernelModule_Unload
 *
 * An example for loading a module:
 *
 * @code
 * le_result_t result = le_kernelModule_Load(moduleName);
 *
 * LE_FATAL_IF(result != LE_OK, "Could not load the required module, %s.", moduleName);
 *
 * LE_INFO("Module, %s has been loaded.", moduleName);
 * @endcode
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_KERNELMODULE_INTERFACE_H_INCLUDE_GUARD
#define LE_KERNELMODULE_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_kernelModule_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_kernelModule_DisconnectHandler_t)(void *);

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
void le_kernelModule_ConnectService
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
le_result_t le_kernelModule_TryConnectService
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
LE_FULL_API void le_kernelModule_SetServerDisconnectHandler
(
    le_kernelModule_DisconnectHandler_t disconnectHandler,
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
void le_kernelModule_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Load the specified kernel module that was bundled with a Legato system.
 *
 * @return
 *   - LE_OK if the module has been successfully loaded into the kernel.
 *   - LE_NOT_FOUND if the named module was not found in the system.
 *   - LE_FAULT if errors were encountered when loading the module, or one of the module's
 *     dependencies.
 *   - LE_DUPLICATE if the module has been already loaded into the kernel.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_kernelModule_Load
(
    const char* LE_NONNULL moduleName
        ///< [IN] Name of the module to load.
);

//--------------------------------------------------------------------------------------------------
/**
 * Unload the specified module.  The module to be unloaded must be one that was bundled with the
 * system.
 *
 * @return
 *   - LE_OK if the module has been successfully unloaded from the kernel.
 *   - LE_NOT_FOUND if the named module was not found in the system.
 *   - LE_FAULT if errors were encountered during the module, or one of the module's dependencies
 *     unloading.
 *   - LE_DUPLICATE if the module has been already unloaded from the kernel.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_kernelModule_Unload
(
    const char* LE_NONNULL moduleName
        ///< [IN] Name of the module to unload.
);

#endif // LE_KERNELMODULE_INTERFACE_H_INCLUDE_GUARD