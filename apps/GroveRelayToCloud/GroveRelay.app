<?xml version="1.0" encoding="UTF-8"?>
<app:application 
    xmlns:app="http://www.sierrawireless.com/airvantage/application/1.0" 
    type="RelayControl.app"
    name="GroveRelayToCloud" 
    revision="1.0">
    <application-manager use="LWM2M_SW"/>
    <capabilities>
        <data>
            <encoding type="LWM2M">
                <asset default-label="MangOH Red kakaka" id="MangOH">
                    <node path="GroveRelayToCloud" default-label="GroveRelayToCloud">
                        <variable default-label="Channel1Status" path="Channel1Status" type="int" />
                        <variable default-label="Channel2Status" path="Channel2Status" type="int" />
                        <variable default-label="Channel3Status" path="Channel3Status" type="int" />
                        <variable default-label="Channel4Status" path="Channel4Status" type="int" />
                    </node>
                    <node path="Commands" default-label="Commands">          
                        <command default-label="SwitchChannel 1" id="GroveRelayToCloud/SwitchChannel1"/>
                        <command default-label="SwitchChannel 2" id="GroveRelayToCloud/SwitchChannel2"/>
                        <command default-label="SwitchChannel 3" id="GroveRelayToCloud/SwitchChannel3"/>
                        <command default-label="SwitchChannel 4" id="GroveRelayToCloud/SwitchChannel4"/>
                    </node>
                </asset>
            </encoding>
        </data>
    </capabilities>
</app:application>