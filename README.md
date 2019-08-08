LegatoPractice
================

Post all practice app
Im using VSCode and Legato, Leaf with this practice.

App groveColorSensor
--------------------

This app use to read all data from Grove color sensor

- Color temperature
- RGB value.
- Lux

App multiChannelRelay
---------------------

This app provided an api to control Grove 4 channel relay.
Ex: 0b0000 -> turn off all channel.
corresponding to each bit is the state of a channel

App GroveRelayToCloud
---------------------

This app use multiChannelRelay api to control and send data to AirVantage cloud, then control from this by Custom command.

App RelayDataHubToCloud_V1
--------------------------

This app sent all data of Grove 4 channel relay to DataHub and observer it. On DataHub, I create an Observation for Temperature. It observe your Input temperature, when It out of limit range, relay channel 1 will blind.You can use Custom command to change limit range (Dead Band) for Temperature Object.

App RelayDataHubToCloud_V2
--------------------------

This is an update of V1, it use "Setting" instead of "Custom command" and fix some error.

App BME680
----------

This app provide connect from MangOH board to BME680 sensor (via I2C, SPI not update). It collect data from sensor then sent to DataHub

App AK9753
----------

This app provide connect from MangOH board to AK9753 sensor (via I2C, C++). It collect data from sensor then sent to DataHub.

App BlueLED
----------

This app provide connect from MangOH board to LEDBlue circuit.

App LightSensor
----------

This app provide connect from MangOH board to Light Sensor, it provide ADC value and Votage value (LUX value not update, because I don't have map to compare ADC value to LUX value, if someone know that map, please sent me via email: hotandat_1995@yahoo.com.vn. Tks you so much).

App LEDStrip
----------

This app provide connect from MangOH board to LEDStrip driver circuit. It control single Led strip.

How to see DataHub
------------------

On the device, to view and control the Data Hub inputs and outputs:

    alias dhub='/legato/systems/current/appsWriteable/dataHub/bin/dhub'
    dhub list

For more information:

    dhub --help
