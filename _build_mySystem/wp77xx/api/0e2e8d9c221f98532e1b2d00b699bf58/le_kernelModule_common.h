
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_KERNELMODULE_COMMON_H_INCLUDE_GUARD
#define LE_KERNELMODULE_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_KERNELMODULE_PROTOCOL_ID "b9a021649e1b6d35ccdd0293014c964b"
#define IFGEN_LE_KERNELMODULE_MSG_SIZE 72



//--------------------------------------------------------------------------------------------------
/**
 * @file le_kernelModule_interface.h
 *
 * Legato @ref c_kernelModule include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */
//--------------------------------------------------------------------------------------------------
#define LE_KERNELMODULE_NAME_LEN 60

//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
#define LE_KERNELMODULE_NAME_LEN_BYTES 61


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_kernelModule_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_kernelModule_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_kernelModule_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
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
LE_SHARED le_result_t ifgen_le_kernelModule_Load
(
    le_msg_SessionRef_t _ifgen_sessionRef,
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
LE_SHARED le_result_t ifgen_le_kernelModule_Unload
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL moduleName
        ///< [IN] Name of the module to unload.
);

#endif // LE_KERNELMODULE_COMMON_H_INCLUDE_GUARD