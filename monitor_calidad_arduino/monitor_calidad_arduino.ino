
//#include <ArduinoJson.h>
//#include <MQUnifiedsensor.h>
//#include <dht.h>
#include <SoftwareSerial.h>

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <stdlib.h>
#include "Fonts/FreeSans9pt7b.h"    // when you want other fonts
#include "Fonts/FreeSans12pt7b.h" // when you want other fonts
#include "Fonts/FreeSerif12pt7b.h" // when you want other fonts
#include "FreeDefaultFonts.h" // when you want other fonts


#define CENTER 239
#define BLACK 0x0000
#define NAVY 0x000F
#define DARKGREEN 0x03E0
#define DARKCYAN 0x03EF
#define MAROON 0x7800
#define PURPLE 0x780F
#define OLIVE 0x7BE0
#define LIGHTGREY 0xC618
#define DARKGREY 0x7BEF
#define BLUE 0x001F
#define GREEN 0x07E0
#define CYAN 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define ORANGE 0xFD20
#define GREENYELLOW 0xAFE5
#define PINK 0xF81F

#define DHTPIN 13

#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A5
#define type "MQ-135"
#define ADC_Bit_Resolution 10
#define RatioMQ135CleanAir 3.6

MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
dht DHT;
MCUFRIEND_kbv tft(A3, A2, A1, A0, A4);
SoftwareSerial esp(11, 12);


float gas = 0;
int   tem = 0, hu = 0;

void(* resetFunc) (void) = 0;
void showmsg(int x, int y, int sz, const GFXfont *f, const char *msg, int c, int b)
{
  int16_t x1, y1;
  uint16_t wid, ht;
  tft.setFont(f);
  tft.setCursor(x, y);
  tft.setTextColor(c, b);
  tft.setTextSize(sz);
  tft.print(msg);
}
void showmenu() {
  tft.fillRect(0, 0, 480, 40, BLUE);

  showmsg(50, 25, 1, &FreeSans9pt7b, "* AIR CONTROL MONITOR *", WHITE, BLUE);

  tft.drawLine(160, 40, 160, 140, BLACK);
  tft.drawLine(161, 40, 161, 140, BLACK);
  tft.drawLine(0, 140, 340, 140, BLACK);

  showmsg(5, 70, 1, &FreeSans9pt7b, "Temp(C):", BLACK, WHITE);
  //showmsg(5, 120, 1, &FreeSans9pt7b, "Humedad:", BLACK, WHITE);
  showmsg(165, 70, 1, &FreeSans9pt7b, "Humedad:", BLACK, WHITE);
  //showmsg(165, 120, 1, &FreeSans9pt7b, "Temp(Obj):", BLACK, WHITE);
}
void Screen(float CO, float CO2, float GAS, int HU, int TEM) {

  tft.fillRect(87, 51, 50, 80, WHITE);
  tft.fillRect(265, 51, 50, 80, WHITE);

  char hu[7];
  char tem[7];
  char gas [10];
  char co2 [10];
  char co [10];

  if (HU > 100) {
    HU = 90;
  }
  if (HU < 0) {
    HU = 0;
  }
  if (TEM > 100) {
    TEM = 90;
  }
  if (TEM < 0) {
    TEM = 0;
  }

  if (CO > 10000) {
    CO = 10000;
  }

  if (CO2 > 10000) {
    CO2 = 10000;
  }

  if (GAS > 10000) {
    GAS = 10000;
  }

  if (CO2 > 1800) {
    tft.fillRect(107, 140, 107, 100, RED);
  }
  else if (CO2 > 1500) {
    tft.fillRect(107, 140, 107, 100, ORANGE);
  }
  else if (CO2 > 1000) {
    tft.fillRect(107, 140, 107, 100, YELLOW);
  }
  else {
    tft.fillRect(107, 140, 107, 100, GREEN);
  }

  if (CO > 250) {
    tft.fillRect(0, 140, 107, 100, RED);
  }
  else if (CO > 150) {
    tft.fillRect(0, 140, 107, 100, YELLOW);
  }
  else if (CO > 50) {
    tft.fillRect(0, 140, 107, 100, YELLOW);
  }
  else {
    tft.fillRect(0, 140, 107, 100, GREEN);
  }

  if (GAS > 2000) {
    tft.fillRect(214, 140, 107, 100, RED);
  }
  else if (GAS > 1700) {
    tft.fillRect(214, 140, 107, 100, ORANGE);
  }
  else if (GAS > 1000) {
    tft.fillRect(214, 140, 107, 100, YELLOW);
  }
  else {
    tft.fillRect(214, 140, 107, 100, GREEN);
  }
  dtostrf(HU, 6, 1, hu);
  dtostrf(CO2, 8, 2, co2);
  dtostrf(CO, 8, 2, co);
  dtostrf(GAS, 8, 2, gas);
  dtostrf(TEM, 6, 1, tem);


  showmsg(80, 70, 1, &FreeSans9pt7b, tem, BLACK, WHITE);
  showmsg(255, 70 , 1, &FreeSans9pt7b, hu, BLACK, WHITE);
  tft.print("%");



  showmsg(5, 200, 1, &FreeSans9pt7b, co, BLACK, WHITE);
  showmsg(113, 200, 1, &FreeSans9pt7b, co2, BLACK, WHITE);
  showmsg(226, 200, 1, &FreeSans9pt7b, gas, BLACK, WHITE);
  showmsg(0, 230, 1, &FreeSans9pt7b, "|       CO       |       CO2       |       PM2       |", BLACK, WHITE);

}

void setup() {

  esp.begin(9600);


  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  tft.invertDisplay(false);
  tft.fillScreen(WHITE);
  showmenu();

  MQ135.setRegressionMethod(1);
  MQ135.init();
  MQ135.setRL(10);

  float calcR0 = 0;
  for (int i = 1; i <= 10; i ++)
  {
    MQ135.update(); // Update data, the arduino will be read the voltage on the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
  }
  MQ135.setR0(calcR0 / 10);

}


void loop() {

  for (int i = 0 ; i < 30; i++) {
    MQ135.update();
    DynamicJsonDocument doc(524);
    String message = "";

    MQ135.setA(605.18); MQ135.setB(-3.937); // Configurate the ecuation values to get CO concentration
    float co = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
    MQ135.setA(77.255); MQ135.setB(-3.18); // Configurate the ecuation values to get Alcohol concentration
    float Alcohol = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
    MQ135.setA(110.47); MQ135.setB(-2.862); // Configurate the ecuation values to get CO2 concentration
    float co2 = MQ135.readSensor() + 400 ; // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
    MQ135.setA(44.947); MQ135.setB(-3.445); // Configurate the ecuation values to get Tolueno concentration
    float Toluene = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
    MQ135.setA(102.2 ); MQ135.setB(-2.473); // Configurate the ecuation values to get NH4 concentration
    float NH4 = MQ135.readSensor(); // Sensor will read PPM concentration using the model and a and b values setted before or in the setup
    MQ135.setA(34.668); MQ135.setB(-3.369); // Configurate the ecuation values to get Acetona concentration
    float Acetona = MQ135.readSensor(); // Sensor will read PPM concentration
    gas = co + Alcohol + co2 + Toluene + NH4 + Acetona;

    DHT.read11(DHTPIN);
    hu = DHT.humidity - 15;
    tem =  DHT.temperature - 2;

    
      doc["co"] = co;
      doc["co2"] = co2;
      doc["gas"] = gas;
      doc["hum"] =  hu;
      doc["tem"] = tem;

      esp.flush();
      serializeJson(doc, esp);
    
    Screen(co, co2, gas, hu, tem);

    delay(1000);
  }
  resetFunc();
}
