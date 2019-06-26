
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_AVC_COMMON_H_INCLUDE_GUARD
#define LE_AVC_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_limit_common.h"

#define IFGEN_LE_AVC_PROTOCOL_ID "1be3592ce61a4cf0fa0127a8de3820a0"
#define IFGEN_LE_AVC_MSG_SIZE 128



//--------------------------------------------------------------------------------------------------
/**
 * Maximum APN name length without NULL terminator.
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVC_APN_NAME_MAX_LEN 48

//--------------------------------------------------------------------------------------------------
/**
 * Maximum APN name length including NULL terminator.
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVC_APN_NAME_MAX_LEN_BYTES 49

//--------------------------------------------------------------------------------------------------
/**
 * Maximum user name length without NULL terminator.
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVC_USERNAME_MAX_LEN 28

//--------------------------------------------------------------------------------------------------
/**
 * Maximum user name length including NULL terminator.
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVC_USERNAME_MAX_LEN_BYTES 29

//--------------------------------------------------------------------------------------------------
/**
 * Maximum password length without NULL terminator..
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVC_PASSWORD_MAX_LEN 28

//--------------------------------------------------------------------------------------------------
/**
 * Maximum password length including NULL terminator.
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVC_PASSWORD_MAX_LEN_BYTES 29

//--------------------------------------------------------------------------------------------------
/**
 * Maximum number of retry timers.
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVC_NUM_RETRY_TIMERS 8

//--------------------------------------------------------------------------------------------------
/**
 * Default HTTP status.
 */
//--------------------------------------------------------------------------------------------------
#define LE_AVC_HTTP_STATUS_INVALID 65535

//--------------------------------------------------------------------------------------------------
/**
 * Status of session or update
 *
 * If an update is pending, it must first be downloaded and then installed.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVC_NO_UPDATE = 0,
        ///< No updates pending
    LE_AVC_DOWNLOAD_PENDING = 1,
        ///< Update pending download
    LE_AVC_DOWNLOAD_IN_PROGRESS = 2,
        ///< Download in progress
    LE_AVC_DOWNLOAD_COMPLETE = 3,
        ///< Download has completed
    LE_AVC_DOWNLOAD_FAILED = 4,
        ///< An error occurred downloading the update
    LE_AVC_INSTALL_PENDING = 5,
        ///< Install is pending (implies download complete)
    LE_AVC_INSTALL_IN_PROGRESS = 6,
        ///< Install in progress
    LE_AVC_INSTALL_COMPLETE = 7,
        ///< Update has been successfully installed
    LE_AVC_INSTALL_FAILED = 8,
        ///< An error occurred installing the update
    LE_AVC_UNINSTALL_PENDING = 9,
        ///< Uninstall is pending
    LE_AVC_UNINSTALL_IN_PROGRESS = 10,
        ///< Uninstall in progress
    LE_AVC_UNINSTALL_COMPLETE = 11,
        ///< App has been successfully uninstalled
    LE_AVC_UNINSTALL_FAILED = 12,
        ///< An error occurred uninstalling the update
    LE_AVC_SESSION_STARTED = 13,
        ///< Session with AV server started
    LE_AVC_SESSION_STOPPED = 14
        ///< Session with AV server stopped
}
le_avc_Status_t;


//--------------------------------------------------------------------------------------------------
/**
 * Request to open or close avms session.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVC_SESSION_ACQUIRE = 0,
        ///< Request by user app to open AV session
    LE_AVC_SESSION_RELEASE = 1
        ///< Request by user app to close AV session
}
le_avc_SessionRequest_t;


//--------------------------------------------------------------------------------------------------
/**
 * The type of pending update
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVC_UNKNOWN_UPDATE = 0,
        ///<
    LE_AVC_FIRMWARE_UPDATE = 1,
        ///<
    LE_AVC_FRAMEWORK_UPDATE = 2,
        ///<
    LE_AVC_APPLICATION_UPDATE = 3
        ///<
}
le_avc_UpdateType_t;


//--------------------------------------------------------------------------------------------------
/**
 * Error code used to provide diagnostic information after a failure (includes both download and
 * install failure).
 *
 * @note
 *     Additional information may also be available in the target device's system log.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVC_ERR_NONE = 0,
        ///< No error.
    LE_AVC_ERR_BAD_PACKAGE = 1,
        ///< Encountered a bad package.
    LE_AVC_ERR_INTERNAL = 2,
        ///< Something failed while doing install/download.
    LE_AVC_ERR_SECURITY_FAILURE = 3
        ///< Security check failure while installing the package.
}
le_avc_ErrorCode_t;


//--------------------------------------------------------------------------------------------------
/**
 * Session type indicates whether the device is connected to the bootstrap server or the
 * device management server.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_AVC_BOOTSTRAP_SESSION = 0,
        ///< Bootstrap session.
    LE_AVC_DM_SESSION = 1,
        ///< Device Management session.
    LE_AVC_SESSION_INVALID = 2
        ///< Session type invalid.
}
le_avc_SessionType_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_avc_StatusEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_avc_StatusEventHandler* le_avc_StatusEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_avc_SessionRequestEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_avc_SessionRequestEventHandler* le_avc_SessionRequestEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by BlockInstall function and used by UnblockInstall function
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_avc_BlockRequest* le_avc_BlockRequestRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for update availability status
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_avc_StatusHandlerFunc_t)
(
        le_avc_Status_t updateStatus,
        ///< status of pending update, if available
        int32_t totalNumBytes,
        ///< Total number of bytes to be downloaded
        ///< only valid when updateStatus is one of
        ///< DOWNLOAD_PENDING, DOWNLOAD_IN_PROGRESS or DOWNLOAD_COMPLETE.
        ///< returns -1 if value is unknown
        int32_t dloadProgress,
        ///< download completion in percentage
        ///< only valid when updateStatus is one of
        ///< DOWNLOAD_PENDING, DOWNLOAD_IN_PROGRESS or DOWNLOAD_COMPLETE.
        ///< returns -1 if value is unknown
        void* contextPtr
        ///<
);

//--------------------------------------------------------------------------------------------------
/**
 * Handler for receiving session open or close request.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_avc_SessionRequestHandlerFunc_t)
(
        le_avc_SessionRequest_t request,
        ///< Request to open or close AV session
        void* contextPtr
        ///<
);


//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_avc_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avc_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_avc_StatusEvent'
 *
 * This event provides information on update availability status
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_avc_StatusEventHandlerRef_t ifgen_le_avc_AddStatusEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avc_StatusHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_avc_StatusEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avc_RemoveStatusEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avc_StatusEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_avc_SessionRequestEvent'
 *
 * This event provides information on session open or close request.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_avc_SessionRequestEventHandlerRef_t ifgen_le_avc_AddSessionRequestEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avc_SessionRequestHandlerFunc_t handlerPtr,
        ///< [IN]
        void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_avc_SessionRequestEvent'
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avc_RemoveSessionRequestEventHandler
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avc_SessionRequestEventHandlerRef_t handlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Start a session with the AirVantage server
 *
 * This will cause a query to be sent to the server, for pending updates. If a download was
 * previously suspended, then this resumes the download.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 *      - LE_DUPLICATE if already connected to AirVantage server.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_StartSession
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Stop a session with the AirVantage server
 *
 * If a download is in progress, then this suspends the download.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_StopSession
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Send a specific message to the server to be sure that the route between the device and the server
 * is available.
 * This API needs to be called when any package download is over (successfully or not) and before
 * sending any notification on asset data to the server.
 *
 * @return
 *      - LE_OK when the treatment is launched
 *      - LE_FAULT on failure
 *      - LE_UNSUPPORTED when the API is not supported
 *
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_CheckRoute
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Accept the currently pending download
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_AcceptDownload
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Defer the currently pending download, for the given number of minutes
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_DeferDownload
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t deferMinutes
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Accept the currently pending install
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_AcceptInstall
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Defer the currently pending install
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_DeferInstall
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t deferMinutes
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Accept the currently pending uninstall
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_AcceptUninstall
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Defer the currently pending uninstall
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_DeferUninstall
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t deferMinutes
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the update type of the currently pending update
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT if not available
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_GetUpdateType
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avc_UpdateType_t* updateTypePtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the name for the currently pending app update
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT if not available, or isn't APPL_UPDATE type
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_GetAppUpdateName
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* updateName,
        ///< [OUT]
        size_t updateNameSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Prevent any pending updates from being installed.
 *
 * @return
 *      - Reference for block update request (to be used later for unblocking updates)
 *      - NULL if the operation was not successful
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_avc_BlockRequestRef_t ifgen_le_avc_BlockInstall
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Allow any pending updates to be installed
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_avc_UnblockInstall
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_avc_BlockRequestRef_t blockRef
        ///< [IN] block request ref returned by le_avc_BlockInstall
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to get error code when update fails.
 *
 * @return
 *      - Error code of encountered error.
 *      - ERR_NONE if update is in any other state.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_avc_ErrorCode_t ifgen_le_avc_GetErrorCode
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to read the current session type, or the last session type if there is no
 * active session.
 *
 * @return
 *      - SessionType
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_avc_SessionType_t ifgen_le_avc_GetSessionType
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to read the http status of the last download.
 *
 * @return
 *      - HttpStatus as defined in RFC 7231, Section 6.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED uint16_t ifgen_le_avc_GetHttpStatus
(
    le_msg_SessionRef_t _ifgen_sessionRef
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to read the polling timer.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT if not available
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_GetPollingTimer
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t* pollingTimerPtr
        ///< [OUT] Polling timer (0 to 525600 minutes).
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to read the retry timers.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if not able to read the timers.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_GetRetryTimers
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint16_t* timerValuePtr,
        ///< [OUT] Array of the retry timers.
        size_t* timerValueSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to read APN configuration.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if there is any error while reading.
 *      - LE_OVERFLOW if the buffer provided is too small.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_GetApnConfig
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* apnName,
        ///< [OUT]
        size_t apnNameSize,
        ///< [IN]
        char* userName,
        ///< [OUT]
        size_t userNameSize,
        ///< [IN]
        char* userPwd,
        ///< [OUT]
        size_t userPwdSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to write APN configuration.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if not able to write the APN configuration.
 *      - LE_OVERFLOW if one of the input strings is too long.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_SetApnConfig
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL apnName,
        ///< [IN]
        const char* LE_NONNULL userName,
        ///< [IN]
        const char* LE_NONNULL userPwd
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to set the polling timer.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if not able to read the timers.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_SetPollingTimer
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint32_t pollingTimer
        ///< [IN] Polling timer (0 to 525600 minutes).
);

//--------------------------------------------------------------------------------------------------
/**
 * Function to set the retry timers.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if not able to write the timers.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_avc_SetRetryTimers
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const uint16_t* timerValuePtr,
        ///< [IN] Array of 8 retry timers.
        size_t timerValueSize
        ///< [IN]
);

#endif // LE_AVC_COMMON_H_INCLUDE_GUARD