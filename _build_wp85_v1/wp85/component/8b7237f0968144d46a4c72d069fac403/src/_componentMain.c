/*
 * AUTO-GENERATED _componentMain.c for the printServer component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _printServer_printer_ServiceInstanceName;
const char** printer_ServiceInstanceNamePtr = &_printServer_printer_ServiceInstanceName;
void printer_AdvertiseService(void);
// Component log session variables.
le_log_SessionRef_t printServer_LogSession;
le_log_Level_t* printServer_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _printServer_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _printServer_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _printServer_Init(void)
{
    LE_DEBUG("Initializing printServer component library.");

    // Advertise server-side IPC interfaces.
    printer_AdvertiseService();

    // Register the component with the Log Daemon.
    printServer_LogSession = log_RegComponent("printServer", &printServer_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_printServer_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_printServer_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
