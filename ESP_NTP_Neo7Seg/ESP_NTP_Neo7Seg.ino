/*

  Uhr auf Basis WS2812B
  Inspiriert durch https://www.instructables.com/id/RGB-7-segment-Display-Clock-With-WS2812B/
  Auch Teile der Software in diesem Projekt. Weitere Teile durch Adafruit NeoPixel
*/

extern "C" {
#include "user_interface.h"
}
#include <NTPClient.h>    // siehe https://github.com/arduino-libraries/NTPClient
#include <TimeLib.h>      // siehe https://github.com/PaulStoffregen/Time
#include <Timezone.h>     // siehe https://github.com/JChristensen/Timezone
#include <TimeLord.h>     //https://github.com/probonopd/TimeLord
#include <ESP8266WiFi.h>
#include <WiFiudp.h>
#include <Wire.h>
#include <U8g2lib.h>    //siehe https://github.com/olikraus/u8g2/wiki
#include <Adafruit_NeoPixel.h>
#include "translate.h"

//#define DEUTSCH

ADC_MODE(ADC_VCC);
U8G2_SSD1306_128X32_UNIVISION_1_SW_I2C u8g2(U8G2_R0, 2, 0, U8X8_PIN_NONE);
#define LED_PIN      3 /* RX0 */
#define NO_OF_LEDS (14 * 4 + 2)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NO_OF_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel background = Adafruit_NeoPixel(NO_OF_LEDS, 99, NEO_GRB + NEO_KHZ800);

#define LED_MIN 127
#define LED_MAX 256
uint32_t firstPixelHue = 0;
uint32_t EffektNow = 0;

// Sonne
#pragma message "ottO - Briesnitzr Hoehe 19a, Dresden"
// what is our longitude (west values negative) and latitude (south values negative)
TimeLord City;
float const LONGITUDE = 13.658055;
float const LATITUDE = 51.062369;
byte Heute[6];
byte PreDay = 0;
bool SonneDa = true;
bool PreSonneDa = false;


#define HOSTNAME "ntp2neo7"

//WiFi
const char* ssid     = "orange";
const char* password = "w!r messen 1000 Werte";
//const char* ssid     = "smuldom";
//const char* password = "dresdener str 33a";
IPAddress ipno;

int Volt = 0;

// local time zone definition
// Central European Time (Frankfurt, Paris) from Timezone/examples/WorldClock/WorldClock.pde
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone LTZ(CEST, CET);    // this is the Timezone object that will be used to calculate local time
TimeChangeRule *tcr;        //pointer to the time change rule, use to get the TZ abbrev


// A UDP instance to let us send and receive packets over UDP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "de.pool.ntp.org", 0, 60000);
//NTPClient timeClient(ntpUDP, "192.168.2.1", 0, 60000);
#define SYNC_TIME 1800
#define SWITCH_TIME 300  //jede SWITCH_TIME sec. die Anmation wechseln
#define BACKGROUND_NO 4 //Anzahl der Animationen

long int getNTPTime(void);
void printNumber(byte Number);

void printDigits(int digits);
void PrintFormattedTime(time_t TimeToPrint);

time_t lastUpdate = 0, DisplayTime = 0, lastPrint = 0, NextSync = 0, ThisTime = 0, LastStart = 0;
time_t last_t = 0;
int LastSecond, LastMinute;
uint32_t LastReset = 0, NextSwitch = 0;
bool ShowOLED = false, ShowIP = false;
bool TimeShown = false;
bool SayWaitOnce = false;
byte NTPMinute = 99;
char FloatString[30];
char OLEDString[26] ;
int rc, ShowSwitch = 0, BackgroundSwitch = 0;
long lastReconnectAttempt = 0;
long StartTime = 0;

uint32_t rgb[NO_OF_LEDS];
uint32_t Background[NO_OF_LEDS];
uint8_t rgb_r = 255, rgb_g = 255, rgb_b = 255;
bool dotBlink = true;


void PutOLED(int x, int y, char* Text);
void PrintOLED(time_t TimeToPrint);
void rainbow(void);

void setup()
{
  Wire.begin(0, 2);
  u8g2.begin();
  Serial.begin(115200);
  pixels.begin();
  pixels.show();
  pixels.setBrightness(64);
  background.begin();
  background.show();
  background.setBrightness(255);

  Serial.println();
  Serial.println("Hello WLAN-World!");
  PutOLED(20, 24, "ntp2neo7");
  //Serial.setDebugOutput(true);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HOSTNAME);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi verbunden");
  Serial.println("IP-Adresse: ");
  Serial.println();
  Serial.println(WiFi.localIP());
  Serial.print("Gateway: ");
  WiFi.gatewayIP().printTo(Serial);
  Serial.println();
  Serial.print("Subnet-Mask: ");
  WiFi.subnetMask().printTo(Serial);
  Serial.println();
  Serial.print("DNS-Server: ");
  WiFi.dnsIP().printTo(Serial);
  Serial.println();

  ipno = WiFi.localIP();
  sprintf(OLEDString, "%d.%d.%d.%d", ipno[0], ipno[1], ipno[2], ipno[3]);
  PutOLED(0, 24, OLEDString);
  delay(2000);

  // Sonnenauf- und -untergang bestimmen
  City.TimeZone(1 * 60); // tell TimeLord what timezone your RTC is synchronized to. You can ignore DST
  // as long as the RTC never changes back and forth between DST and non-DST
  City.DstRules(3, 4, 10, 4, 60); //Umschaltung WZ/SZ am 4. Sonntag im ärz, zurück am 4. Sonntag im Oktober. SZ 60 min plus
  City.Position(LATITUDE, LONGITUDE); // tell TimeLord where in the world we are

  bool LEDStatus = 0;
  timeClient.begin();
  while (!timeClient.update()) {
    Serial.print(".");
    delay(2500);
  }
  setTime(LTZ.toLocal(timeClient.getEpochTime()));
  NextSync = now() + SYNC_TIME;
  NTPMinute = minute(now());
  SayWaitOnce = true;

  Serial.print("Geht los mit:");
  DisplayTime = now();
  PrintFormattedTime(now());
  OLEDFormattedTime(now());
  //showTime(now());
  lastPrint = DisplayTime;

  LastReset = now();
  delay(200);
  //LEDTest();
  NextSwitch = now();
}

void loop() {
  if ((now() > NextSync) && (second(now()) == 30)) { // jede SYNC_TIME
    Serial.println("");
    Serial.println(F("*************************"));
    if (timeClient.update()) {
      setTime(LTZ.toLocal(timeClient.getEpochTime(), &tcr));
      Serial.print(F("*** NTP erfolgreich ****"));
      PutOLED(20, 0, "NTP erfolgreich");
      NextSync = now() + SYNC_TIME;
    }
    else {
      Serial.print(F("-- NTP nicht erfolgreich --"));
      PutOLED(20, 0, "NTP nicht erfolgreich");
      NextSync = now() + 45;
    }
    PrintFormattedTime(now());
    SunUpDown();
    Serial.println(F("*************************"));
  }

  if ((now() - NextSwitch) >= SWITCH_TIME) {  // Zeit, die Animation zu wechseln
    NextSwitch = now();
    BackgroundSwitch++;
    if (BackgroundSwitch >= BACKGROUND_NO)
      BackgroundSwitch = 0;
    BackgroundSwitch = 1; // Mal sehen, ob die Umschaltung schuld ist.
  }

  switch (second(now())) {
    case 15:
    case 16:
    case 17:
      ipno = WiFi.localIP();
      sprintf(OLEDString, "%d.%d.%d.%d", ipno[0], ipno[1], ipno[2], ipno[3]);
      PutOLED(0, 24, OLEDString);
      break;
    case 28:
    case 29:
    case 30:
      sprintf(OLEDString, "Hintergrund %d", BackgroundSwitch);
      PutOLED(0, 24, OLEDString);
      break;
    case 0:
    case 18:
    case 31:
      OLEDFormattedTime(now());
      break;
  }

  /*if ((hour(now()) == 3) && (minute(now()) == 3) && (second(now()) == 3)) {
    ESP.reset();
  }*/

  if (LastMinute != minute(now())) {
    PrintFormattedTime(now());
    DisplayTime = now();
    SunUpDown();
    LastMinute = minute(now());
  }

  if ((second(now()) != LastSecond)) {
    LastSecond = second(now());
    dotBlink = true;
    showTime(now());
  }

  if ((millis() - EffektNow) >= 50) {
    rainbow();
    EffektNow = millis();
  }

}
