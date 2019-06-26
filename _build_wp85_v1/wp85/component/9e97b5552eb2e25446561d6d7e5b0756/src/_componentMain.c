/*
 * AUTO-GENERATED _componentMain.c for the avcCompat component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../liblegato/eventLoop.h"
#include "../liblegato/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _avcCompat_le_avc_ServiceInstanceName;
const char** le_avc_ServiceInstanceNamePtr = &_avcCompat_le_avc_ServiceInstanceName;
void le_avc_ConnectService(void);
extern const char* _avcCompat_le_appCtrl_ServiceInstanceName;
const char** le_appCtrl_ServiceInstanceNamePtr = &_avcCompat_le_appCtrl_ServiceInstanceName;
void le_appCtrl_ConnectService(void);
extern const char* _avcCompat_le_cfg_ServiceInstanceName;
const char** le_cfg_ServiceInstanceNamePtr = &_avcCompat_le_cfg_ServiceInstanceName;
void le_cfg_ConnectService(void);
extern const char* _avcCompat_le_appInfo_ServiceInstanceName;
const char** le_appInfo_ServiceInstanceNamePtr = &_avcCompat_le_appInfo_ServiceInstanceName;
void le_appInfo_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t avcCompat_LogSession;
le_log_Level_t* avcCompat_LogLevelFilterPtr;

// Declare component's COMPONENT_INIT_ONCE function,
// and provide default empty implementation.
__attribute__((weak))
void _avcCompat_COMPONENT_INIT_ONCE(void)
{
}
// Component initialization function (COMPONENT_INIT).
void _avcCompat_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _avcCompat_Init(void)
{
    LE_DEBUG("Initializing avcCompat component library.");

    // Connect client-side IPC interfaces.
    // 'le_avc' is [manual-start].
    le_appCtrl_ConnectService();
    le_cfg_ConnectService();
    le_appInfo_ConnectService();

    // Register the component with the Log Daemon.
    avcCompat_LogSession = log_RegComponent("avcCompat", &avcCompat_LogLevelFilterPtr);

// Queue the default component's COMPONENT_INIT_ONCE to Event Loop.
    event_QueueComponentInit(_avcCompat_COMPONENT_INIT_ONCE);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_avcCompat_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
