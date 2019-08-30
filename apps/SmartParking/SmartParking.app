<?xml version="1.0" encoding="UTF-8"?>
<app:application 
    xmlns:app="http://www.sierrawireless.com/airvantage/application/1.0" 
    type="SmartParking.app"
    name="SmartParking" 
    revision="1.1">
    <application-manager use="LWM2M_SW"/>
    <capabilities>
        <data>
            <encoding type="LWM2M">
                <asset default-label="MangOH Red" id="SmartParking">     
                    <node path="SpacesStatus" default-label="SpacesStatus">
                        <variable path="Space1" type="int" default-label="State Space 1"/>
                        <variable path="Space2" type="int" default-label="State Space 2"/>
                        <variable path="Space3" type="int" default-label="State Space 3"/>
                        <variable path="Space4" type="int" default-label="State Space 4"/>
                        <variable path="Space5" type="int" default-label="State Space 5"/>
                        <variable path="Space6" type="int" default-label="State Space 6"/>
                        <variable path="Space7" type="int" default-label="State Space 7"/>
                        <variable path="Space8" type="int" default-label="State Space 8"/>
                    </node>
                    <node path="SettingSpacesStatus" default-label="Setting Spaces Status">
                        <setting path="Space1" type="int" default-label="Space1"/>
                        <setting path="Space2" type="int" default-label="Space2"/>
                        <setting path="Space3" type="int" default-label="Space3"/>
                        <setting path="Space4" type="int" default-label="Space4"/>
                        <setting path="Space5" type="int" default-label="Space5"/>
                        <setting path="Space6" type="int" default-label="Space6"/>
                        <setting path="Space7" type="int" default-label="Space7"/>
                        <setting path="Space8" type="int" default-label="Space8"/>
                    </node>
                </asset>
            </encoding>
        </data>
    </capabilities>
</app:application>