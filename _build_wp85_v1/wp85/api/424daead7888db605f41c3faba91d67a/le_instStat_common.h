
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_INSTSTAT_COMMON_H_INCLUDE_GUARD
#define LE_INSTSTAT_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_limit_common.h"

#define IFGEN_LE_INSTSTAT_PROTOCOL_ID "e160a983e92b4310ae8af3fbeff75a1b"
#define IFGEN_LE_INSTSTAT_MSG_SIZE 59



//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_instStat_AppInstallEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_instStat_AppInstallEventHandler* le_instStat_AppInstallEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_instStat_AppUninstallEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_instStat_AppUninstallEventHandler* le_instStat_AppUninstallEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for application install.  Called when an application is installed.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_instStat_AppInstallHandlerFunc_t)
(
        const char* LE_NONNULL appName,
        ///< Name of the application installed.
        void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for application uninstall.  Called when an application is uninstalled.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_instStat_AppUninstallHandlerFunc_t)
(
        const char* LE_NONNULL appName,
        ///< Name of the application uninstalled.
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_instStat_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_instStat_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_instStat_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_instStat_AppInstallEvent'
 *
 * This event provides a notification of when an application is installed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_instStat_AppInstallEventHandlerRef_t ifgen_le_instStat_AddAppInstallEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_instStat_AppInstallHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_instStat_AppInstallEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_instStat_RemoveAppInstallEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_instStat_AppInstallEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_instStat_AppUninstallEvent'
 *
 * This event provides a notification of when an application is uninstalled.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_instStat_AppUninstallEventHandlerRef_t ifgen_le_instStat_AddAppUninstallEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_instStat_AppUninstallHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_instStat_AppUninstallEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_instStat_RemoveAppUninstallEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_instStat_AppUninstallEventHandlerRef_t handlerRef
        ///< [IN]
);

#endif // LE_INSTSTAT_COMMON_H_INCLUDE_GUARD