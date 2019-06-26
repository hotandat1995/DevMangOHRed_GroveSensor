
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_PORT_COMMON_H_INCLUDE_GUARD
#define LE_PORT_COMMON_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_atServer_common.h"

#define IFGEN_LE_PORT_PROTOCOL_ID "c7135da061a6a38d810159fff5799177"
#define IFGEN_LE_PORT_MSG_SIZE 62



//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the device name.
 */
//--------------------------------------------------------------------------------------------------
#define LE_PORT_MAX_LEN_DEVICE_NAME 50

//--------------------------------------------------------------------------------------------------
/**
 * Reference type for referring to the device.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_port_Device* le_port_DeviceRef_t;



//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_port_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_port_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_port_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * This function requests to open a configured device. If the device was not opened, it opens
 * the device.
 *
 * @return
 *      - Reference to the device.
 *      - NULL if the device is not available.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_port_DeviceRef_t ifgen_le_port_Request
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL deviceName
        ///< [IN] Device name to be requested.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function switches the device into data mode.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 *      - LE_BAD_PARAMETER Invalid parameter.
 *      - LE_UNAVAILABLE   JSON parsing is not completed.
 *      - LE_DUPLICATE     Device already opened in data mode.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_port_SetDataMode
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_port_DeviceRef_t devRef,
        ///< [IN] Device reference.
        int* fdPtr
        ///< [OUT] File descriptor of the device.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function switches the device into AT command mode and returns AT server device reference.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 *      - LE_BAD_PARAMETER Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_port_SetCommandMode
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_port_DeviceRef_t devRef,
        ///< [IN] Device reference.
        le_atServer_DeviceRef_t* atServerDevRefPtr
        ///< [OUT] AT server device reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function closes the device and releases the resources.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 *      - LE_BAD_PARAMETER Invalid parameter.
 *      - LE_UNAVAILABLE   JSON parsing is not completed.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_port_Release
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_port_DeviceRef_t devRef
        ///< [IN] Device reference.
);

//--------------------------------------------------------------------------------------------------
/**
 * This function gets the device reference regarding to a given reference coming from the AT server.
 *
 * @return
 *      - LE_OK            Function succeeded.
 *      - LE_FAULT         Function failed.
 *      - LE_BAD_PARAMETER Invalid parameter.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_port_GetPortReference
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_atServer_DeviceRef_t atServerDevRef,
        ///< [IN] Device reference from AT server.
        le_port_DeviceRef_t* devRefPtr
        ///< [OUT] Device reference from port service.
);

#endif // LE_PORT_COMMON_H_INCLUDE_GUARD