Smart Parking app
================

How to get access_token
----------

API: POST

url: https://eu.airvantage.net/api/oauth/token
grant_type: password
username: nxthongbk@gmail.com
client_id: 22814490e23740d3ac8ebf44854ca11d
client_secret: 9c86b87dabbf45eeaa88b4ee936f97a1
password: 1_Abc_123

How to change setting
----------

API: POST

url: https://eu.airvantage.net/api/v1/operations/systems/settings
access_token: get from access_token

Example: Change Space 1 status = 1 
body: JSON(applicatation/json)

{
    "protocol" : "LWM2M",
        "reboot": false,
    "systems" : {
        "label" : "Main MangOH Red"
    },
    "settings" : [
        {
            "key" : "SmartParking.SettingSpacesStatus.Space1",
            "value" : 1
        }
    ]
}

Example: Change Space 1 status = 1, Space 2 status = 2
body: JSON(applicatation/json)
{
    "protocol" : "LWM2M",
        "reboot": false,
    "systems" : {
        "label" : "Main MangOH Red"
    },
    "settings" : [
        {
            "key" : "SmartParking.SettingSpacesStatus.Space1",
            "value" : 1
        },
            {
            "key" : "SmartParking.SettingSpacesStatus.Space2",
            "value" : 2
        }
    ]
}

List key:                                           Space status:
SmartParking.SettingSpacesStatus.Space1             Space 1
SmartParking.SettingSpacesStatus.Space2             Space 2
SmartParking.SettingSpacesStatus.Space3             Space 3
SmartParking.SettingSpacesStatus.Space4             Space 4
SmartParking.SettingSpacesStatus.Space5             Space 5
SmartParking.SettingSpacesStatus.Space6             Space 6
SmartParking.SettingSpacesStatus.Space7             Space 7
SmartParking.SettingSpacesStatus.Space8             Space 8

How to get lastest data
----------

API: GET

url: https://eu.airvantage.net/api/v1/systems/1e0ee3e9031740189c3f83c39965be6b/data

access_token: Gen from access_token
timestamp: gen from system (example: 1566988616078)
all: TRUE
company: e1a61f787cca4d8b98dc82e07079ad6d
