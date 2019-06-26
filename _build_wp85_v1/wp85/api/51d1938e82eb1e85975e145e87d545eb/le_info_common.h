
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_INFO_COMMON_H_INCLUDE_GUARD
#define LE_INFO_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_INFO_PROTOCOL_ID "975257ab76b4bd700b883f96a47b6508"
#define IFGEN_LE_INFO_MSG_SIZE 272



//--------------------------------------------------------------------------------------------------
/**
 * Maximum IMEI length (15 digits), excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_IMEI_MAX_LEN 15

//--------------------------------------------------------------------------------------------------
/**
 * Maximum IMEI length (15 digits)
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_IMEI_MAX_BYTES 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the IMEISV string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_IMEISV_MAX_LEN 255

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the IMEISV string
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_IMEISV_MAX_BYTES 256

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a version string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_VERS_LEN 256

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a version string
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_VERS_BYTES 257

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a model string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MODEL_LEN 256

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a model string
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MODEL_BYTES 257

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Electronic Serial Number (ESN) string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_ESN_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Electronic Serial Number (ESN) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_ESN_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a reset specific information string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_RESET_LEN 50

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of a reset specific information string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_RESET_BYTES 51

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Equipment Identifier (MEID) string, excluding any termination
 * character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MEID_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Equipment Identifier (MEID) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MEID_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Directory Number (MDN) string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MDN_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Directory Number (MDN) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MDN_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Station ISDN Number (MSISDN) string, excluding any termination
 * character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MIN_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Station ISDN Number (MSISDN) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MIN_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Station ISDN Number (NAI) string, excluding any termination
 * character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_NAI_LEN 72

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Mobile Station ISDN Number (NAI) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_NAI_BYTES 73

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Manufacturer Name string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MFR_NAME_LEN 128

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Manufacturer Name string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_MFR_NAME_BYTES 129

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Product Requirement Information Identifier (PRI ID) Part Number string
 * , excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PRIID_PN_LEN 15

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Product Requirement Information Identifier (PRI ID) Part Number string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PRIID_PN_BYTES 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Product Requirement Information Identifier (PRI ID) Revision Number string
 * , excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PRIID_REV_LEN 15

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Product Requirement Information Identifier (PRI ID) Revision Number string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PRIID_REV_BYTES 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Carrier Product Requirement Information (CAPRI) Name string.
 * , excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_CAPRI_NAME_LEN 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Carrier Product Requirement Information (CAPRI) Name string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_CAPRI_NAME_BYTES 17

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Carrier Product Requirement Information (CAPRI) Revision Number string.
 * , excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_CAPRI_REV_LEN 12

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Carrier Product Requirement Information (CAPRI) Revision Number string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_CAPRI_REV_BYTES 13

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of an Platform Serial Number (PSN) string, excluding any termination character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PSN_LEN 14

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the Platform Serial Number (PSN) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_PSN_BYTES 15

//--------------------------------------------------------------------------------------------------
/**
 * Maximum fields for the RF devices status request.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_RF_DEVICES_STATUS_MAX 16

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the product stock keeping unit number (SKU) string, excluding any termination
 * character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_SKU_LEN 32

//--------------------------------------------------------------------------------------------------
/**
 * Maximum length of the product stock keeping unit number (SKU) string.
 * One extra byte is added for the null character.
 */
//--------------------------------------------------------------------------------------------------
#define LE_INFO_MAX_SKU_BYTES 33

//--------------------------------------------------------------------------------------------------
/**
 * Reset type enum
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_INFO_RESET_UNKNOWN = 0,
        ///< Unknown
    LE_INFO_RESET_USER = 1,
        ///< User request
    LE_INFO_RESET_HARD = 2,
        ///< Hardware switch
    LE_INFO_RESET_UPDATE = 3,
        ///< Software update
    LE_INFO_RESET_CRASH = 4,
        ///< Software crash
    LE_INFO_POWER_DOWN = 5,
        ///< Power Down
    LE_INFO_VOLT_CRIT = 6,
        ///< Power Down due to a critical voltage level
    LE_INFO_TEMP_CRIT = 7
        ///< Power Down due to a critical temperature level
}
le_info_Reset_t;



//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_info_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_info_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the International Mobile Equipment Identity (IMEI).
 *
 * @return LE_FAULT       Function failed to retrieve the IMEI.
 * @return LE_OVERFLOW     IMEI length exceed the maximum length.
 * @return LE_OK          Function succeeded.
 *
 * @note If the caller passes a bad pointer into this function, it's a fatal error the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetImei
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* imei,
        ///< [OUT] IMEI string.
        size_t imeiSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Retrieve the International Mobile Equipment Identity software version number (IMEISV).
 *
 * @return LE_FAULT       Function failed to retrieve the IMEISV.
 * @return LE_OVERFLOW    IMEISV length exceed the maximum length.
 * @return LE_OK          Function succeeded.
 *
 * @note If the caller passes a bad pointer into this function, it's a fatal error; the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetImeiSv
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* imeiSv,
        ///< [OUT] IMEISV string.
        size_t imeiSvSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the firmware version string
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the version string is not available
 *      - LE_OVERFLOW if version string to big to fit in provided buffer
 *      - LE_FAULT for any other errors
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetFirmwareVersion
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* version,
        ///< [OUT] Firmware version string
        size_t versionSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the last reset information reason
 *
 * @return
 *      - LE_OK          on success
 *      - LE_UNSUPPORTED if it is not supported by the platform
 *        LE_OVERFLOW    specific reset information length exceeds the maximum length.
 *      - LE_FAULT       for any other errors
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetResetInformation
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_info_Reset_t* resetPtr,
        ///< [OUT] Reset information
        char* resetSpecificInfoStr,
        ///< [OUT] Reset specific information
        size_t resetSpecificInfoStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the bootloader version string
 *
 * @return
 *      - LE_OK on success
 *      - LE_NOT_FOUND if the version string is not available
 *      - LE_OVERFLOW if version string to big to fit in provided buffer
 *      - LE_FAULT for any other errors
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetBootloaderVersion
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* version,
        ///< [OUT] Bootloader version string
        size_t versionSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the device model identity (Target Hardware Platform).
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The device model identity length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetDeviceModel
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* modelPtr,
        ///< [OUT] The model identity string (null-terminated).
        size_t modelPtrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA device Mobile Equipment Identifier (MEID).
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The device Mobile Equipment identifier length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetMeid
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* meidStr,
        ///< [OUT] The Mobile Equipment identifier (MEID)
        ///<  string (null-terminated).
        size_t meidStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Electronic Serial Number (ESN) of the device.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Electronic Serial Number length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetEsn
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* esnStr,
        ///< [OUT] The Electronic Serial Number (ESN) of the device.
        ///<  string (null-terminated).
        size_t esnStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Mobile Directory Number (MDN) of the device.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Mobile Directory Number length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetMdn
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* mdnStr,
        ///< [OUT] The Mobile Directory Number (MDN)
        ///<  string (null-terminated).
        size_t mdnStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA version of Preferred Roaming List (PRL).
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_NOT_FOUND     The information is not available.
 *      - LE_FAULT         The function failed to get the value.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetPrlVersion
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint16_t* prlVersionPtr
        ///< [OUT] The Preferred Roaming List (PRL) version.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Preferred Roaming List (PRL) only preferences status.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_NOT_FOUND     The information is not available.
 *      - LE_FAULT         The function failed to get the value.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetPrlOnlyPreference
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        bool* prlOnlyPreferencePtr
        ///< [OUT] The CDMA Preferred Roaming List only preferences status.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Mobile Identification Number (MIN).
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The CDMA Mobile Identification Number length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetMin
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* msisdnStr,
        ///< [OUT] The Mobile Identification Number (MIN)
        ///<  string (null-terminated).
        size_t msisdnStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the CDMA Network Access Identifier (NAI) string in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Network Access Identifier (NAI) length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetNai
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* naiStr,
        ///< [OUT] The Network Access Identifier (NAI)
        ///<  string (null-terminated).
        size_t naiStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Manufacturer Name string in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Manufacturer Name length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetManufacturerName
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* mfrNameStr,
        ///< [OUT] The Manufacturer Name string (null-terminated).
        size_t mfrNameStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Product Requirement Information Part Number and Revision Number strings in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Part or the Revision Number strings length exceed the maximum length.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetPriId
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* priIdPnStr,
        ///< [OUT] The Product Requirement Information Identifier
        ///< (PRI ID) Part Number string (null-terminated).
        size_t priIdPnStrSize,
        ///< [IN]
        char* priIdRevStr,
        ///< [OUT] The Product Requirement Information Identifier
        ///< (PRI ID) Revision Number string
        ///< (null-terminated).
        size_t priIdRevStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Carrier PRI Name and Revision Number strings in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The Part or the Revision Number strings length exceed the maximum length.
 *      - LE_UNSUPPORTED   The function is not supported on the platform.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetCarrierPri
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* capriNameStr,
        ///< [OUT] The Carrier Product Requirement Information
        ///< (CAPRI) Name string (null-terminated).
        size_t capriNameStrSize,
        ///< [IN]
        char* capriRevStr,
        ///< [OUT] The Carrier Product Requirement Information
        ///< (CAPRI) Revision Number string
        ///< (null-terminated).
        size_t capriRevStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the Platform Serial Number (PSN) string.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW if Platform Serial Number to big to fit in provided buffer
 *      - LE_FAULT for any other errors
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetPlatformSerialNumber
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* platformSerialNumberStr,
        ///< [OUT] Platform Serial Number string.
        size_t platformSerialNumberStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the RF devices working status (i.e. working or broken) of modem's RF devices such as
 * power amplifier, antenna switch and transceiver. That status is updated every time the module
 * power on.
 *
 * @return
 *      - LE_OK on success
 *      - LE_UNSUPPORTED request not supported
 *      - LE_FAULT function failed to get the RF devices working status
 *      - LE_OVERFLOW the number of statuses exceeds the maximum size
 *        (LE_INFO_RF_DEVICES_STATUS_MAX)
 *      - LE_BAD_PARAMETER Null pointers provided
 *
 * @note If the caller is passing null pointers to this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetRfDeviceStatus
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint16_t* manufacturedIdPtr,
        ///< [OUT] Manufactured identifier (MID)
        size_t* manufacturedIdSizePtr,
        ///< [INOUT]
        uint8_t* productIdPtr,
        ///< [OUT] Product identifier (PID)
        size_t* productIdSizePtr,
        ///< [INOUT]
        bool* statusPtr,
        ///< [OUT] Status of the RF device (MID,PID):
        ///< 0 means something wrong
        ///< 1 means no error found
        size_t* statusSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the product stock keeping unit number (SKU) string in ASCII text.
 *
 * @return
 *      - LE_OK            The function succeeded.
 *      - LE_FAULT         The function failed to get the value.
 *      - LE_OVERFLOW      The SKU number string length exceeds the maximum length.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetSku
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        char* skuIdStr,
        ///< [OUT] Product SKU ID string.
        size_t skuIdStrSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of expected resets
 *
 * @return
 *      - LE_OK             Success
 *      - LE_BAD_PARAMETER  Input prameter is a null pointer
 *      - LE_UNSUPPORTED    If not supported by the platform
 *      - LE_FAULT          Failed to get the number if expected resets
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetExpectedResetsCount
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint64_t* resetsCountPtrPtr
        ///< [OUT] Number of expected resets
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the number of unexpected resets
 *
 * @return
 *      - LE_OK             Success
 *      - LE_BAD_PARAMETER  Input prameter is a null pointer
 *      - LE_UNSUPPORTED    If not supported by the platform
 *      - LE_FAULT          Failed to get the number if expected resets
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_info_GetUnexpectedResetsCount
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        uint64_t* resetsCountPtrPtr
        ///< [OUT] Number of expected resets
);

#endif // LE_INFO_COMMON_H_INCLUDE_GUARD