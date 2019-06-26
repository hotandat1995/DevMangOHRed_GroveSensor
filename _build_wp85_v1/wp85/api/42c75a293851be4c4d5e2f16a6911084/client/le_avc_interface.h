

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_le_avc AirVantage Connector API
 *
 * @ref le_avc_interface.h "API Reference" <br>
 * @ref legatoServicesAirVantage "AirVantage Connector Platform Service"
 *
 *
 * The AirVantage connector service provides an API to communicate with the AirVantage Server to
 * download and install updates.
 *
 * @section c_le_avc_update Firmware/Application Updates
 *
 * The API for firmware/application update is divided into two parts:
 *  - Allow an App to contact the server for pending updates. If available,
 *    can select to download or install the update.
 *  - Allow an App to block an update from being installed. An App may need this if it's
 *    performing a critical operation that can't be interrupted (e.g., eCall). This is necessary
 *    as installing an update will cause the App to restart (either the App itself needs to restart
 *    or a firmware change causes the modem to reset).
 *
 * @subsection c_le_avc_user_agreement User Agreement
 *
 * By default, user agreements are disabled. This means that avcService automatically accepts
 * requests from the server without asking for user approval: it initiates a connection to the
 * server, download/install packages and reboots the target if needed. Thus, if no control app for
 * the AirVantage service is present on the target, the daemon is still able to update the target.
 *
 * User agreements are persistent to reboot/update, they are stored in the file system. The default
 * settings are only applied when the target boots the first time or when the configuration file is
 * missing or corrupted. In all other cases, default configuration never overwrites the current one.
 *
 * When writing a control app for the AirVantage Service, it is up to the developer to ensure that
 * user agreements have been enabled for all actions the control app performs. Otherwise, avcService
 * will automatically accept server requests. User agreements can be retrieved and updated using
 * le_avc_GetUserAgreement() and le_avc_SetUserAgreement().
 *
 * If user agreements are enabled and avcService can't communicate with the control app to report
 * the pending requests, it waits for 3 minutes and tries again.
 *
 * There are 5 events that require user agreements:
 * - LE_AVC_CONNECTION_PENDING
 * - LE_AVC_DOWNLOAD_PENDING
 * - LE_AVC_INSTALL_PENDING
 * - LE_AVC_REBOOT_PENDING
 * - LE_AVC_UNINSTALL_PENDING
 *
 * An app can respond to these user agreement notifications by opening a connection if the
 * notification is connection pending or by accepting or deferring the operation if the notification
 * is one of download, install, uninstall or reboot pending.
 * If the user agreement is not accepted or deferred, a new notification is returned 30 minutes
 * later.
 *
 * Whether avcService should forward these notifications to an app or act on these notifications by
 * itself is determined by the individual configuration flags that enables or disables user
 * agreement for these operations. If the user agreement flag is turned off for an operation,
 * the default action by avcService is to automatically accept that pending operation. An app can
 * register its handler by calling le_avc_AddStatusEventHandler() described in
 * c_le_avc_update_control and the configuration flags can be set by le_avc_SetUserAgreement().
 *
 * Example of enabling user agreement for LE_AVC_DOWNLOAD_PENDING:
 * @code
 *        le_result_t result;
 *        result = le_avc_SetUserAgreement(LE_AVC_USER_AGREEMENT_DOWNLOAD, true);
 *
 *        if (result != LE_OK)
 *        {
 *            LE_ERROR("Failed to enable user agreement for download operation");
 *        }
 * @endcode
 *
 * @subsection c_le_avc_update_control Update Control
 *
 * Any App can start a session with an AirVantage server to determine if there
 * is a pending update. This is done with le_avc_StartSession().
 *
 * The current session can be stopped using le_avc_StopSession(). This suspends a download in
 * progress if the user agreement for download is activated and change its state to download
 * pending. Otherwise if the user agreement for download is not activated, a new connection is
 * immediatly initiated by AVC in order to resume the download. However, it <b>won't stop</b>
 * an install in progress.
 * The status of a pending update is sent to all Apps registered for receiving a notification.
 * An App can use le_avc_AddStatusEventHandler() to register a handler function to
 * receive this notification. The notification will be received after a session is started.
 *
 * Example of registering an AVC handler and starting a session with fault checking:
 * @code
 *    // Start AVC Session
 *    LE_INFO("AirVantage Connection Controller started.");
 *    le_avc_AddStatusEventHandler(avcStatusHandler, NULL);    //register a AVC handler
 *
 *    // Start AVC session. Note: AVC handler must be registered prior starting a session
 *    le_result_t result = le_avc_StartSession();
 *    if (LE_FAULT == result)
 *    {
 *        le_avc_StopSession();
 *        le_avc_StartSession();
 *    }
 * @endcode
 *
 * An App can use le_avc_AddCommInfoHandler() to register a handler function and receive
 * communication information notifications. This function returns a reference. To unregister
 * the handler, le_avc_RemoveCommInfoHandler() should be called with the previously returned
 * reference.
 *
 *
 * Sometimes, @c avcService may decide to start a session with the AirVantage
 * server; for instance if a call to le_avc_AcceptDownload() when the session is stopped,
 * @c avcService will open a session to proceed with the download. In this case, a session started
 * notification could also be received, even if le_avc_StartSession() is not called explicitly.
 *
 * @subsubsection c_le_avc_updateControl_pending Pending Updates
 *
 * There can only be one pending update at a time.  To query the type of update, use
 * le_avc_GetUpdateType().  App updates can call le_avc_GetAppUpdateName() to retrieve the App name.
 *
 * If a download is pending, le_avc_AcceptDownload() can be used to allow the update to be
 * downloaded. An AirVantage session will be initiated if this api is called while there is no
 * active AirVantage session. le_avc_DeferDownload() can be used to defer the download for the
 * specified number of minutes. After the defer time has elapsed, the pending download notification
 * will be re-sent again to all registered apps. le_avc_AcceptDownload() can be used to accept the
 * download even before the defer timer expires. This behaviour is true for deferring install
 * and uninstall as well.
 *
 * Once an update has been downloaded, a new notification will be received to indicate that an
 * install is pending. Apps can then use le_avc_AcceptInstall() to allow the install to proceed.
 *
 * If an uninstall is pending, then le_avc_AcceptUninstall() can be used to allow the uninstall to
 * proceed. To defer the decision, le_avc_DeferUninstall() can be used to defer the uninstall for
 * the specified number of minutes. In case of an upgrade, the existing application will not be
 * uninstalled after le_avc_AcceptUninstall() is called. le_avc_AcceptUninstall() is only used to
 * signal the server to start downloading the new application. To proceed with an upgrade process,
 * accept the uninstall of the existing version followed by accepting the download and install of
 * the new version.
 *
 * @code
 *   switch (updateStatus)
 *        {
 *            case LE_AVC_DOWNLOAD_PENDING:
 *                LE_INFO("Accepting %s update.", GetUpdateType());
 *                res = le_avc_AcceptDownload();
 *                if (res != LE_OK)
 *                {
 *                    LE_ERROR("Failed to accept download from AirVantage (%s)", LE_RESULT_TXT(res));
 *                }
 *                break;
 *
 *            case LE_AVC_INSTALL_PENDING:
 *                LE_INFO("Accepting %s installation.", GetUpdateType());
 *                res = le_avc_AcceptInstall();
 *                if (res != LE_OK)
 *                {
 *                    LE_ERROR("Failed to accept install from AirVantage (%s)", LE_RESULT_TXT(res));
 *                }
 *                break;
 *
 *            case LE_AVC_UNINSTALL_PENDING:
 *                LE_INFO("Accepting %s uninstall.", GetUpdateType());
 *                res = le_avc_AcceptUninstall();
 *                if (res != LE_OK)
 *                {
 *                    LE_ERROR("Failed to accept uninstall from AirVantage (%s)", LE_RESULT_TXT(res));
 *                }
 *                break;
 *
 *            case LE_AVC_REBOOT_PENDING:
 *                LE_INFO("Accepting device reboot.");
 *                res = le_avc_AcceptReboot();
 *                if (res != LE_OK)
 *                {
 *                    LE_ERROR("Failed to accept reboot from AirVantage (%s)", LE_RESULT_TXT(res));
 *                }
 *                break;
 *
 *            default:
 *                // No action required
 *                break;
 *        }
 * @endcode
 *
 * @note Even if an App calls le_avc_AcceptInstall(), the install may still be blocked by another
 * App using the @ref c_le_avc_update_app functions. To defer the decision, an App can use
 * le_avc_DeferInstall() to defer the install for the specified number of minutes. After the defer
 * time has elapsed, the pending install notification will be re-sent to allow Apps to make a new
 * decision, or again defer.
 *
 * @subsubsection c_le_avc_updateControl_download_end Download and install processes
 *
 * When a package is fully downloaded from the AirVantage server, the device sends a specific
 * message to the server to follow the update process.
 * When the AirVantage server receives this message, it checks the download state and in case of
 * success, it sends the install request to the device.
 * If the user agreement for package install is activated, the @c LE_AVC_INSTALL_PENDING
 * notification is sent. The user needs to accept it in order to launch the install process on
 * device side.
 * If the user agreement for package install is deactivated, the install process on device side
 * is automatically launched.
 * After accepting the install or when the install is automatic, the install process is launched 2
 * seconds later by disconnecting the device from the AirVantage server if it's still connected and
 * by launching the install process (device reboot).
 *
 * @subsubsection c_le_avc_updateControl_accepting Accepting Installs/Uninstalls
 *
 * Accepting an App install or uninstall will not initiate an AirVantage session if no session
 * is active.  An App should start an AirVantage session before accepting an App install/uninstall,
 * to ensure the process is completed, and the server is updated.
 *
 * If no app has registered for notifications using le_avc_AddStatusEventHandler(),
 * then any pending downloads and installs will happen automatically, subject to any restrictions
 * imposed by app using the @ref c_le_avc_update_app functions.
 *
 * There is no restriction on the number of Apps registering for notifications.
 *
 * In case of any error incurred during App download/install, an error code will be set which can be
 * retrieved by calling le_avc_GetErrorCode().
 *
 * @subsection c_le_avc_update_app Application Installation Blocking
 *
 * When an App is about to perform a critical operation, it can block the installation of
 * an update with le_avc_BlockInstall(), and after it's finished with the critical operation, it
 * can unblock the install with le_avc_UnblockInstall().
 *
 * What constitutes a critical operation depends on the App.  An eCall App might
 * block installs for the duration that it runs.  A data collection App that wakes up
 * once an hour might block installs while it collects and stores and/or transmits a new data
 * sample, and then unblock installs just before it goes to sleep again.
 *
 * If an install can't be applied because it's blocked, another attempt to apply the install
 * will be made at a later time.
 *
 * An App can add a session control handler using le_avc_AddSessionRequestEventHandler(). Requests
 * by user Apps to open or close session will be forwarded to the session control handler. If no App
 * has registered a session request handler, AVC session can be opened or closed anytime by
 * user Apps.
 *
 * @subsection c_le_avc_suspend_resume Suspend and resume an update
 *
 * A firmware/application update can be interrupted or suspended by different events:
 * - a device reboot
 * - a network loss
 * - a manual suspend through a session stopped by calling le_avc_StopSession() function
 * - RAM issue
 *
 * After the update suspension, when the @c avcService is ready to resume the update process:
 * - If the user agreements are disabled, the update process is automatically restarted.
 * - If the user agreements are enabled, the previous @c PENDING notification is sent to the App,
 * as indicated in the table below.
 *
 * | Last received event            | Event sent to resume                                        | Expected action from App |
 * |--------------------------------|-------------------------------------------------------------|--------------------------|
 * | @c LE_AVC_DOWNLOAD_PENDING     | @c LE_AVC_DOWNLOAD_PENDING                                  | Accept download          |
 * | @c LE_AVC_DOWNLOAD_IN_PROGRESS | @c LE_AVC_DOWNLOAD_PENDING with remaining bytes to download | Accept download          |
 * | @c LE_AVC_DOWNLOAD_COMPLETE    | @c LE_AVC_DOWNLOAD_PENDING with zero bytes to download      | Accept download          |
 * | @c LE_AVC_INSTALL_PENDING      | @c LE_AVC_INSTALL_PENDING                                   | Accept install           |
 * | @c LE_AVC_INSTALL_IN_PROGRESS  | @c LE_AVC_INSTALL_PENDING                                   | Accept install           |
 * | @c LE_AVC_UNINSTALL_PENDING    | @c LE_AVC_UNINSTALL_PENDING                                 | Accept uninstall         |
 * | @c LE_AVC_CONNECTION_PENDING   | @c LE_AVC_CONNECTION_PENDING                                | Start session            |
 *
 * @note
 * - The @c avcService may not suspend the update process instantly (it typically takes a a few
 * minutes). The @c PENDING notification will only be sent when the suspention is finished. Trying
 * to resume the update process while a suspention is in progress will have no effect.
 * - If firmware is updated (via fwupdate tool) or new legato is installed (via instlegato),
 * all suspend/resume information stored by avcService is erased. So if developer updates firmware
 * or legato (via ethernet or ecm etc.) in the middle of any update initiated by avcService, this
 * need to be cancelled and restarted again from airVantage server.
 *
 * @subsubsection c_le_avc_suspend_resume_agreement Suspend/resume with activated user agreement
 *
 * When a user agreement for package download is activated and a download is suspended
 * (@c LE_AVC_DOWNLOAD_PENDING notification is sent to registered apps), an app can get the download
 * suspend reason by calling le_avc_GetErrorCode() and le_avc_GetHttpStatus() functions.
 * If the download was suspended because of an app request or if the @c LE_AVC_DOWNLOAD_PENDING
 * notification for the 1st time, the returned value by le_avc_GetErrorCode() function is
 * @c LE_AVC_ERR_NONE.
 *
 * If the user agreement for package download is not activated
 * - when a download is suspended by an app, the platform will automatically resume the download.
 * - when a download is suspended because of network loss, the platform will automatically resume
 * the download at next network registration.
 *
 * @subsection c_le_avc_update_failure Update failures
 *
 * A firmware/application update can fail if:
 * - package URL is not valid
 * - flash write issue
 * - in case of delta package, the downloaded package could not be compliant with the current
 * FW/App version
 * - package is not certified sent by a trusted server
 * - integrity check error
 * - install/uninstall error
 *
 * The package URL is considered as invalid when folowing HTTP error codes are received when the
 * package tries to be retrieved:
 * - 404: Not found
 * - 500: Internal Server Error
 * - 501: Not Implemented
 * - 505: HTTP version not supported
 *
 * | Failure reason        | Related event              | Function to be called to get more info |
 * |-----------------------|----------------------------|----------------------------------------|
 * | Invalid package URL   | @c LE_AVC_DOWNLOAD_FAILED  | le_avc_GetHttpStatus()                 |
 * | Flash write error     | @c LE_AVC_DOWNLOAD_FAILED  | le_avc_GetErrorCode()                  |
 * | Invalid delta package | @c LE_AVC_INSTALL_FAILED   | |
 * | integrity check error | @c LE_AVC_DOWNLOAD_FAILED  | |
 * | package not certified | @c LE_AVC_CERTIFICATION_KO | |
 * | install error         | @c LE_AVC_INSTALL_FAILED   | |
 * | uninstall error       | @c LE_AVC_UNINSTALL_FAILED | |
 *
 * Example on checking the download failure reason:
 * @code
 *        // In AVC event handler
 *        if (LE_AVC_DOWNLOAD_FAILED == updateStatus)
 *        {
 *            int avcError;
 *            int httpStatus;
 *
 *            LE_INFO("Download failed");
 *
 *            // Get the error cause
 *            avcError = le_avc_GetErrorCode();
 *            if (LE_AVC_ERR_NONE != avcError)
 *            {
 *                LE_WARN("Download failure %d", avcError);
 *            }
 *
 *            // Get the HTTP status code
 *            httpStatus = le_avc_GetHttpStatus();
 *            if ((200 != httpStatus) && (206 != httpStatus))
 *            {
 *                LE_WARN("HTTP status code %d", httpStatus);
 *            }
 *        }
 * @endcode
 *
 *
 * @section c_le_avc_Timers Timers
 *
 * Polling timers sets the time that the Target will communicate with the AirVantage Server to check
 * for new jobs.  Retry timers will try and re-establish a connection to the AirVantage Server in
 * accordance with the times that are declared.
 *
 * @subsection c_le_avc_PollingTimer Polling Timer
 *
 * The target will periodically initiate a connection to the AirVantage Server according to the
 * settings for the polling timer to check if there are any pending jobs.  The polling timer will
 * initiate a session when it starts the count (at 0 minutes) and then again at the specified time
 * set.
 *
 * To disable the polling timer call le_avc_SetPollingTimer() and set the value to 0.
 *
 * The polling timer accepts ranges from 0 to 525600 minutes.  The polling timer does not get reset
 * with reboots or power loss.  If the target is powered off during the polling time, it will
 * connect to the AVC Server upon startup of the app.  For example, if the polling timer is set to 1
 * hour then and the target reboots at the 20 minute mark, the polling timer will still initiate a
 * connection at the 1 hour mark.  If the target is powered off or in the middle of rebooting at the
 * 1 hour mark, as soon as the app is started again, the polling timer will initiate a connection to
 * the AirVantage Server.
 *
 * Polling timer initiated sessions will be disconnected after 20 seconds of inactivity. This does
 * not apply to AirVantage sessions that have been initiated by an app. The app is responsible for
 * disconnecting the session it initiates within a reasonable timeframe. Leaving the session open
 * for long period will result in power wastage.
 *
 * To read the polling timer call: le_avc_GetPollingTimer()
 *
 * To write a new value to the polling timer call: le_avc_SetPollingTimer()
 *
 * Writing to the polling timer stops the current polling timer if it is running and starts a timer
 * with the new value. The next connection will be initiated when the new polling timer reaches it's
 * set value.
 *
 * @subsection c_le_avc_RetryTimers Retry Timers
 *
 * If an error occurs during a connection to the Device Services server (WWAN DATA establishment
 * failed and an http error code is received) the embedded module will initiate a new connection
 * according to the values defined in the retry timers.
 *
 * The timers are tried in sequence until a connection is established, or all enabled retry timers
 * are exhausted. After all the enabled timers are exhausted, a new session must be initiated again
 * by calling le_avc_startSession() after the expiry of the retry timer.
 *
 * The retry timer values are persistent (reboots and updates do not effect the retry timers).
 * If you wish to disable a retry timer set the timer value
 * to 0.  You must always pass in at least 8 values to the retry timers.
 *
 * Retry timer values range from 0 to 20160 minutes.
 * The function le_avc_GetRetryTimers() reads the retry timers in an array and the function
 * le_avc_SetRetryTimers() writes the retry timers. When writing to the retry timers, values of
 * all the 8 timers have to be defined.
 *
 * Example of calling retry timers, the session will be retried after 15 minutes, 1 hour, 4 hours,
 * 8 hours, 1 day and 2 days, the last two retries are disabled:
 * @code
 *   uint16_t RetryTimers[LE_AVC_NUM_RETRY_TIMERS] = {15, 60, 240, 480, 1440, 2880, 0, 0};
 *   le_avc_SetRetryTimers(RetryTimers, LE_AVC_NUM_RETRY_TIMERS);
 * @endcode
 *
 * @section c_le_avc_reboot Device reboot
 *
 * The AirVantage server can request to reboot the device. If a reboot is requested a notification
 * is sent to the registered Apps. The App can either accept the reboot with le_avc_AcceptReboot()
 * or defer it for a specified number of minutes with le_avc_DeferReboot(). After the defer time
 * has elapsed, the pending reboot notification will be re-sent. This allows the registered app to
 * make a new decision or defer the reboot again.
 *
 * If no App has registered for notifications using le_avc_AddStatusEventHandler(), then
 * any pending reboot will happen automatically.
 *
 * @section c_le_avc_GetCredentialStatus Credential Status
 * The device is provisioned with bootstrap credentials from factory. The Device Management (DM)
 * credentials are provisioned by AirVantage Bootstrap Server. This API is used to retrieve the
 * status of credentials provisioned on the device.
 *
 * @section c_le_avc_connection Connection pending
 *
 * The AirVantage agent can request a connection to the AirVantage server, especially when a
 * firmware package is installed (after a platform reboot) or device reboots in the middle of
 * software update (after finishing software update on reboot). In this case a notification is
 * sent to the control App, which can start the connection with le_avc_StartSession(). If
 * the user agreement is enabled for the connection, and the user enabled the connection,
 * the connection will not  be automatically disconnected without explicit call to
 * le_avc_StopSession()
 *
 * @section c_le_avc_routing Data routing
 *
 * By default the AirVantage connection uses the default mobile data profile and the default route
 * set by the data connection service.
 *
 * If the user wishes to control the network configuration, e.g. to use the AirVantage agent with
 * multi-PDP contexts, they should first bind the application to the data connection service:
 * @verbatim
   bindings:
   {
      clientExe.clientComponent.le_data -> dataConnectionService.le_data
   }
   @endverbatim
 *
 * The data connection service should then be configured before launching the AirVantage connection:
 * - le_data_SetCellularProfileIndex() allows to change the data profile to use.
 * - le_data_GetDefaultRouteStatus() indicates if the default route is activated in the data
 * connection service. This default route can be deactivated in the data connection service
 * configuration database, as explained in @ref c_le_data_defaultRoute. If the default route is
 * deactivated, the AirVantage agent will automatically add routes to be able to reach the
 * AirVantage server through the connection used by AirVantage.
 *
 * @section c_le_avc_timeout Connection / Download timeout
 *
 * The AirVantage connector service will abort FOTA/SOTA download, if it takes more than
 * 300 seconds to establish a connection. Download will also be aborted, if the download speed is
 * too low (less than 100 bytes /second) for too long (for more than 1000 seconds).
 * These values are chosen based on experiments on low speed network. There is no configuration
 * for these timeouts.
 *
 * @section le_avcService_configdb Service Configuration Tree
 * @copydoc le_avcService_configdbPage_Hide
 *
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/**
 * @interface le_avcService_configdbPage_Hide
 *
 * The configuration database path for the activityTimeout is:
 * @verbatim
 /
     apps/
         avcService/
                  activityTimeout
 @endverbatim
 *
 *
 * After an AirVantage session is started, if there's no activity between the device and the server
 * within the timer interval, then LE_AVC_NO_UPDATE state will be returned to the app. However,
 * this activity timeout can be overridden by setting an integer value for
 * /apps/avcService/activityTimeout. The activity timer is initialized only when the @c avcService
 * starts. If a valid entry >0 is found, then it will be used instead of the default value of 20
 * seconds. The following steps should be used to set the activityTimeout.
 *
 *
 * @verbatim
 config set /apps/avcService/activityTimeout xx int
 app restart avcService
 @endverbatim
 *
 * @note
 * Everytime a new value is written to activityTimeout, the avcService needs to be
 * restarted to read the new value.
 *
 *
 */
/**
 * @file le_avc_interface.h
 *
 * Legato @ref c_le_avc include file.
 *
 * Copyright (C) Sierra Wireless Inc.
 */

#ifndef LE_AVC_INTERFACE_H_INCLUDE_GUARD
#define LE_AVC_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_limit_interface.h"

// Internal includes for this interface
#include "le_avc_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Type for handler called when a server disconnects.
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_avc_DisconnectHandler_t)(void *);

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
void le_avc_ConnectService
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
le_result_t le_avc_TryConnectService
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
LE_FULL_API void le_avc_SetServerDisconnectHandler
(
    le_avc_DisconnectHandler_t disconnectHandler,
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
void le_avc_DisconnectService
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