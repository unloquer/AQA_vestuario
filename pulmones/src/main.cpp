#include "Arduino.h"
#include <SoftwareSerial.h>
#include "PMS.h"
#include <FastLED.h>
#include <FastLEDPainter.h>
#include <Time.h>

#define P_TOWER_RX D2
#define P_TOWER_TX 6

// Plantower
SoftwareSerial plantower_serial(P_TOWER_RX, P_TOWER_TX);
PMS pms(plantower_serial);
PMS::DATA data;

int pm2_5 = 0;
int *dir_pm2_5;

// Leds
#define DATA_PIN D7
#define CLK_PIN D6
#define LED_TYPE DOTSTAR
#define COLOR_ORDER BGR
#define NUM_LEDS 26
#define BRIGHTNESS 96
CRGB leds[NUM_LEDS];

//create one canvas and one brush with global scope
FastLEDPainterCanvas pixelcanvas = FastLEDPainterCanvas(NUM_LEDS);  //create canvas, linked to the FastLED library (canvas must be created before the brush)
FastLEDPainterBrush pixelbrush = FastLEDPainterBrush(&pixelcanvas); //crete brush, linked to the canvas to paint to

CHSV brushcolor; //the brush and the canvas operate on HSV color space only

int color = 0;
int speed = 400;

int time_hours = 0;
int time_minutes = 0;
int time_seconds = 0;

void readPlantower();
void animateLeds();
void updateValues();

void setup() {
  //initilize FastLED library
  FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  plantower_serial.begin(9600);
  Serial.begin(115200);
  Serial.println(" ");
  Serial.println(F("FastLED Painter simple demo"));

  //check if ram allocation of brushes and canvases was successful (painting will not work if unsuccessful, program should still run though)
  //this check is optional but helps to check if something does not work, especially on low ram chips like the Arduino Uno
  if (pixelcanvas.isvalid() == false)
    Serial.println(F("canvas allocation problem (out of ram, reduce number of pixels)"));
  else
    Serial.println(F("canvas allocation ok"));

  if (pixelbrush.isvalid() == false)
    Serial.println(F("brush allocation problem"));
  else
    Serial.println(F("brush allocation ok"));

  //initialize the animation, this is where the magic happens:
  brushcolor.h = color; //zero is red in HSV. Library uses 0-255 instead of 0-360 for colors (see https://en.wikipedia.org/wiki/HSL_and_HSV)
  brushcolor.s = 255;   //full color saturation
  brushcolor.v = 130;   //about half the full brightness

  pixelbrush.setSpeed(speed);      //set the brush movement speed (4096 means to move one pixel per update)
  pixelbrush.setColor(brushcolor); //set the brush color
  pixelbrush.setFadeSpeed(130);    //fading speed of pixels (255 is maximum fading speed)
  pixelbrush.setFadeout(true);     //do brightness fadeout after painting
  pixelbrush.setBounce(true);      //bounce the brush when it reaches the end of the strip

  // set pointer
  dir_pm2_5 = &pm2_5;

  // setup time
  setTime(time_hours, time_minutes, time_seconds, 30, 11, 19);
  pms.wakeUp();
}

void loop() {
  readPlantower();
}

void readPlantower() {
  int sec = second();
  if (sec == 0 || sec == 15 || sec == 20 || sec == 25 || sec == 30 || sec == 40 || sec == 50 ||
      sec == 5 || sec == 10 || sec == 35 || sec == 35 || sec == 45 || sec == 55) {
    if (pms.read(data)) { 
      pm2_5 = data.PM_AE_UG_2_5; 
      updateValues(); 
    }
    Serial.print("Leyendo pm 2.5 "); 
    Serial.println(*dir_pm2_5);
  }
  else {
    Serial.print("durmiendo. Ultimo valor ");
    Serial.println(*dir_pm2_5); 
    animateLeds();
  }
  // Serial.print("sec: ");
  // Serial.println(sec);
}

void animateLeds(){
  // Serial.println("Encendiendo leds");
  FastLED.clear();        //always need to clear the pixels, the canvas' colors will be added to whatever is on the pixels before calling a canvas update
  pixelbrush.paint();     //paint the brush to the canvas (and update the brush, i.e. move it a little)
  pixelcanvas.transfer(); //transfer the canvas to the LEDs
  FastLED.show();
}

void updateValues() {
  int velocity = map(*dir_pm2_5, 0, 2000, 200, 1500);
  int colour = map(*dir_pm2_5, 0, 500, 255, 100);
  brushcolor.h = colour;
  pixelbrush.setSpeed(velocity); 
  pixelbrush.setColor(brushcolor);
  pixelbrush.setFadeout(true);
  pixelbrush.setBounce(true);
}