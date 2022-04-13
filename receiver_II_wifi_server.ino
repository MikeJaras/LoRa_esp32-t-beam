/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <axp20x.h>   
#include "AsyncUDP.h"

AXP20X_Class axp;
TinyGPSPlus gps;      
AsyncUDP udp;

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 866E6

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;
int rssi;
String text;
const char* ssid = "****ssid****";
const char* password = "****pwd****";

void setup() { 
  //initialize Serial Monitor
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 34, 12);   //12 rx -- 34 tx for GPS
  
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  digitalWrite(OLED_RST, HIGH);
  
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
    Serial.println("AXP192 Begin PASS");
  } else {
    Serial.println("AXP192 Begin FAIL");
  }

  // variabels
  int refresh_disp = 0;
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA RECEIVER ");
  display.display();

  Serial.println("LoRa Receiver Test");
  
  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSyncWord(0xA3);           // ranges from 0-0xFF, default 0x34, see API docs
  
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.println("LoRa Initializing OK!");
  display.display();  


}



void initWiFi(){
  axp.setChgLEDMode(AXP20X_LED_LOW_LEVEL); // LED on
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
//  while (WiFi.status() != WL_CONNECTED) {
  while (WiFi.status() != 3) {
    Serial.print('.');
    smartDelay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  display.setCursor(70,50);
  display.print(WiFi.status());
  display.display();   
  axp.setChgLEDMode(AXP20X_LED_OFF); // LED off
}

void loop() {

  // if (WiFi.status() != WL_CONNECTED) {
  if (WiFi.status() != 3) {
    initWiFi();
  }

  

  //try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    //Serial.print("Received packet ");
    //read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      rssi = LoRa.packetRssi();
    }

    Serial.print(LoRaData);
    text = LoRaData;
//    text = text + "\n";
    text = text + " with RSSI " + String(rssi) + "\n";

    // send to wifi
    udp.broadcastTo(text.c_str(), 1234);

      
      
     
      
    
    
    //print RSSI of packet

    Serial.print(" with RSSI ");    
    Serial.println(rssi);

    // Display information
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("LORA RECEIVER");
    display.setCursor(0,10);
    display.print("Receivedf\n packet:");
    display.setCursor(0,20);
    display.print(LoRaData);
    display.setCursor(0,50);
    display.print("RSSI:");
    display.setCursor(30,50);
    display.print(rssi);
    display.setCursor(70,50);
    display.print(WiFi.status());
    display.display();   


  }
}

static void smartDelay(unsigned long ms){
  unsigned long start = millis();
  do
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}
