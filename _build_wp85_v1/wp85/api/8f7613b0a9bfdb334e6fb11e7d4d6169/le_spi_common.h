
/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */
#ifndef LE_SPI_COMMON_H_INCLUDE_GUARD
#define LE_SPI_COMMON_H_INCLUDE_GUARD


#include "legato.h"

#define IFGEN_LE_SPI_PROTOCOL_ID "740a490bba2182b36d04362adb42f669"
#define IFGEN_LE_SPI_MSG_SIZE 1044



//--------------------------------------------------------------------------------------------------
/**
 * Max byte storage size for write buffer
 */
//--------------------------------------------------------------------------------------------------
#define LE_SPI_MAX_WRITE_SIZE 1024

//--------------------------------------------------------------------------------------------------
/**
 * Max byte storage size for read buffer
 */
//--------------------------------------------------------------------------------------------------
#define LE_SPI_MAX_READ_SIZE 1024

//--------------------------------------------------------------------------------------------------
/**
 * Handle for passing to related functions to access the SPI device
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_spi_DeviceHandle* le_spi_DeviceHandleRef_t;



//--------------------------------------------------------------------------------------------------
/**
 * Get if this client bound locally.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED bool ifgen_le_spi_HasLocalBinding
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Init data that is common across all threads
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_spi_InitCommonData
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Perform common initialization and open a session
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_spi_OpenSession
(
    le_msg_SessionRef_t _ifgen_sessionRef,
    bool isBlocking
);

//--------------------------------------------------------------------------------------------------
/**
 * Opens an SPI device so that the attached device may be accessed.
 *
 * @return
 *      - LE_OK on success
 *      - LE_BAD_PARAMETER if the device name string is bad
 *      - LE_NOT_FOUND if the SPI device file could not be found
 *      - LE_NOT_PERMITTED if the SPI device file can't be opened for read/write
 *      - LE_DUPLICATE if the given device file is already opened by another spiService client
 *      - LE_FAULT for non-specific failures
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_spi_Open
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        const char* LE_NONNULL deviceName,
        ///< [IN] Handle for the SPI master to open
        le_spi_DeviceHandleRef_t* handlePtr
        ///< [OUT] Handle for passing to related functions in order to
        ///< access the SPI device
);

//--------------------------------------------------------------------------------------------------
/**
 * Closes the device associated with the given handle and frees the associated resources.
 *
 * @note
 *      Once a handle is closed, it's not permitted to use it for future SPI access without first
 *      calling Open.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_spi_Close
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_spi_DeviceHandleRef_t handle
        ///< [IN] Handle for the SPI master to close
);

//--------------------------------------------------------------------------------------------------
/**
 * Configures an SPI device's data sample settings. The required values should be
 * included in your target's datasheet. Most common @c Mode values are
 * @c 0 and @c 3.
 *
 * These are the SPI Mode options:
 * |   Mode  |   Clock Polarity   |  Clock Phase   |   Clock Edge   |
 * | :-----: | :----------------: | :------------: | :------------: |
 * |   0     |       0            |   0            |    1           |
 * |   1     |       0            |   1            |    0           |
 * |   2     |       1            |   0            |    1           |
 * |   3     |       1            |   1            |    0           |
 *
 * @note
 *      This function should be called before any of the Read/Write functions to ensure
 *      the SPI bus configuration is in a known state.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED void ifgen_le_spi_Configure
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_spi_DeviceHandleRef_t handle,
        ///< [IN] Handle for the SPI master to configure
        int32_t mode,
        ///< [IN] Choose mode options for the bus per above table
        uint8_t bits,
        ///< [IN] bits per word, typically 8 bits per word
        uint32_t speed,
        ///< [IN] max speed (Hz), this is slave dependant
        int32_t msb
        ///< [IN] set as 0 for MSB as first bit or 1 for LSB as first bit
);

//--------------------------------------------------------------------------------------------------
/**
 * Performs SPI WriteRead Half Duplex. You can send send Read command/ address of data to read.
 *
 * @return
 *      LE_OK on success or LE_FAULT on failure.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_spi_WriteReadHD
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_spi_DeviceHandleRef_t handle,
        ///< [IN] Handle for the SPI master to perform the write-read on
        const uint8_t* writeDataPtr,
        ///< [IN] TX command/address being sent to slave with size
        size_t writeDataSize,
        ///< [IN]
        uint8_t* readDataPtr,
        ///< [OUT] RX response from slave with number of bytes reserved
        ///< on master
        size_t* readDataSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * SPI Write for Half Duplex Communication
 *
 * @return
 *      LE_OK on success or LE_FAULT on failure.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_spi_WriteHD
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_spi_DeviceHandleRef_t handle,
        ///< [IN] Handle for the SPI master to perform the write on
        const uint8_t* writeDataPtr,
        ///< [IN] TX command/address being sent to slave with size
        size_t writeDataSize
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * SPI Read for Half Duplex Communication
 *
 * @return
 *      LE_OK on success or LE_FAULT on failure.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_spi_ReadHD
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_spi_DeviceHandleRef_t handle,
        ///< [IN] Handle for the SPI master to perform the read from
        uint8_t* readDataPtr,
        ///< [OUT] RX response from slave with number of bytes reserved
        ///<  on master
        size_t* readDataSizePtr
        ///< [INOUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Simultaneous SPI Write and  Read for full duplex communication
 *
 * @return
 *      LE_OK on success or LE_FAULT on failure.
 */
//--------------------------------------------------------------------------------------------------
LE_SHARED le_result_t ifgen_le_spi_WriteReadFD
(
    le_msg_SessionRef_t _ifgen_sessionRef,
        le_spi_DeviceHandleRef_t handle,
        ///< [IN] Handle for the SPI master to perform full duplex write-read on
        const uint8_t* writeDataPtr,
        ///< [IN] TX command/address being sent to slave with size
        size_t writeDataSize,
        ///< [IN]
        uint8_t* readDataPtr,
        ///< [OUT] RX response from slave with same buffer size as TX
        size_t* readDataSizePtr
        ///< [INOUT]
);

#endif // LE_SPI_COMMON_H_INCLUDE_GUARD