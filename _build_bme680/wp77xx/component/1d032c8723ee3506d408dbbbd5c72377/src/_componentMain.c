/*
 * AUTO-GENERATED _componentMain.c for the BME680Component component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _BME680Component_io_ServiceInstanceName;
const char** io_ServiceInstanceNamePtr = &_BME680Component_io_ServiceInstanceName;
void io_ConnectService(void);
extern const char* _BME680Component_admin_ServiceInstanceName;
const char** admin_ServiceInstanceNamePtr = &_BME680Component_admin_ServiceInstanceName;
void admin_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t BME680Component_LogSession;
le_log_Level_t* BME680Component_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _BME680Component_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _BME680Component_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _BME680Component_Init(void)
{
    LE_DEBUG("Initializing BME680Component component library.");

    // Connect client-side IPC interfaces.
    io_ConnectService();
    admin_ConnectService();

    // Register the component with the Log Daemon.
    BME680Component_LogSession = log_RegComponent("BME680Component", &BME680Component_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_BME680Component_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_BME680Component_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
