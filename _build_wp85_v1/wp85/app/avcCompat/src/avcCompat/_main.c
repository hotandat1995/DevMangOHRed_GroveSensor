
// Startup code for the executable 'avcCompat'.
// This is a generated file, do not edit.

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"
#include <dlfcn.h>


// Define IPC API interface names.
LE_SHARED const char* _appCfg_le_cfg_ServiceInstanceName = "avcCompat.appCfg.le_cfg";
LE_SHARED const char* _watchdogChain_le_wdog_ServiceInstanceName = "avcCompat.watchdogChain.le_wdog";
LE_SHARED const char* _avcCompat_le_avc_ServiceInstanceName = "avcCompat.avcCompat.le_avc";
LE_SHARED const char* _avcCompat_le_appCtrl_ServiceInstanceName = "avcCompat.avcCompat.le_appCtrl";
LE_SHARED const char* _avcCompat_le_cfg_ServiceInstanceName = "avcCompat.avcCompat.le_cfg";
LE_SHARED const char* _avcCompat_le_appInfo_ServiceInstanceName = "avcCompat.avcCompat.le_appInfo";

// Define default component's log session variables.
LE_SHARED le_log_SessionRef_t avcCompat_exe_LogSession;
LE_SHARED le_log_Level_t* avcCompat_exe_LogLevelFilterPtr;

// Loads a library using dlopen().
__attribute__((unused)) static void LoadLib
(
    const char* libName
)
{
    dlopen(libName, RTLD_LAZY | RTLD_GLOBAL);
    const char* errorMsg = dlerror();
    LE_FATAL_IF(errorMsg != NULL,
                "Failed to load library '%s' (%s)",                libName,
                errorMsg);
}


int main(int argc, const char* argv[])
{
    // Pass the args to the Command Line Arguments API.
    le_arg_SetArgs((size_t)argc, argv);
    // Make stdout line buffered so printf shows up in logs without flushing.
    setlinebuf(stdout);

    avcCompat_exe_LogSession = log_RegComponent("avcCompat_exe", &avcCompat_exe_LogLevelFilterPtr);

    // Connect to the log control daemon.
    // Note that there are some rare cases where we don't want the
    // process to try to connect to the Log Control Daemon (e.g.,
    // the Supervisor and the Service Directory shouldn't).
    // The NO_LOG_CONTROL macro can be used to control that.
    #ifndef NO_LOG_CONTROL
        log_ConnectToControlDaemon();
    #else
        LE_DEBUG("Not connecting to the Log Control Daemon.");
    #endif

    // Load dynamic libraries.
    LoadLib("libComponent_appCfg.so");
    LoadLib("libComponent_le_pa_avc_default.so");
    LoadLib("libComponent_swiQmi.so");
    LoadLib("libComponent_watchdogChain.so");
    LoadLib("libComponent_le_pa_fwupdate.so");
    LoadLib("libComponent_le_pa_avc.so");
    LoadLib("libComponent_avcCompat.so");

    // Set the Signal Fault handler
    le_sig_InstallShowStackHandler();

    // Set the Signal Term handler
    le_sig_InstallDefaultTermHandler();

    LE_DEBUG("== Starting Event Processing Loop ==");
    le_event_RunLoop();
    LE_FATAL("== SHOULDN'T GET HERE! ==");
}
