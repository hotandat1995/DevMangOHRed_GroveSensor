

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_AVC_INTERFACE_H_INCLUDE_GUARD
#define LE_AVC_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_limit_server.h"

// Internal includes for this interface
#include "le_avc_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_avc_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_avc_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_avc_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Status of session or update
 *
 * If an update is pending, it must first be downloaded and then installed.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Operations which require user agreement
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Request to open or close avms session.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * The type of pending update
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Error code used to provide diagnostic information after a failure (includes both download and
 * install failure).
 *
 * @note
 *     Additional information may also be available in the target device's system log.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Session type indicates whether the device is connected to the bootstrap server or the
 * device management server.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Status of the device credentials
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for update availability status
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_avc_StatusEvent'
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for receiving session open or close request.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_avc_SessionRequestEvent'
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for receiving communication information.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_avc_CommInfo'
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by BlockInstall function and used by UnblockInstall function
 */
//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_avc_StatusEvent'
 *
 * This event provides information on update availability status
 */
//--------------------------------------------------------------------------------------------------
le_avc_StatusEventHandlerRef_t le_avc_AddStatusEventHandler
(
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
void le_avc_RemoveStatusEventHandler
(
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
le_avc_SessionRequestEventHandlerRef_t le_avc_AddSessionRequestEventHandler
(
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
void le_avc_RemoveSessionRequestEventHandler
(
    le_avc_SessionRequestEventHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_avc_CommInfo'
 *
 * This event provides communication errors.
 */
//--------------------------------------------------------------------------------------------------
le_avc_CommInfoHandlerRef_t le_avc_AddCommInfoHandler
(
    le_avc_CommInfoHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_avc_CommInfo'
 */
//--------------------------------------------------------------------------------------------------
void le_avc_RemoveCommInfoHandler
(
    le_avc_CommInfoHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Start a session with the AirVantage server
 *
 * This will cause a query to be sent to the server, for pending updates.
 *
 * @return
 *      - LE_OK if connection request has been sent.
 *      - LE_FAULT on failure
 *      - LE_DUPLICATE if already connected.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_StartSession
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Stop a session with the AirVantage server
 *
 * If a download is in progress and the user agreement is enabled, this suspends the download,
 * otherwise if the user agreement is disabled, a new connection is automatically initiated in order
 * to resume the download.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 *      - LE_DUPLICATE if already disconnected
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_StopSession
(
    void
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
le_result_t le_avc_CheckRoute
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Defer the currently pending connection, for the given number of minutes
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_DeferConnect
(
    uint32_t deferMinutes
        ///< [IN]
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
le_result_t le_avc_AcceptDownload
(
    void
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
le_result_t le_avc_DeferDownload
(
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
le_result_t le_avc_AcceptInstall
(
    void
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
le_result_t le_avc_DeferInstall
(
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
le_result_t le_avc_AcceptUninstall
(
    void
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
le_result_t le_avc_DeferUninstall
(
    uint32_t deferMinutes
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Accept the currently pending reboot
 *
 * @note When this function is called, a 2-second timer is launched and the reboot function is
 * called when the timer expires.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_AcceptReboot
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Defer the currently pending reboot
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_DeferReboot
(
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
le_result_t le_avc_GetUpdateType
(
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
le_result_t le_avc_GetAppUpdateName
(
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
le_avc_BlockRequestRef_t le_avc_BlockInstall
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Allow any pending updates to be installed
 */
//--------------------------------------------------------------------------------------------------
void le_avc_UnblockInstall
(
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
le_avc_ErrorCode_t le_avc_GetErrorCode
(
    void
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
le_avc_SessionType_t le_avc_GetSessionType
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Function to read the http status of the last download.
 *
 * @return
 *      - HttpStatus as defined in RFC 7231, Section 6.
 */
//--------------------------------------------------------------------------------------------------
uint16_t le_avc_GetHttpStatus
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Function to read the polling timer.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT if not available
 *      - LE_OUT_OF_RANGE if the polling timer value is out of range (0 to 525600).
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_GetPollingTimer
(
    uint32_t* pollingTimerPtr
        ///< [OUT] Polling timer interval, minutes
);



//--------------------------------------------------------------------------------------------------
/**
 * Function to read the retry timers.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if not able to read the timers.
 *      - LE_OUT_OF_RANGE if one of the retry timers is out of range (0 to 20160).
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_GetRetryTimers
(
    uint16_t* timerValuePtr,
        ///< [OUT] Array of retry timer intervals, minutes.
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
le_result_t le_avc_GetApnConfig
(
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
 *      - LE_OVERFLOW if one of the input strings is too long.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_SetApnConfig
(
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
 *      - LE_OUT_OF_RANGE if the polling timer value is out of range (0 to 525600).
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_SetPollingTimer
(
    uint32_t pollingTimer
        ///< [IN] Polling timer interval, minutes
);



//--------------------------------------------------------------------------------------------------
/**
 * Function to set the retry timers.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if not able to set the timers.
 *      - LE_OUT_OF_RANGE if one of the retry timers is out of range (0 to 20160).
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_SetRetryTimers
(
    const uint16_t* timerValuePtr,
        ///< [IN] Array of retry timer intervals, minutes.
    size_t timerValueSize
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Function to retrieve status of the credentials provisioned on the device.
 *
 * @return
 *     LE_AVC_NO_CREDENTIAL_PROVISIONED
 *          - If neither Bootstrap nor Device Management credential is provisioned.
 *     LE_AVC_BS_CREDENTIAL_PROVISIONED
 *          - If Bootstrap credential is provisioned but Device Management credential is
 *              not provisioned.
 *     LE_AVC_DM_CREDENTIAL_PROVISIONED
 *          - If Device management key is provisioned.
 */
//--------------------------------------------------------------------------------------------------
le_avc_CredentialStatus_t le_avc_GetCredentialStatus
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Function to set user agreements for download, install, reboot, connection and uninstall.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if failed to configure user agreement.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_SetUserAgreement
(
    le_avc_UserAgreement_t updateStatus,
        ///< [IN] Operation for which user agreements has to be set.
    bool enable
        ///< [IN] true = enable, false = disable.
);



//--------------------------------------------------------------------------------------------------
/**
 * Function to get user agreements for download, install, reboot, connection and uninstall.
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if failed to read user agreement state.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_GetUserAgreement
(
    le_avc_UserAgreement_t updateStatus,
        ///< [IN] Operation for which user agreements has to be read.
    bool* enablePtr
        ///< [OUT] true = enable, false = disable.
);



//--------------------------------------------------------------------------------------------------
/**
 * Function to read a resource from a LWM2M object
 *
 * @return
 *      - LE_OK on success.
 *      - LE_FAULT if failed.
 *      - LE_UNSUPPORTED if unsupported.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_avc_ReadLwm2mResource
(
    uint16_t objectId,
        ///< [IN] Object identifier
    uint16_t objectInstanceId,
        ///< [IN] Object instance identifier
    uint16_t resourceId,
        ///< [IN] Resource identifier
    uint16_t resourceInstanceId,
        ///< [IN] Resource instance identifier
    char* data,
        ///< [OUT] String of requested resources to be read
    size_t dataSize
        ///< [IN]
);


#endif // LE_AVC_INTERFACE_H_INCLUDE_GUARD