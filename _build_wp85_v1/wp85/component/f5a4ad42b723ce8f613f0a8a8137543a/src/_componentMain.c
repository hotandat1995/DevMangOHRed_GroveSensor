/*
 * AUTO-GENERATED _componentMain.c for the spiLibrary component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

// Component log session variables.
le_log_SessionRef_t spiLibrary_LogSession;
le_log_Level_t* spiLibrary_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _spiLibrary_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _spiLibrary_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _spiLibrary_Init(void)
{
    LE_DEBUG("Initializing spiLibrary component library.");

    // Register the component with the Log Daemon.
    spiLibrary_LogSession = log_RegComponent("spiLibrary", &spiLibrary_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_spiLibrary_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_spiLibrary_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
