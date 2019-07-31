/*
 * AUTO-GENERATED _componentMain.c for the multiChannelRelayComponent component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _multiChannelRelayComponent_multiChannelRelay_ServiceInstanceName;
const char** multiChannelRelay_ServiceInstanceNamePtr = &_multiChannelRelayComponent_multiChannelRelay_ServiceInstanceName;
void multiChannelRelay_AdvertiseService(void);
// Component log session variables.
le_log_SessionRef_t multiChannelRelayComponent_LogSession;
le_log_Level_t* multiChannelRelayComponent_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _multiChannelRelayComponent_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _multiChannelRelayComponent_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _multiChannelRelayComponent_Init(void)
{
    LE_DEBUG("Initializing multiChannelRelayComponent component library.");

    // Advertise server-side IPC interfaces.
    multiChannelRelay_AdvertiseService();

    // Register the component with the Log Daemon.
    multiChannelRelayComponent_LogSession = log_RegComponent("multiChannelRelayComponent", &multiChannelRelayComponent_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_multiChannelRelayComponent_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_multiChannelRelayComponent_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
