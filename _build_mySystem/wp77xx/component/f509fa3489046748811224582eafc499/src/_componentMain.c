/*
 * AUTO-GENERATED _componentMain.c for the dcsCellular component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _dcsCellular_le_mdc_ServiceInstanceName;
const char** le_mdc_ServiceInstanceNamePtr = &_dcsCellular_le_mdc_ServiceInstanceName;
void le_mdc_ConnectService(void);
extern const char* _dcsCellular_le_mrc_ServiceInstanceName;
const char** le_mrc_ServiceInstanceNamePtr = &_dcsCellular_le_mrc_ServiceInstanceName;
void le_mrc_ConnectService(void);
extern const char* _dcsCellular_le_cfg_ServiceInstanceName;
const char** le_cfg_ServiceInstanceNamePtr = &_dcsCellular_le_cfg_ServiceInstanceName;
void le_cfg_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t dcsCellular_LogSession;
le_log_Level_t* dcsCellular_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _dcsCellular_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _dcsCellular_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _dcsCellular_Init(void)
{
    LE_DEBUG("Initializing dcsCellular component library.");

    // Connect client-side IPC interfaces.
    le_mdc_ConnectService();
    le_mrc_ConnectService();
    le_cfg_ConnectService();

    // Register the component with the Log Daemon.
    dcsCellular_LogSession = log_RegComponent("dcsCellular", &dcsCellular_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_dcsCellular_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_dcsCellular_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
