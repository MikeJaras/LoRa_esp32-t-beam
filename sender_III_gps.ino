/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <axp20x.h>   

AXP20X_Class axp;
TinyGPSPlus gps;      

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
#define OLED_RST -1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//packet counter

// const int blueLED = 14; // blue LED next to GPS -- blink when packet sent //not used on t-beam
#define BUTTON_PIN 38

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

float home_lat = xx.xxxxx;
float home_lng = yy.yyyy;



void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 34, 12);   //12 rx -- 34 tx for GPS

  // pinMode(blueLED, OUTPUT); // For LED feedback
  pinMode(BUTTON_PIN, INPUT);  // Middle button next to LoRa chip. The one on the right is RESET, careful...

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  smartDelay(20);
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

  // varialbels
  float temp;
  float distance;

  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER ");
  display.display();
  
  Serial.println("LoRa Sender Test");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.setSpreadingFactor(7); // ranges from 6-12, default 7 see API docs. 
  LoRa.setTxPower(14, PA_OUTPUT_PA_BOOST_PIN);
  LoRa.setSyncWord(0xA3);           // ranges from 0-0xFF, default 0x34, see API docs
  
  display.setCursor(0,10);
  display.print("LoRa Initializing OK!");  
  display.display();
  smartDelay(2000);


}

float get_distance(float refLat, float refLng){
  float dist = (TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), refLat, refLng));
  Serial.println(dist / 1000);
  return dist;
  }

void displayDistance(float distansInKm){
  display.setTextColor(WHITE);
  display.setTextSize(1);

  display.setCursor(0,30);
  display.setTextColor(WHITE, BLACK); // to clear the line
  display.print(distansInKm / 1000);
  display.println(" km    ") ;
  display.display();

  display.setCursor(0,40);
  display.setTextColor(WHITE, BLACK); // to clear the line
  display.print(axp.getTemp());       // testing the axp functionality
  display.print(" ");
  display.write(167);
  display.print("C");
  }

void print_info_json(){
  Serial.print("{\'Valid\': \'");
  Serial.print(gps.location.isValid());
  Serial.print("\', \'Lat\': \'");
  Serial.print(gps.location.lat(), 5);
  Serial.print("\', \'Long\': \'");
  Serial.print(gps.location.lng(), 4);
  Serial.print("\', \'Satellites\': \'");
  Serial.print(gps.satellites.value());
  Serial.print("\', \'Altitude\': \'");
  Serial.print(gps.altitude.feet());
  Serial.print("\', \'Time\': \'");
  Serial.printf("%.2d:%.2d:%.2d",gps.time.hour(),gps.time.minute(),gps.time.second());
  Serial.print("\', \'Button state\': \'");
  Serial.print(digitalRead(BUTTON_PIN));
  Serial.print("\', \'temp [C]\': \'");
  Serial.print(axp.getTemp());
  Serial.println("\'}");
  }


void loop() {
  print_info_json();
  float distance = get_distance(home_lat, home_lng);
  displayDistance(distance);
  
  // send LoRa packet
  if (gps.location.isValid()){  // first couple of times through the loop, gps not yet available
    //digitalWrite(blueLED, HIGH);  // Turn blue LED on
    axp.setChgLEDMode(AXP20X_LED_LOW_LEVEL); // LED on
    
    // wait until the radio is ready to send a packet
    while (LoRa.beginPacket() == 0) {
      Serial.print("waiting for radio ... ");
      delay(100);
    }
    
    LoRa.beginPacket();
    LoRa.printf("Time: %.2d:%.2d:%.2d\n",gps.time.hour(),gps.time.minute(),gps.time.second());
    LoRa.print("Lat: ");
    LoRa.print(gps.location.lat(),4);
    LoRa.print("\nLong: ");
    LoRa.print(gps.location.lng(),4);
    LoRa.endPacket();
    //digitalWrite(blueLED, LOW); // Turn blue LED off
    smartDelay(100);
    axp.setChgLEDMode(AXP20X_LED_OFF); // LED off

    
  }
  else {
    LoRa.beginPacket();
    //LoRa.print("Error: Invalid gps data");
    Serial.print("Error: Invalid gps data");
    LoRa.endPacket();
  }
  
  smartDelay(10000);                                      
  
  if (millis() > 5000 && gps.charsProcessed() < 10){
    Serial.println(F("No GPS data received: check wiring"));
  }
    
  }
  



static void smartDelay(unsigned long ms)                
{
  unsigned long start = millis();
  do
  {
    while (Serial1.available())
      gps.encode(Serial1.read());
  } while (millis() - start < ms);
}
