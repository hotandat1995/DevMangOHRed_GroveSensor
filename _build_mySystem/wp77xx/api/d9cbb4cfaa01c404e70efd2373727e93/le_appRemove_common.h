
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_APPREMOVE_COMMON_H_INCLUDE_GUARD
#define LE_APPREMOVE_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_limit_common.h"

#define IFGEN_LE_APPREMOVE_PROTOCOL_ID "13c5300b39cc08587796fd4b47183f92"
#define IFGEN_LE_APPREMOVE_MSG_SIZE 59




//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_appRemove_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_appRemove_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_appRemove_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Removes a given app from the target device.
 *
 * @return
 *     - LE_OK if successful
 *     - LE_BUSY if system busy.
 *     - LE_NOT_FOUND if given app is not installed.
 *     - LE_FAULT for any other failure.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_appRemove_Remove
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL appName
        ///< [IN] Name of the app to remove.
);

#endif // LE_APPREMOVE_COMMON_H_INCLUDE_GUARD