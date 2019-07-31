

/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_GPIOPIN61_INTERFACE_H_INCLUDE_GUARD
#define LE_GPIOPIN61_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

// Internal includes for this interface
#include "le_gpio_common.h"
//--------------------------------------------------------------------------------------------------
/**
 * Get the server service reference
 */
//--------------------------------------------------------------------------------------------------
le_msg_ServiceRef_t le_gpioPin61_GetServiceRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the client session reference for the current message
 */
//--------------------------------------------------------------------------------------------------
le_msg_SessionRef_t le_gpioPin61_GetClientSessionRef
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Initialize the server and advertise the service.
 */
//--------------------------------------------------------------------------------------------------
void le_gpioPin61_AdvertiseService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Pin polarities.
 */
//--------------------------------------------------------------------------------------------------
typedef le_gpio_Polarity_t le_gpioPin61_Polarity_t;
#define LE_GPIOPIN61_ACTIVE_HIGH   LE_GPIO_ACTIVE_HIGH        ///< GPIO active-high, output is 1
#define LE_GPIOPIN61_ACTIVE_LOW   LE_GPIO_ACTIVE_LOW        ///< GPIO active-low, output is 0


//--------------------------------------------------------------------------------------------------
/**
 * State change event handler (callback).
 */
//--------------------------------------------------------------------------------------------------
typedef le_gpio_ChangeCallbackFunc_t le_gpioPin61_ChangeCallbackFunc_t;


//--------------------------------------------------------------------------------------------------
/**
 * Edge transitions.
 */
//--------------------------------------------------------------------------------------------------
typedef le_gpio_Edge_t le_gpioPin61_Edge_t;
#define LE_GPIOPIN61_EDGE_NONE   LE_GPIO_EDGE_NONE        ///< No edge detection
#define LE_GPIOPIN61_EDGE_RISING   LE_GPIO_EDGE_RISING        ///< Notify when voltage goes from low to high.
#define LE_GPIOPIN61_EDGE_FALLING   LE_GPIO_EDGE_FALLING        ///< Notify when voltage goes from high to low.
#define LE_GPIOPIN61_EDGE_BOTH   LE_GPIO_EDGE_BOTH        ///< Notify when pin voltage changes state in either direction.


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_gpioPin61_ChangeEvent'
 */
//--------------------------------------------------------------------------------------------------
typedef le_gpio_ChangeEventHandlerRef_t le_gpioPin61_ChangeEventHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Pull up or down.
 */
//--------------------------------------------------------------------------------------------------
typedef le_gpio_PullUpDown_t le_gpioPin61_PullUpDown_t;
#define LE_GPIOPIN61_PULL_OFF   LE_GPIO_PULL_OFF        ///< Neither resistor is enabled
#define LE_GPIOPIN61_PULL_DOWN   LE_GPIO_PULL_DOWN        ///< Pulldown resistor is enabled
#define LE_GPIOPIN61_PULL_UP   LE_GPIO_PULL_UP        ///< Pullup resistor is enabled



//--------------------------------------------------------------------------------------------------
/**
 * Configure the pin as an input pin.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_SetInput
(
    le_gpioPin61_Polarity_t polarity
        ///< [IN] Active-high or active-low.
);



//--------------------------------------------------------------------------------------------------
/**
 * Configure the pin as a push-pull output pin.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_SetPushPullOutput
(
    le_gpioPin61_Polarity_t polarity,
        ///< [IN] Active-high or active-low.
    bool value
        ///< [IN] Initial value to drive (true = active, false = inactive)
);



//--------------------------------------------------------------------------------------------------
/**
 * Configure the pin as a tri-state output pin.
 *
 * @note The initial state will be high-impedance.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_SetTriStateOutput
(
    le_gpioPin61_Polarity_t polarity
        ///< [IN] Active-high or active-low.
);



//--------------------------------------------------------------------------------------------------
/**
 * Configure the pin as an open-drain output pin.  "High" is a high-impedance state, while "Low"
 * pulls the pin to ground.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_SetOpenDrainOutput
(
    le_gpioPin61_Polarity_t polarity,
        ///< [IN] Active-high or active-low.
    bool value
        ///< [IN] Initial value to drive (true = active, false = inactive)
);



//--------------------------------------------------------------------------------------------------
/**
 * Enable the pull-up resistor (disables pull-down if previously enabled).
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_EnablePullUp
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Enable the pull-down resistor (disables pull-up if previously enabled).
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_EnablePullDown
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Disable the pull-up and pull-down resistors.  Does nothing if both are already disabled.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_DisableResistors
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Set output pin to active state.
 *
 * @warning Only valid for output pins.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_Activate
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Set output pin to inactive state.
 *
 * @warning Only valid for output pins.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_Deactivate
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Set output pin to high impedance state.
 *
 * @warning Only valid for tri-state or open-drain output pins.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_SetHighZ
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Read value of GPIO input pin.
 *
 * @return true = active, false = inactive.
 *
 * @note It is invalid to read an output pin.
 */
//--------------------------------------------------------------------------------------------------
bool le_gpioPin61_Read
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_gpioPin61_ChangeEvent'
 *
 * Register a callback function to be called when an input pin changes state.
 *
 * If the pin is not capable of interrupt-driven operation, then it will be sampled every
 * sampleMs milliseconds.  Otherwise, sampleMs will be ignored.
 *
 * If this fails, either because the handler cannot be registered, or setting the
 * edge detection fails, then it will return a NULL reference.
 */
//--------------------------------------------------------------------------------------------------
le_gpioPin61_ChangeEventHandlerRef_t le_gpioPin61_AddChangeEventHandler
(
    le_gpioPin61_Edge_t trigger,
        ///< [IN] Change(s) that should trigger the callback to be called.
    le_gpioPin61_ChangeCallbackFunc_t handlerPtr,
        ///< [IN] The callback function.
    void* contextPtr,
        ///< [IN]
    int32_t sampleMs
        ///< [IN] If not interrupt capable, sample the input this often (ms).
);



//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_gpioPin61_ChangeEvent'
 */
//--------------------------------------------------------------------------------------------------
void le_gpioPin61_RemoveChangeEventHandler
(
    le_gpioPin61_ChangeEventHandlerRef_t handlerRef
        ///< [IN]
);



//--------------------------------------------------------------------------------------------------
/**
 * Set the edge detection mode. This function can only be used when a handler is registered
 * in order to prevent interrupts being generated and not handled
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_SetEdgeSense
(
    le_gpioPin61_Edge_t trigger
        ///< [IN] Change(s) that should trigger the callback to be called.
);



//--------------------------------------------------------------------------------------------------
/**
 * Turn off edge detection
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_gpioPin61_DisableEdgeSense
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Check if the pin is configured as an output.
 *
 * @return true = output, false = input.
 */
//--------------------------------------------------------------------------------------------------
bool le_gpioPin61_IsOutput
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Check if the pin is configured as an input.
 *
 * @return true = input, false = output.
 */
//--------------------------------------------------------------------------------------------------
bool le_gpioPin61_IsInput
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the current value of edge sensing.
 *
 * @return The current configured value
 *
 * @note it is invalid to read the edge sense of an output
 */
//--------------------------------------------------------------------------------------------------
le_gpioPin61_Edge_t le_gpioPin61_GetEdgeSense
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the current value the pin polarity.
 *
 * @return The current configured value
 */
//--------------------------------------------------------------------------------------------------
le_gpioPin61_Polarity_t le_gpioPin61_GetPolarity
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Check if the pin is currently active.
 *
 * @return true = active, false = inactive.
 *
 * @note this function can only be used on output pins
 */
//--------------------------------------------------------------------------------------------------
bool le_gpioPin61_IsActive
(
    void
);



//--------------------------------------------------------------------------------------------------
/**
 * Get the current value of pull up and down resistors.
 *
 * @return The current configured value
 */
//--------------------------------------------------------------------------------------------------
le_gpioPin61_PullUpDown_t le_gpioPin61_GetPullUpDown
(
    void
);


#endif // LE_GPIOPIN61_INTERFACE_H_INCLUDE_GUARD