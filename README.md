# Arduino monitor water level with NodeMCU ESP8266 + Firebase + Blynk

This project to check water level in tank if water level < 20% pump will start and when water level > 80% pump will stop with HC-SR04 ultrasonic sensor, show data in 0.96 inch OLED Display and send distance to firebase realtime database for React dash board in next project, **with Blynk**.

## Arduino Library use
1. Arduino core for ESP8266 WiFi chip: https://github.com/esp8266/Arduino
2. Firebase Realtime Database Arduino Library for ESP8266 and RP2040 Pico: https://github.com/mobizt/Firebase-ESP8266
3. Adafruit_SSD1306: https://github.com/adafruit/Adafruit_SSD1306
4. Blynk Library: https://docs.blynk.io/en/blynk.edgent-firmware-api/installation/install-blynk-library-in-arduino-ide

## Need to change code before use
```
#define BLYNK_TEMPLATE_ID           "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define BLYNK_TEMPLATE_NAME         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define BLYNK_AUTH_TOKEN            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
```
```
#define FIREBASE_HOST "Firebase database URL"
#define FIREBASE_AUTH "Database Secrets"
```
```
#define WIFI_SSID "Wifi Username"
#define WIFI_PASSWORD "Wifi Password"
```
```
IPAddress ip(192, 168, 0, 100); //Wifi IPAddress
```
```
int maxDist = 1000; // Tank Height or full water level you need in millimeters
```

## Schematic
<img alt="Schematicwith" src="https://i.ibb.co/xqggfKV/Schematicwith-Line.png">

## Pin wiring for LED & Relay Pump
| LED & Relay  | NodeMCU ESP8266 | Remark |
| ------------ | --------------- | -------|
| LED Wifi Status  | D3  | Check wifi connection |
| LED Pump Working  | D4  | Check pump working |
| LED Pump Stop  | D5  | Check pump stop |
| Pump Start for Relay  | D6  | Replace this to relay |


## Pin Wiring for HC-SR04
| HC-SR04 Pin  | NodeMCU ESP8266 |
| ------------ | --------------- |
| VCC  | VU  |
| TRIG  | D7  |
| ECHO  | D8  |
| GND  | GND  |

## Pin wiring for 0.96 inch OLED Display
| 0.96 inch OLED Display  | NodeMCU ESP8266 |
| ----------------------- | --------------- |
| GND  | GND  |
| VDD  | 3V  |
| SCK  | GPIO 5 (D1) |
| SDA  | GPIO 4 (D2) |

## Result in firebase realtime database
<img alt="Firebased realtime database" src="https://i.ibb.co/mTXLQSX/Screenshot-2023-03-25-174941.png">

+ Can change pumpstartmanual state to **ture** for start pump when water level < 80%
+ Can change pumpstopmanual state to **ture** for stop pump
+ Can change resetboard state to **ture** for restart board

## Blynk App Setup
1. Login to https://blynk.cloud/dashboard/login
2. Add datastreams value as picture, in mobile app wiil show follow this.
<img alt="arduino database" src="https://i.ibb.co/23YHY38/Screenshot-11.png">
3. Login Blynk app in mobile, create dashboard interface and add datastreams

<img src="https://i.ibb.co/NKPmX5R/IMG-8792.png" width="334"/> <img src="https://i.ibb.co/ZYqYMY4/IMG-8794.png" width="334"/> <img src="https://i.ibb.co/5GBCwz7/IMG-8793.png" width="334"/>

## Project photo
<img alt="photo1" src="https://i.ibb.co/zSCpRzh/IMG-8791.jpg">
<img alt="photo1" src="https://i.ibb.co/6FBrkxZ/IMG-8784.jpg">

