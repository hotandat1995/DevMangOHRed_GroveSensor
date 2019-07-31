/*
 * AUTO-GENERATED _componentMain.c for the alsa_intf component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

// Component log session variables.
le_log_SessionRef_t alsa_intf_LogSession;
le_log_Level_t* alsa_intf_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _alsa_intf_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _alsa_intf_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _alsa_intf_Init(void)
{
    LE_DEBUG("Initializing alsa_intf component library.");

    // Register the component with the Log Daemon.
    alsa_intf_LogSession = log_RegComponent("alsa_intf", &alsa_intf_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_alsa_intf_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_alsa_intf_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
