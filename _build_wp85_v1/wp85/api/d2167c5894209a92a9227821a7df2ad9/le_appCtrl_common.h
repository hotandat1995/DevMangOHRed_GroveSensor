
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_APPCTRL_COMMON_H_INCLUDE_GUARD
#define LE_APPCTRL_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_limit_common.h"

#define IFGEN_LE_APPCTRL_PROTOCOL_ID "4d849724ee91cb561bc0059af7eead96"
#define IFGEN_LE_APPCTRL_MSG_SIZE 533



//--------------------------------------------------------------------------------------------------
/**
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_appCtrl_App* le_appCtrl_AppRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_appCtrl_TraceAttach'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_appCtrl_TraceAttachHandler* le_appCtrl_TraceAttachHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for attaching to a process that is to be traced.  The process is blocked allowing the
 * tracer to attach to it.  The tracer must call TraceUnblock() to unblock the traced process.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_appCtrl_TraceAttachHandlerFunc_t)
(
        le_appCtrl_AppRef_t appRef,
        ///< Ref to the app.
        int32_t pid,
        ///< PID of a process that can be attached to.
        const char* LE_NONNULL procName,
        ///< Name of the process name.
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_appCtrl_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_appCtrl_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_appCtrl_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Gets a reference to an app.
 *
 * @return
 *      Reference to the named app.
 *      NULL on error (check logs for errors).
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_appCtrl_AppRef_t ifgen_le_appCtrl_GetRef
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL appName
        ///< [IN] Name of the app to get the ref for.
);

//--------------------------------------------------------------------------------------------------
/**
 * Release the reference to an app, resetting all overrides set for this app using other
 * functions, like SetRun(), SetDevicePerm(), etc.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_appCtrl_ReleaseRef
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_appCtrl_AppRef_t appRef
        ///< [IN] Ref to the app.
);

//--------------------------------------------------------------------------------------------------
/**
 * Sets the run flag for a process in an app.
 *
 * If there is an error this function will kill the calling client.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_appCtrl_SetRun
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
        const char* LE_NONNULL procName,
        ///< [IN] Process name to set the run flag for.
        bool run
        ///< [IN] Flag to run the process or not.
);

//--------------------------------------------------------------------------------------------------
/**
 * Sets the debug flag for a process in an app.
 *
 * If there is an error this function will kill the calling client.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_appCtrl_SetDebug
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
        const char* LE_NONNULL procName,
        ///< [IN] Process name to set the run flag for.
        bool debug
        ///< [IN] Flag to debug the process or not.
);

//--------------------------------------------------------------------------------------------------
/**
 * Imports a file into the app's working directory.
 *
 * @return
 *      LE_OK if successfully imported the file.
 *      LE_DUPLICATE if the path conflicts with items already in the app's working directory.
 *      LE_NOT_FOUND if the path does not point to a valid file.
 *      LE_BAD_PARAMETER if the path is formatted incorrectly.
 *      LE_FAULT if there was some other error.
 *
 * @note If the caller is passing an invalid reference to the app, it is a fatal error,
 *       the function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_appCtrl_Import
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
        const char* LE_NONNULL path
        ///< [IN] Absolute path to the file to import.
);

//--------------------------------------------------------------------------------------------------
/**
 * Sets a device file's permissions.
 *
 * @return
 *      LE_OK if successfully set the device's permissions.
 *      LE_NOT_FOUND if the path does not point to a valid device.
 *      LE_BAD_PARAMETER if the path is formatted incorrectly.
 *      LE_FAULT if there was some other error.
 *
 * @note If the caller is passing an invalid reference to the app, it is a fatal error,
 *       the function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_appCtrl_SetDevicePerm
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
        const char* LE_NONNULL path,
        ///< [IN] Absolute path to the device.
        const char* LE_NONNULL permissions
        ///< [IN] Permission string, "r", "w", "rw".
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_appCtrl_TraceAttach'
 *
 * Event that indicates the process is blocked and can be attached to.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_appCtrl_TraceAttachHandlerRef_t ifgen_le_appCtrl_AddTraceAttachHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
        le_appCtrl_TraceAttachHandlerFunc_t attachToPidPtr,
        ///< [IN] Attach handler to register.
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_appCtrl_TraceAttach'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_appCtrl_RemoveTraceAttachHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_appCtrl_TraceAttachHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Unblocks the traced process.  This should normally be done once the tracer has successfully
 * attached to the process.
 *
 * @note If the caller is passing an invalid reference to the app, it is a fatal error,
 *       the function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_appCtrl_TraceUnblock
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_appCtrl_AppRef_t appRef,
        ///< [IN] Ref to the app.
        int32_t pid
        ///< [IN] PID of the process to unblock.
);

//--------------------------------------------------------------------------------------------------
/**
 * Starts an app.
 *
 * @return
 *      LE_OK if the app is successfully started.
 *      LE_DUPLICATE if the app is already running.
 *      LE_NOT_FOUND if the app isn't installed.
 *      LE_FAULT if there was an error and the app could not be launched.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_appCtrl_Start
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL appName
        ///< [IN] Name of the app to start.
);

//--------------------------------------------------------------------------------------------------
/**
 * Stops an app.
 *
 * @return
 *      LE_OK if successful.
 *      LE_NOT_FOUND if the app could not be found.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_appCtrl_Stop
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL appName
        ///< [IN] Name of the app to stop.
);

#endif // LE_APPCTRL_COMMON_H_INCLUDE_GUARD