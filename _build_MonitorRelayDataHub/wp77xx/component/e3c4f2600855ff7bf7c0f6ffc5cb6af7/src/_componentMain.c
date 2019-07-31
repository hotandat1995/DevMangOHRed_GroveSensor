/*
 * AUTO-GENERATED _componentMain.c for the MonitorRelayDataHubComponent component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _MonitorRelayDataHubComponent_le_avdata_ServiceInstanceName;
const char** le_avdata_ServiceInstanceNamePtr = &_MonitorRelayDataHubComponent_le_avdata_ServiceInstanceName;
void le_avdata_ConnectService(void);
extern const char* _MonitorRelayDataHubComponent_io_ServiceInstanceName;
const char** io_ServiceInstanceNamePtr = &_MonitorRelayDataHubComponent_io_ServiceInstanceName;
void io_ConnectService(void);
extern const char* _MonitorRelayDataHubComponent_admin_ServiceInstanceName;
const char** admin_ServiceInstanceNamePtr = &_MonitorRelayDataHubComponent_admin_ServiceInstanceName;
void admin_ConnectService(void);
extern const char* _MonitorRelayDataHubComponent_multiChannelRelay_ServiceInstanceName;
const char** multiChannelRelay_ServiceInstanceNamePtr = &_MonitorRelayDataHubComponent_multiChannelRelay_ServiceInstanceName;
void multiChannelRelay_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t MonitorRelayDataHubComponent_LogSession;
le_log_Level_t* MonitorRelayDataHubComponent_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _MonitorRelayDataHubComponent_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _MonitorRelayDataHubComponent_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _MonitorRelayDataHubComponent_Init(void)
{
    LE_DEBUG("Initializing MonitorRelayDataHubComponent component library.");

    // Connect client-side IPC interfaces.
    le_avdata_ConnectService();
    io_ConnectService();
    admin_ConnectService();
    multiChannelRelay_ConnectService();

    // Register the component with the Log Daemon.
    MonitorRelayDataHubComponent_LogSession = log_RegComponent("MonitorRelayDataHubComponent", &MonitorRelayDataHubComponent_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_MonitorRelayDataHubComponent_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_MonitorRelayDataHubComponent_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
