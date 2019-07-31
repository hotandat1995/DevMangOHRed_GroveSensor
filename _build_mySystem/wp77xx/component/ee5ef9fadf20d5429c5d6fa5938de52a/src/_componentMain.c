/*
 * AUTO-GENERATED _componentMain.c for the dcs component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _dcs_le_mdc_ServiceInstanceName;
const char** le_mdc_ServiceInstanceNamePtr = &_dcs_le_mdc_ServiceInstanceName;
void le_mdc_ConnectService(void);
extern const char* _dcs_le_mrc_ServiceInstanceName;
const char** le_mrc_ServiceInstanceNamePtr = &_dcs_le_mrc_ServiceInstanceName;
void le_mrc_ConnectService(void);
extern const char* _dcs_le_cfg_ServiceInstanceName;
const char** le_cfg_ServiceInstanceNamePtr = &_dcs_le_cfg_ServiceInstanceName;
void le_cfg_ConnectService(void);
extern const char* _dcs_le_appInfo_ServiceInstanceName;
const char** le_appInfo_ServiceInstanceNamePtr = &_dcs_le_appInfo_ServiceInstanceName;
void le_appInfo_ConnectService(void);
extern const char* _dcs_le_dcs_ServiceInstanceName;
const char** le_dcs_ServiceInstanceNamePtr = &_dcs_le_dcs_ServiceInstanceName;
void le_dcs_AdvertiseService(void);
// Component log session variables.
le_log_SessionRef_t dcs_LogSession;
le_log_Level_t* dcs_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _dcs_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _dcs_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _dcs_Init(void)
{
    LE_DEBUG("Initializing dcs component library.");

    // Advertise server-side IPC interfaces.
    le_dcs_AdvertiseService();

    // Connect client-side IPC interfaces.
    le_mdc_ConnectService();
    le_mrc_ConnectService();
    le_cfg_ConnectService();
    le_appInfo_ConnectService();

    // Register the component with the Log Daemon.
    dcs_LogSession = log_RegComponent("dcs", &dcs_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_dcs_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_dcs_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
