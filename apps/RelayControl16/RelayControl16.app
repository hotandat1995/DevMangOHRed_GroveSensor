<?xml version="1.0" encoding="UTF-8"?>
<app:application 
    xmlns:app="http://www.sierrawireless.com/airvantage/application/1.0" 
    type="RelayControl16.app"
    name="RelayControl16" 
    revision="1.1">
    <application-manager use="LWM2M_SW"/>
    <capabilities>
        <data>
            <encoding type="LWM2M">
                <asset default-label="MangOH Red" id="RelayControl16">     
                    <node path="channelSetting" default-label="channelSetting">
                        <setting path="StateChannel1" type="int" default-label="State Channel1"/>
                        <setting path="StateChannel2" type="int" default-label="State Channel2"/>
                        <setting path="BlinkChannel1" type="int" default-label="Blink Interval Channel1"/>
                    </node>
                </asset>
            </encoding>
        </data>
    </capabilities>
</app:application>