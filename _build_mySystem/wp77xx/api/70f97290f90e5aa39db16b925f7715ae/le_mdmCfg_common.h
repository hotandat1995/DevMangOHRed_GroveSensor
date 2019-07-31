
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_MDMCFG_COMMON_H_INCLUDE_GUARD
#define LE_MDMCFG_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_MDMCFG_PROTOCOL_ID "fcf3b5cece06794472fb938f2c621c50"
#define IFGEN_LE_MDMCFG_MSG_SIZE 12




//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_mdmCfg_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_mdmCfg_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_mdmCfg_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Store the modem current configuration.
 *
 * @return
 *    - LE_OK on success
 *    - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_mdmCfg_StoreCurrentConfiguration
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Restore previously saved the modem configuration.
 *
 * @return
 *    - LE_OK on success
 *    - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_mdmCfg_RestoreSavedConfiguration
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

#endif // LE_MDMCFG_COMMON_H_INCLUDE_GUARD