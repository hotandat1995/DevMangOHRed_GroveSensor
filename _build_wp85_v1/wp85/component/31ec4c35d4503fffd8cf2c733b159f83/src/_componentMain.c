/*
 * AUTO-GENERATED _componentMain.c for the dcsWifi component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _dcsWifi_le_wifiClient_ServiceInstanceName;
const char** le_wifiClient_ServiceInstanceNamePtr = &_dcsWifi_le_wifiClient_ServiceInstanceName;
void le_wifiClient_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t dcsWifi_LogSession;
le_log_Level_t* dcsWifi_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _dcsWifi_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _dcsWifi_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _dcsWifi_Init(void)
{
    LE_DEBUG("Initializing dcsWifi component library.");

    // Connect client-side IPC interfaces.
    // 'le_wifiClient' is [manual-start].

    // Register the component with the Log Daemon.
    dcsWifi_LogSession = log_RegComponent("dcsWifi", &dcsWifi_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_dcsWifi_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_dcsWifi_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
