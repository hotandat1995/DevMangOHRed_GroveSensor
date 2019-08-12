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
                        <!-- <setting path="channel1Setting" type="boolean" default-label="channel1Setting"/>
                        <setting path="channel2Setting" type="boolean" default-label="channel2Setting"/>
                        <setting path="channel3Setting" type="boolean" default-label="channel3Setting"/>
                        <setting path="channel4Setting" type="boolean" default-label="channel4Setting"/> -->
                        <setting path="UpperChannel1" type="int" default-label="Upper bound Channel1"/>
                        <setting path="BlinkChannel1" type="int" default-label="Blink Interval Channel1"/>
                        <!-- <setting path="LowerChannel1" type="int" default-label="Lower bound Channel1"/> -->
                    </node>

                </asset>
            </encoding>
        </data>
    </capabilities>
</app:application>