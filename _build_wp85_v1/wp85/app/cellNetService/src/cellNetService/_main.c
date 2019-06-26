
// Startup code for the executable 'cellNetService'.
// This is a generated file, do not edit.

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"
#include <dlfcn.h>


// Define IPC API interface names.
LE_SHARED const char* _watchdogChain_le_wdog_ServiceInstanceName = "cellNetService.watchdogChain.le_wdog";
LE_SHARED const char* _cellNetService_le_cellnet_ServiceInstanceName = "le_cellnet";
LE_SHARED const char* _cellNetService_le_sim_ServiceInstanceName = "cellNetService.cellNetService.le_sim";
LE_SHARED const char* _cellNetService_le_mrc_ServiceInstanceName = "cellNetService.cellNetService.le_mrc";
LE_SHARED const char* _cellNetService_le_secStore_ServiceInstanceName = "cellNetService.cellNetService.le_secStore";

// Define default component's log session variables.
LE_SHARED le_log_SessionRef_t cellNetService_exe_LogSession;
LE_SHARED le_log_Level_t* cellNetService_exe_LogLevelFilterPtr;

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

    cellNetService_exe_LogSession = log_RegComponent("cellNetService_exe", &cellNetService_exe_LogLevelFilterPtr);

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
    LoadLib("libComponent_watchdogChain.so");
    LoadLib("libComponent_cellNetService.so");

    // Set the Signal Fault handler
    le_sig_InstallShowStackHandler();

    // Set the Signal Term handler
    le_sig_InstallDefaultTermHandler();

    LE_DEBUG("== Starting Event Processing Loop ==");
    le_event_RunLoop();
    LE_FATAL("== SHOULDN'T GET HERE! ==");
}