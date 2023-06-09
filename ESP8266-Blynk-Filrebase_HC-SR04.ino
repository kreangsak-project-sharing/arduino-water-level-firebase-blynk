
/*************************************************************

  This sketch shows how to write values to Virtual Pins

  NOTE:
  BlynkTimer provides SimpleTimer functionality:
    http://playground.arduino.cc/Code/SimpleTimer

  App dashboard setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define BLYNK_TEMPLATE_NAME         "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define BLYNK_AUTH_TOKEN            "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//https://github.com/mobizt/Firebase-ESP8266
#include <FirebaseESP8266.h>

//Firebase and Wifi Setup
#define FIREBASE_HOST "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define FIREBASE_AUTH "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

// Your WiFi credentials.
// Set password to "" for open networks.
#define WIFI_SSID "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

//Olwd display
//https://randomnerdtutorials.com/esp8266-0-96-inch-oled-display-with-arduino-ide/
//https://randomnerdtutorials.com/esp32-esp8266-dht-temperature-and-humidity-oled-display/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Pin output
int wifiStatus = D3;
int ledPumpWorking = D4;
int ledPumpStop = D5;
int pumpStart = D6;
int outputTrig = D7;

//Pin Input
int inputEcho = D8;

//Variable
int valueSensorRead = 0;
int valueSensorReadinPer = 0;
int nLoop = 0;
String wifiStatusOLED = "Off";
String pumpStatusOLED = "Off";

//Firebase
FirebaseData sensorFbRead;
FirebaseData fbStartPump;
FirebaseData fbStopPump;
FirebaseData resetFbBoard;

FirebaseJson updatevalueSensorRead;
FirebaseJson updatestatusPump;
FirebaseJson updateresetBoard;
FirebaseJson updatepumpStartmanual;
FirebaseJson updatepumpStopmanual;

BlynkTimer timer;
WidgetLED ledPump(V3);
//WidgetLED ledWifi(V4);

//Blynk Start pump manual
BLYNK_WRITE(V6) {
  if (param.asInt()) {
    if (valueSensorReadinPer < 80) {
      funcPumpStart(); //function pumpStart
    }
  }
}

//Blynk Stop pump manual
BLYNK_WRITE(V7)
{
  if (param.asInt()) {
    funcPumpStop(); //function pumpStop
  }
}

BLYNK_WRITE(V8)
{
  if (param.asInt()) {
    void(* resetFunc) (void) = 0;
    resetFunc();
  }
}

void waterLevel() {
  //-------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  //Check Wifi status
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(wifiStatus, LOW);
    wifiStatusOLED = "Off";

  } else {
    digitalWrite(wifiStatus, HIGH); //for reconnect
    wifiStatusOLED = "On";
  }

  delay(100);

  //-------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  //Check Water Level
  //valueSensorRead = analogRead(sensorRead);

  digitalWrite(outputTrig, LOW);
  delayMicroseconds(2);
  digitalWrite(outputTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(outputTrig, LOW);

  valueSensorRead = ((pulseIn(inputEcho, HIGH)) / 58.2) * 10;

  //Convert mm to %
  valueSensorReadinPer = funcConvetMMtoPercent(valueSensorRead);

  delay(100);

  //firebase
  updatevalueSensorRead.set("wificonnect", nLoop);
  updatevalueSensorRead.set("distance", valueSensorRead);
  updatevalueSensorRead.set("distanceinpercen", valueSensorReadinPer);
  Firebase.updateNode(sensorFbRead, "/arduino-pump/sensorhcsr04", updatevalueSensorRead);

  //Blynk
  Blynk.virtualWrite(V2, nLoop);
  Blynk.virtualWrite(V0, valueSensorReadinPer);
  Blynk.virtualWrite(V5, valueSensorRead);

  if (nLoop > 100) {
    nLoop = 0;
  }

  delay(100);

  //Start pump manual from firebase
  if (Firebase.getBool(fbStartPump, "/arduino-pump/pumpstartmanual/state")) {

    if (fbStartPump.boolData()) {
      delay(200);
      updatepumpStartmanual.set("state", false);
      Firebase.updateNode(fbStartPump, "/arduino-pump/pumpstartmanual", updatepumpStartmanual);

      delay(100);

      if (valueSensorReadinPer < 80) {
        funcPumpStart(); //function pumpStart
      }

      delay(200);
    }
  }

  //Stop pump manual from firebase
  if (Firebase.getBool(fbStopPump, "/arduino-pump/pumpstopmanual/state")) {

    if (fbStopPump.boolData()) {
      delay(200);
      updatepumpStopmanual.set("state", false);
      Firebase.updateNode(fbStopPump, "/arduino-pump/pumpstopmanual", updatepumpStopmanual);

      delay(100);

      funcPumpStop(); //function pumpStop

      delay(200);
    }
  }

  //Water level need pump stop
  if (valueSensorReadinPer < 20) {
    funcPumpStart(); //function pumpStart
  }

  delay(200);

  //Water level need pump stop
  if (valueSensorReadinPer > 80) {
    funcPumpStop(); //function pumpStop
  }

  delay(50);

  //OLED Display
  //clear display
  display.clearDisplay();

  //display Pump Status
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Pump ST: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(pumpStatusOLED);

  display.setTextSize(1);
  display.setCursor(65, 0);
  display.print("Water LV: ");
  display.setTextSize(2);
  display.setCursor(65, 10);
  display.print(valueSensorReadinPer);
  display.print("%");

  delay(50);

  //display Wifi Status
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Wifi ST: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(wifiStatusOLED);

  delay(50);

  //display Wifi Status
  display.setTextSize(1);
  display.setCursor(65, 35);
  display.print("Distance: ");
  display.setTextSize(2);
  display.setCursor(65, 45);
  display.print(valueSensorRead / 10);
  display.print("cm");

  display.display();

  delay(100);

  //-------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  //Reset Board from firebase
  if (Firebase.getBool(resetFbBoard, "/arduino-pump/resetboard/state")) {

    if (resetFbBoard.boolData()) {
      delay(200);
      updateresetBoard.set("state", false);
      Firebase.updateNode(resetFbBoard, "/arduino-pump/resetboard", updateresetBoard);
      delay(100);

      //Reset
      void(* resetFunc) (void) = 0;
      resetFunc();
    }
  }

  nLoop++;

  //main loop delay
  delay(200);
}

void setup()
{
  Serial.begin(9600);
  delay(100);

  pinMode(wifiStatus, OUTPUT);
  pinMode(ledPumpWorking, OUTPUT);
  pinMode(ledPumpStop, OUTPUT);
  pinMode(pumpStart, OUTPUT);
  pinMode(outputTrig, OUTPUT);

  pinMode(inputEcho, INPUT);

  //Setup OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(1000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(35, 24);
  display.setTextSize(2);
  display.println("START");
  display.display();
  delay(5000);

  //Check Wifi Connected
  connectWifi();

  //Open LED
  digitalWrite(ledPumpStop, HIGH);
  digitalWrite(wifiStatus, HIGH);

  //Firebase connect
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  delay(10);

  updatestatusPump.set("state", false);
  Firebase.updateNode(sensorFbRead, "/arduino-pump/pumpstatus", updatestatusPump);

  delay(10);

  updateresetBoard.set("state", false);
  Firebase.updateNode(resetFbBoard, "/arduino-pump/resetboard", updateresetBoard);

  delay(10);

  updatepumpStartmanual.set("state", false);
  Firebase.updateNode(fbStartPump, "/arduino-pump/pumpstartmanual", updatepumpStartmanual);

  delay(10);

  updatepumpStopmanual.set("state", false);
  Firebase.updateNode(fbStopPump, "/arduino-pump/pumpstopmanual", updatepumpStopmanual);

  delay(10);

  updatevalueSensorRead.set("wificonnect", 0);
  updatevalueSensorRead.set("distance", 0);
  updatevalueSensorRead.set("distanceinpercen", 0);
  Firebase.updateNode(sensorFbRead, "/arduino-pump/sensorhcsr04", updatevalueSensorRead);

  //-------------------------------------------------------------------------
  //-------------------------------------------------------------------------
  // Setup Blynk
  Blynk.config(BLYNK_AUTH_TOKEN);
  while (Blynk.connect() == false) {
  }

  //1sec = 10000L
  timer.setInterval(1000L, waterLevel);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//Check Wifi Status
void connectWifi() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  display.print("Connecting to ");
  display.print(WIFI_SSID);
  display.display();

  delay(50);

  //Fix IP
  IPAddress ip(192, 168, 0, 100);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress dns(8, 8, 8, 8);
  WiFi.config(ip, gateway, subnet, dns);

  // Connect to the Wi-Fi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");

    display.print(".");
    display.display();

    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);

  display.println();
  display.print("Connected to ");
  display.println(WIFI_SSID);
  display.display();

  delay(50);

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  delay(50);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//Pump Start
void funcPumpStart() {
  digitalWrite(ledPumpWorking, HIGH);
  digitalWrite(pumpStart, HIGH);
  digitalWrite(ledPumpStop, LOW);

  Serial.println("Pump On Working");
  pumpStatusOLED = "On";

  //Blynk
  Blynk.virtualWrite(V1, "On"); //Pump status
  ledPump.on(); //Wifi Pump

  delay(100);

  //Firebase
  updatestatusPump.set("state", true);
  Firebase.updateNode(sensorFbRead, "/arduino-pump/pumpstatus", updatestatusPump);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//Pump Stop
void funcPumpStop() {
  digitalWrite(ledPumpWorking, LOW);
  digitalWrite(pumpStart, LOW);
  digitalWrite(ledPumpStop, HIGH);

  Serial.println("Pump Stop");
  pumpStatusOLED = "Off";

  //Blynk
  Blynk.virtualWrite(V1, "Off"); //Pump status
  ledPump.off(); //Wifi Pump

  delay(100);

  //Firebase
  updatestatusPump.set("state", false);
  Firebase.updateNode(sensorFbRead, "/arduino-pump/pumpstatus", updatestatusPump);
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//Convert mm to Percen
int funcConvetMMtoPercent(int mmValue) {
  // Convert distance in millimeters to a percentage value
  int maxDist = 1000; // Maximum distance in millimeters
  int minDist = 0; // Minimum distance in millimeters
  int dist = mmValue; // Distance in millimeters
  int percent = map(dist, minDist, maxDist, 100, 0); // Map distance to 100-0%

  // Store percentage value in a variable
  return percent;
}
