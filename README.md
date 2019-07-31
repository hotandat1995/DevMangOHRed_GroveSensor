# LegatoPractice
Post all practice app 

- groveColorSensor:
	This app use to read all data from Grove color sensor:
		+ Color temperature
		+ RGB value.
		+ Lux
- multiChannelRelay:
	This app provided an api to control Grove 4 channel relay.
	Ex: 0b0000 -> turn off all channel.
	corresponding to each bit is the state of a channel
- GroveRelayToCloud:
	This app use multiChannelRelay api to control and send data to AirVantage cloud, then control
from this by Custom command.
- RelayDataHubToCloud_V1:
	This app sent all data of Grove 4 channel relay to DataHub and observer it. On DataHub, I create an Observation for Temperature. It observe your Input temperature, when It out of limit range, relay channel 1 will blind.You can use Custom command to change limit range (Dead Band) for Temperature Object.
