/*
 * AUTO-GENERATED _componentMain.c for the groverColorComponent component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

// Component log session variables.
le_log_SessionRef_t groverColorComponent_LogSession;
le_log_Level_t* groverColorComponent_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _groverColorComponent_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _groverColorComponent_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _groverColorComponent_Init(void)
{
    LE_DEBUG("Initializing groverColorComponent component library.");

    // Register the component with the Log Daemon.
    groverColorComponent_LogSession = log_RegComponent("groverColorComponent", &groverColorComponent_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_groverColorComponent_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_groverColorComponent_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
