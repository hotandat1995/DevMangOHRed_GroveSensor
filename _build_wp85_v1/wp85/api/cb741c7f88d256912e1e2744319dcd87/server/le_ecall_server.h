

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_ECALL_INTERFACE_H_INCLUDE_GUARD
#define LE_ECALL_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Interface specific includes
#include "le_mdmDefs_server.h"
#include "le_mcc_server.h"

// Internal includes for this interface
#include "le_ecall_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_ecall_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_ecall_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_ecall_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference returned by Create function and used by other functions
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 *  Configured operation modes.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 *  Configure which standard to follow for the eCall, either PAN European eCall or ERA GLONASS.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 *  eCall session states.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 *  eCall MSD transmission mode.
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 *  eCall MSD vehicle type
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Propulsion type Bit Mask
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Diagnostic result Bit Mask (64 bits)
 *
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Crash information Bit Mask (16 bits) indicating the type of road accident.
 *
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 *  eCall MSD coordinate system type
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Handler for eCall state changes.
 *
 */
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_ecall_StateChange'
 */
//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
/**
 * This function configures the eCall operation mode to eCall only, only emergency number can be
 * used to start an eCall session. The modem doesn't try to register on the Cellular network.
 * This function forces the modem to behave as eCall only mode whatever U/SIM operation mode.
 * The change doesn't persist over power cycles.
 * This function can be called before making an eCall.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_ForceOnlyMode
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Same as le_ecall_ForceOnlyMode(), but the change persists over power cycles.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_ForcePersistentOnlyMode
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * This function exits from eCall Only mode. It configures the eCall operation mode to Normal mode,
 * the modem uses the default operation mode at power up (or after U/SIM hotswap). The modem behaves
 * following the U/SIM eCall operation mode; for example the U/SIM can be configured only for eCall,
 * or a combination of eCall and commercial service provision.
 *
 * @return
 *      - LE_OK on success
 *      - LE_FAULT for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_ExitOnlyMode
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the configured Operation mode.
 *
 * @return
 *      - LE_OK     on success
 *      - LE_FAULT  for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetConfiguredOperationMode
(
    le_ecall_OpMode_t* opModePtr
        ///< [OUT] Operation mode
);



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_ecall_StateChange'
 *
 * This event provides information on eCall state changes.
 *
 */
//--------------------------------------------------------------------------------------------------
le_ecall_StateChangeHandlerRef_t le_ecall_AddStateChangeHandler
(
    le_ecall_StateChangeHandlerFunc_t handlerPtr,
        ///< [IN]
    void* contextPtr
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_ecall_StateChange'
 */
//--------------------------------------------------------------------------------------------------
void le_ecall_RemoveStateChangeHandler
(
    le_ecall_StateChangeHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Create a new eCall object
 *
 * The eCall is not actually established at this point. It's still up to the caller to call
 * le_ecall_Start() when ready.
 *
 * @return
 *      - A reference to the new Call object.
 *
 * @note On failure, the process exits; you don't have to worry about checking the returned
 *       reference for validity.
 */
//--------------------------------------------------------------------------------------------------
le_ecall_CallRef_t le_ecall_Create
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Call to free up a call reference.
 *
 * @note This will free the reference, but not necessarily stop an active eCall. If there are
 *       other holders of this reference the eCall will remain active.
 */
//--------------------------------------------------------------------------------------------------
void le_ecall_Delete
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the system standard.
 * Default is PAN EUROPEAN
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetSystemStandard
(
    le_ecall_SystemStandard_t systemStandard
        ///< [IN] System standard
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the system standard.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 *  - LE_BAD_PARAMETER parameter is NULL
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetSystemStandard
(
    le_ecall_SystemStandard_t* systemStandardPtr
        ///< [OUT] System Standard
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the MSDs version.
 * Default value is 1
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdVersion
(
    uint32_t msdVersion
        ///< [IN] Msd version
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the MSD version.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 *  - LE_BAD_PARAMETER parameter is NULL
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetMsdVersion
(
    uint32_t* msdVersionPtr
        ///< [OUT] Msd version
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the Vehicled Type
 * Default value is 0
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetVehicleType
(
    le_ecall_MsdVehicleType_t vehicleType
        ///< [IN] Vehicle type
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the Vehicled Type.
 *
 * @return
 *  - LE_OK on success
 *  - LE_BAD_PARAMETER parameter is NULL
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetVehicleType
(
    le_ecall_MsdVehicleType_t* vehicleTypePtr
        ///< [OUT] Vehicle type
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the VIN (Vehicle Identification Number).
 *
 * @return
 *  - LE_OK on success
 *  - LE_BAD_PARAMETER parameter is NULL.
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetVIN
(
    const char* LE_NONNULL vin
        ///< [IN] VIN (Vehicle Identification Number)
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the VIN (Vehicle Identification Number).
 *
 * @return
 *  - LE_OK on success
 *  - LE_NOT_FOUND if the value is not set.
 *  - LE_BAD_PARAMETER parameter is NULL or to small
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetVIN
(
    char* vin,
        ///< [OUT] VIN is gotten with a null termination.
    size_t vinSize
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the propulsion type.
 * Note that a vehicle may have more than one propulsion type.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetPropulsionType
(
    le_ecall_PropulsionTypeBitMask_t propulsionType
        ///< [IN] bitmask
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the propulsion stored.
 * Note that a vehicle may have more than one propulsion type.
 *
 * @return
 *  - LE_OK on success
 *  - LE_NOT_FOUND if the value is not set.
 *  - LE_FAULT for other failures
 *  - LE_BAD_PARAMETER parameter is NULL
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetPropulsionType
(
    le_ecall_PropulsionTypeBitMask_t* propulsionTypePtr
        ///< [OUT] bitmask
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the push/pull transmission mode.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 *  - LE_UNSUPPORTED Not supported on this platform
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdTxMode
(
    le_ecall_MsdTxMode_t mode
        ///< [IN] Transmission mode
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the push/pull transmission mode.
 *
 * @return
 *      - LE_OK     on success
 *      - LE_FAULT  for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetMsdTxMode
(
    le_ecall_MsdTxMode_t* modePtr
        ///< [OUT] Transmission mode
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the position transmitted by the MSD.
 *
 * @return
 *      - LE_OK on success
 *      - LE_DUPLICATE an MSD has been already imported
 *      - LE_BAD_PARAMETER bad input parameter
 *      - LE_FAULT on other failures
 *
 * @note The process exits, if an invalid eCall reference is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdPosition
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    bool isTrusted,
        ///< [IN] true if the position is accurate, false otherwise
    int32_t latitude,
        ///< [IN] latitude in degrees with 6 decimal places, positive North.
        ///< Maximum value is +90 degrees (+90000000), minimum value is -90
        ///< degrees (-90000000).
    int32_t longitude,
        ///< [IN] longitude in degrees with 6 decimal places, positive East.
        ///< Maximum value is +180 degrees (+180000000), minimum value is
        ///< -180 degrees (-180000000).
    int32_t direction
        ///< [IN] direction of the vehicle from magnetic north (0 to 358, clockwise)
        ///< in 2-degrees unit. Valid range is 0 to 179. If direction of
        ///< travel is invalid or unknown, the value 0xFF shall be used.
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the position Delta N-1 from position set in le_ecall_SetMsdPosition() transmitted by the MSD.
 *
 * @return
 *      - LE_OK on success
 *      - LE_DUPLICATE an MSD has been already imported
 *      - LE_BAD_PARAMETER bad input parameter
 *      - LE_FAULT on other failures
 *
 * @note The process exits, if an invalid eCall reference is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdPositionN1
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    int32_t latitudeDeltaN1,
        ///< [IN] longitude delta from position set in SetMsdPosition
        ///< 1 Unit = 100 miliarcseconds, which is approximately 3m
        ///< maximum value: 511 = 0 0'51.100'' (+- 1580m)
        ///< minimum value: -512 = -0 0'51.200'' (+- -1583m)
    int32_t longitudeDeltaN1
        ///< [IN] longitude delta from position set in SetMsdPosition
        ///< 1 Unit = 100 miliarcseconds, which is approximately 3m
        ///< maximum value: 511 = 0 0'51.100'' (+-1580m)
        ///< minimum value: -512 = -0 0'51.200'' (+- -1583m) )
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the position Delta N-2 from position set in le_ecall_SetMsdPositionN1() transmitted by the MSD.
 *
 * @return
 *      - LE_OK on success
 *      - LE_DUPLICATE an MSD has been already imported
 *      - LE_BAD_PARAMETER bad input parameter
 *      - LE_FAULT on other failures
 *
 * @note The process exits, if an invalid eCall reference is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdPositionN2
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    int32_t latitudeDeltaN2,
        ///< [IN] longitude delta from position set in SetMsdPositionN1
        ///< 1 Unit = 100 miliarcseconds, which is approximately 3m
        ///< maximum value: 511 = 0 0'51.100'' (+-1580m)
        ///< minimum value: -512 = -0 0'51.200'' (+- -1583m)
    int32_t longitudeDeltaN2
        ///< [IN] longitude delta from position set in SetMsdPositionN1
        ///< 1 Unit = 100 miliarcseconds, which is approximately 3m
        ///< maximum value: 511 = 0 0'51.100'' (+-1580m)
        ///< minimum value: -512 = -0 0'51.200'' (+- -1583m) )
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the number of passengers transmitted by the MSD.
 *
 * @return
 *      - LE_OK on success
 *      - LE_DUPLICATE an MSD has been already imported
 *      - LE_BAD_PARAMETER bad eCall reference
 *
 * @note The process exits, if an invalid eCall reference is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdPassengersCount
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    uint32_t paxCount
        ///< [IN] number of passengers
);



//--------------------------------------------------------------------------------------------------
/**
 * Import an already prepared MSD.
 *
 * MSD is transmitted only after an emergency call has been established.
 *
 * @return
 *      - LE_OK on success
 *      - LE_OVERFLOW The imported MSD length exceeds the MSD_MAX_LEN maximum length.
 *      - LE_BAD_PARAMETER bad eCall reference
 *      - LE_FAULT for other failures
 *
 * @note On failure, the process exits; you don't have to worry about checking the returned
 *       reference for validity.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_ImportMsd
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    const uint8_t* msdPtr,
        ///< [IN] the prepared MSD
    size_t msdSize
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Export the encoded MSD.
 *
 * @return
 *  - LE_OK on success
 *  - LE_OVERFLOW  The encoded MSD length exceeds the user's buffer length.
 *  - LE_NOT_FOUND  No encoded MSD is available.
 *  - LE_BAD_PARAMETER bad eCall reference.
 *  - LE_FAULT for other failures.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_ExportMsd
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    uint8_t* msdPtr,
        ///< [OUT] the encoded MSD
    size_t* msdSizePtr
        ///< [INOUT]
);



//--------------------------------------------------------------------------------------------------
/**
 * Send the MSD.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER bad eCall reference
 *      - LE_FAULT for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 *
 * @note On failure, the process exits, so you don't have to worry about checking the returned
 *       reference for validity.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SendMsd
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Start an automatic eCall session
 *
 * @return
 *      - LE_OK on success
 *      - LE_BUSY an eCall session is already in progress
 *      - LE_BAD_PARAMETER bad eCall reference
 *      - LE_FAULT for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 *
 * @note The process exits, if an invalid eCall reference is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_StartAutomatic
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Start a manual eCall session
 *
 * @return
 *      - LE_OK on success
 *      - LE_BUSY an eCall session is already in progress
 *      - LE_BAD_PARAMETER bad eCall reference
 *      - LE_FAULT for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 *
 * @note The process exits, if an invalid eCall reference is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_StartManual
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Start a test eCall  session
 *
 * @return
 *      - LE_OK on success
 *      - LE_BUSY an eCall session is already in progress
 *      - LE_BAD_PARAMETER bad eCall reference
 *      - LE_FAULT for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 *
 * @note The process exits, if an invalid eCall reference is given
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_StartTest
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * End the current eCall session
 *
 * @note
 * - During an automatic eCall session, it is not possible to end the ongoing session until
 *   LE_ECALL_STATE_STOPPED event is notified.
 * - For manual session, it is possible to end the ongoing session if the eCall is not connected
 *   (LE_ECALL_STATE_CONNECTED not yet received) and after LE_ECALL_STATE_STOPPED event is notified.
 * - Using this API doesn't affect the NAD deregistration mechanism. However, it disables the call
 *   auto answer function if already running.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER bad eCall reference
 *      - LE_FAULT for other failures
 *      - LE_UNSUPPORTED Not supported on this platform
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_End
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the current state for the given eCall
 *
 * @return
 *      - The current state for the given eCall
 *
 * @note The process exits, if an invalid eCall reference is given
 */
//--------------------------------------------------------------------------------------------------
le_ecall_State_t le_ecall_GetState
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the Public Safely Answering Point telephone number.
 *
 * @note That PSAP number is not applied to Manually or Automatically initiated eCall. For those
 *       modes, an emergency call is launched.
 *
 * @warning This function doesn't modify the U/SIM content.
 *
 * @return
 *  - LE_OK           On success
 *  - LE_FAULT        For other failures
 *  - LE_UNSUPPORTED  Not supported on this platform
 *
 * @note If PSAP number is empty or too long (max LE_MDMDEFS_PHONE_NUM_MAX_LEN digits), it is a
 *       fatal error, the function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetPsapNumber
(
    const char* LE_NONNULL psap
        ///< [IN] Public Safely Answering Point number
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the Public Safely Answering Point telephone number set with le_ecall_SetPsapNumber()
 * function.
 *
 * @note That PSAP number is not applied to Manually or Automatically initiated eCall. For those
 *       modes, an emergency call is launched.
 *
 * @warning This function doesn't read the U/SIM content.
 *
 * @return
 *  - LE_OK           On success
 *  - LE_FAULT        On failures or if le_ecall_SetPsapNumber() has never been called before
 *  * - LE_OVERFLOW     Retrieved PSAP number is too long for the out parameter
 *  - LE_UNSUPPORTED  Not supported on this platform
 *
 * @note If the passed PSAP pointer is NULL, a fatal error is raised and the function will not
 *       return.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetPsapNumber
(
    char* psap,
        ///< [OUT] Public Safely Answering Point telephone
        ///< number
    size_t psapSize
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * When modem is in ECALL_FORCED_PERSISTENT_ONLY_MODE or ECALL_ONLY_MODE, this function
 * can be called to request the modem to read the number to dial from the FDN/SDN of the U/SIM.
 *
 * @note If FDN directory is updated with new dial numbers, be sure that the SIM card is refreshed.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_UseUSimNumbers
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the NAD (network access device) deregistration time value. After termination of an emergency
 * call the in-vehicle system remains registered on the network for the period of time, defined by
 * the installation parameter NAD (network access device) deregistration time.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *
 * @note The formula to calculate NAD deregistration time for PAN_EUROPEAN is as below:
 *       ECallConfiguration.nad_deregistration_time = (deregTime+59)/60;
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetNadDeregistrationTime
(
    uint16_t deregTime
        ///< [IN] the NAD (network access device) deregistration time value (in minutes).
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the NAD (network access device) deregistration time value.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetNadDeregistrationTime
(
    uint16_t* deregTimePtr
        ///< [OUT] the NAD (network access device) deregistration time value
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the minimum interval value between dial attempts.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetIntervalBetweenDialAttempts
(
    uint16_t pause
        ///< [IN] the minimum interval value in seconds
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the minimum interval value between dial attempts.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT for other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetIntervalBetweenDialAttempts
(
    uint16_t* pausePtr
        ///< [OUT] the minimum interval value in seconds
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ECALL_MANUAL_DIAL_ATTEMPTS value. If a dial attempt under manual emergency call
 * initiation failed, it should be repeated maximally ECALL_MANUAL_DIAL_ATTEMPTS-1 times within
 * the maximal time limit of ECALL_DIAL_DURATION. The default value is 10.
 * Redial attempts stop once the call has been cleared down correctly, or if counter/timer reached
 * their limits.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetEraGlonassManualDialAttempts
(
    uint16_t attempts
        ///< [IN] the ECALL_MANUAL_DIAL_ATTEMPTS value
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ECALL_AUTO_DIAL_ATTEMPTS value. If a dial attempt under automatic emergency call
 * initiation failed, it should be repeated maximally ECALL_AUTO_DIAL_ATTEMPTS-1 times within
 * the maximal time limit of ECALL_DIAL_DURATION. The default value is 10.
 * Redial attempts stop once the call has been cleared down correctly, or if counter/timer reached
 * their limits.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetEraGlonassAutoDialAttempts
(
    uint16_t attempts
        ///< [IN] the ECALL_AUTO_DIAL_ATTEMPTS value
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ECALL_DIAL_DURATION time. It's the maximum time the IVS have to connect the emergency
 * call to the PSAP, including all redial attempts.
 * If the call is not connected within this time (or ManualDialAttempts/AutoDialAttempts dial
 * attempts), it will stop.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED if the function is not supported by the target
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetEraGlonassDialDuration
(
    uint16_t duration
        ///< [IN] the ECALL_DIAL_DURATION time value (in seconds)
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ECALL_CCFT time. It's the maximum delay before initiating an an automatic call
 * termination.
 * When the delay is reached and IVS NAD didn't receive a call clear-down indication then the call
 * is immediatly terminated.
 *
 * @note Allowed range of values is 1 to 720 minutes.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED if the function is not supported by the target
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetEraGlonassFallbackTime
(
    uint16_t duration
        ///< [IN] the ECALL_CCFT time value (in minutes)
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ECALL_AUTO_ANSWER_TIME time, the time interval wherein IVDS responds to incoming calls
 * automatically after emergency call completion.
 *
 * @note Default value of auto answer time is 20 minutes. Maximum value is 720 minutes.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED if the function is not supported by the target
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetEraGlonassAutoAnswerTime
(
    uint16_t autoAnswerTime
        ///< [IN] The ECALL_AUTO_ANSWER_TIME time value (in minutes)
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ECALL_MSD_MAX_TRANSMISSION_TIME time, the time period for MSD transmission.
 *
 * @note Default value of MSD transmission time is 20 seconds.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED if the function is not supported by the target
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetEraGlonassMSDMaxTransmissionTime
(
    uint16_t msdMaxTransTime
        ///< [IN] The ECALL_MSD_MAX_TRANSMISSION_TIME time value (in seconds)
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ERA-GLONASS ECALL_POST_TEST_REGISTRATION_TIME time.
 *
 * After completion of transmission of test diagnostics results in an eCall test session, the
 * in-vehicle system remains registered on the network for the period of time defined by the
 * ECALL_POST_TEST_REGISTRATION_TIME value.
 *
 * @note The ECALL_POST_TEST_REGISTRATION_TIME setting takes effect immediately and is persistent to
 * reset.
 *
 * @note An ECALL_POST_TEST_REGISTRATION_TIME value of zero means the IVS doesn't remain registered
 * after completion of transmission of test (diagnostics) results.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED if the function is not supported by the target
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetEraGlonassPostTestRegistrationTime
(
    uint16_t postTestRegTime
        ///< [IN] The ECALL_POST_TEST_REGISTRATION_TIME time value (in seconds)
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the ECALL_MANUAL_DIAL_ATTEMPTS value.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetEraGlonassManualDialAttempts
(
    uint16_t* attemptsPtr
        ///< [OUT] the ECALL_MANUAL_DIAL_ATTEMPTS value
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the ECALL_AUTO_DIAL_ATTEMPTS value.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetEraGlonassAutoDialAttempts
(
    uint16_t* attemptsPtr
        ///< [OUT] the ECALL_AUTO_DIAL_ATTEMPTS value
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the ECALL_DIAL_DURATION time.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetEraGlonassDialDuration
(
    uint16_t* durationPtr
        ///< [OUT] the ECALL_DIAL_DURATION time value (in seconds)
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the ECALL_CCFT time.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetEraGlonassFallbackTime
(
    uint16_t* durationPtr
        ///< [OUT] the ECALL_CCFT time value (in minutes)
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the ECALL_AUTO_ANSWER_TIME time.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on execution failure
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetEraGlonassAutoAnswerTime
(
    uint16_t* autoAnswerTimePtr
        ///< [OUT] The ECALL_AUTO_ANSWER_TIME time value (in minutes)
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the ECALL_MSD_MAX_TRANSMISSION_TIME time.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED if the function is not supported by the target
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetEraGlonassMSDMaxTransmissionTime
(
    uint16_t* msdMaxTransTimePtr
        ///< [OUT] The ECALL_MSD_MAX_TRANSMISSION_TIME time value (in seconds)
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the ERA-GLONASS ECALL_POST_TEST_REGISTRATION_TIME time.
 *
 * @return
 *  - LE_OK on success
 *  - LE_FAULT on failure
 *  - LE_UNSUPPORTED if the function is not supported by the target
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_GetEraGlonassPostTestRegistrationTime
(
    uint16_t* postTestRegTimePtr
        ///< [OUT] The ECALL_POST_TEST_REGISTRATION_TIME time value (in
        ///< seconds).
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ERA-GLONASS crash severity parameter.
 *
 * @return
 *  - LE_OK on success
 *  - LE_DUPLICATE an MSD has been already imported
 *  - LE_BAD_PARAMETER bad eCall reference
 *  - LE_FAULT on other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdEraGlonassCrashSeverity
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    uint32_t crashSeverity
        ///< [IN] the ERA-GLONASS crash severity parameter
);



//--------------------------------------------------------------------------------------------------
/**
 * Reset the ERA-GLONASS crash severity parameter. Therefore that optional parameter is not included
 * in the MSD message.
 *
 * @return
 *  - LE_OK on success
 *  - LE_DUPLICATE an MSD has been already imported
 *  - LE_BAD_PARAMETER bad eCall reference
 *  - LE_FAULT on other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_ResetMsdEraGlonassCrashSeverity
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ERA-GLONASS diagnostic result using a bit mask.
 *
 * @return
 *  - LE_OK on success
 *  - LE_DUPLICATE an MSD has been already imported
 *  - LE_BAD_PARAMETER bad eCall reference
 *  - LE_FAULT on other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdEraGlonassDiagnosticResult
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    le_ecall_DiagnosticResultBitMask_t diagnosticResultMask
        ///< [IN] ERA-GLONASS diagnostic result bit mask.
);



//--------------------------------------------------------------------------------------------------
/**
 * Reset the ERA-GLONASS diagnostic result bit mask. Optional parameter is not
 * included in the MSD message.
 *
 * @return
 *  - LE_OK on success
 *  - LE_DUPLICATE an MSD has been already imported
 *  - LE_BAD_PARAMETER bad eCall reference
 *  - LE_FAULT on other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_ResetMsdEraGlonassDiagnosticResult
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ERA-GLONASS crash type bit mask
 *
 * @return
 *  - LE_OK on success
 *  - LE_DUPLICATE an MSD has been already imported
 *  - LE_BAD_PARAMETER bad eCall reference
 *  - LE_FAULT on other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdEraGlonassCrashInfo
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    le_ecall_CrashInfoBitMask_t crashInfoMask
        ///< [IN] ERA-GLONASS crash type bit mask.
);



//--------------------------------------------------------------------------------------------------
/**
 * Reset the ERA-GLONASS crash type bit mask. Optional parameter is not included
 * in the MSD message.
 *
 * @return
 *  - LE_OK on success
 *  - LE_DUPLICATE an MSD has been already imported
 *  - LE_BAD_PARAMETER bad eCall reference
 *  - LE_FAULT on other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_ResetMsdEraGlonassCrashInfo
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the ERA-GLONASS coordinate system type.
 *
 * @return
 *  - LE_OK on success
 *  - LE_DUPLICATE an MSD has been already imported
 *  - LE_BAD_PARAMETER bad eCall reference
 *  - LE_FAULT on other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_SetMsdEraGlonassCoordinateSystemType
(
    le_ecall_CallRef_t ecallRef,
        ///< [IN] eCall reference
    le_ecall_MsdCoordinateType_t coordinateSystemType
        ///< [IN] the ERA-GLONASS coordinate system type
);



//--------------------------------------------------------------------------------------------------
/**
 * Reset the ERA-GLONASS coordinate system type. Therefore that optional parameter is not included
 * in the MSD message.
 *
 * @return
 *  - LE_OK on success
 *  - LE_DUPLICATE an MSD has been already imported
 *  - LE_BAD_PARAMETER bad eCall reference
 *  - LE_FAULT on other failures
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_ecall_ResetMsdEraGlonassCoordinateSystemType
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference
);



//--------------------------------------------------------------------------------------------------
/**
 * Called to get the termination reason.
 *
 * @return The termination reason.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
le_mcc_TerminationReason_t le_ecall_GetTerminationReason
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference.
);



//--------------------------------------------------------------------------------------------------
/**
 * Called to get the platform specific termination code.
 *
 * @return The platform specific termination code.
 *
 * @note If the caller is passing a bad pointer into this function, it is a fatal error, the
 *       function will not return.
 */
//--------------------------------------------------------------------------------------------------
int32_t le_ecall_GetPlatformSpecificTerminationCode
(
    le_ecall_CallRef_t ecallRef
        ///< [IN] eCall reference.
);


#endif // LE_ECALL_INTERFACE_H_INCLUDE_GUARD