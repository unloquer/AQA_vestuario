#include "Arduino.h"
#include <SoftwareSerial.h>
#include "PMS.h"
#include <FastLED.h>
#include <Time.h>

#define ESCENAS 4

#define LED_PIN D6
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

#define P_TOWER_RX D2
#define P_TOWER_TX 6

const uint8_t kMatrixWidth = 8;
const uint8_t kMatrixHeight = 8;
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

// Plantower
SoftwareSerial plantower_serial(P_TOWER_RX, P_TOWER_TX);
PMS pms(plantower_serial);
CRGB leds[kMatrixWidth * kMatrixHeight];
PMS::DATA data;

#define amarillo CRGB::Yellow
#define black CRGB::Black
#define rojo CRGB::Red
#define blue CRGB::Blue

int pm2_5 = 0;
int *dir_pm2_5;
int time_hours = 0;
int time_minutes = 0;
int time_seconds = 0;
int BRIGHTNESS = 20; // this is half brightness
int loop_cnt = 0;

void readPlantower();
void animateBlue();
void animateYellow();
void animateRed();
void animateLeds();

CRGB blueM[ESCENAS][8][8] = {
  {
    {black, black, black, blue, blue, black, black, black},   // 1
    {black, black, blue, black, black, blue, black, black},   // 2
    {black, blue, black, blue, blue, black, blue, black},     // 3
    {blue, black, blue, black, black, blue, black, blue},     // 4
    {black, blue, black, black, black, black, blue, black},   // 5
    {blue, black, black, black, black, black, black, blue},   // 6
    {black, black, black, black, black, black, black, black}, // 7
    {blue, blue, blue, blue, blue, blue, blue, blue},         // 8
  },
  {
    {black, black, black, blue, blue, black, black, black},   // 1
    {black, black, blue, black, black, blue, black, black},   // 2
    {black, blue, black, blue, blue, black, blue, black},     // 3
    {blue, black, blue, black, black, blue, black, blue},     // 4
    {black, blue, black, black, black, black, blue, black},   // 5
    {blue, black, black, black, black, black, black, blue},   // 6
    {black, black, black, black, black, black, black, black}, // 7
    {black, black, black, black, black, black, black, black}, // 8
  },
  {
    {black, black, black, blue, blue, black, black, black},   // 1
    {black, black, blue, black, black, blue, black, black},   // 2
    {black, blue, black, black, black, black, blue, black},   // 3
    {blue, black, black, black, black, black, black, blue},   // 4
    {black, black, black, black, black, black, black, black}, // 5
    {black, black, black, black, black, black, black, black}, // 6
    {black, black, black, black, black, black, black, black}, // 7
    {black, black, black, black, black, black, black, black}, // 8
  },
  {
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
}};

CRGB yellowM[ESCENAS][8][8] = {
  {
    {black, black, black, amarillo, amarillo, black, black, black},   // 1
    {black, black, amarillo, black, black, amarillo, black, black},   // 2
    {black, amarillo, black, amarillo, amarillo, black, amarillo, black},     // 3
    {amarillo, black, amarillo, black, black, amarillo, black, amarillo},     // 4
    {black, amarillo, black, black, black, black, amarillo, black},   // 5
    {amarillo, black, black, black, black, black, black, amarillo},   // 6
    {black, black, black, black, black, black, black, black}, // 7
    {amarillo, amarillo, amarillo, amarillo, amarillo, amarillo, amarillo, amarillo},         // 8
  },
  {
    {black, black, black, amarillo, amarillo, black, black, black},   // 1
    {black, black, amarillo, black, black, amarillo, black, black},   // 2
    {black, amarillo, black, amarillo, amarillo, black, amarillo, black},     // 3
    {amarillo, black, amarillo, black, black, amarillo, black, amarillo},     // 4
    {black, amarillo, black, black, black, black, amarillo, black},   // 5
    {amarillo, black, black, black, black, black, black, amarillo},   // 6
    {black, black, black, black, black, black, black, black}, // 7
    {black, black, black, black, black, black, black, black}, // 8
  },
  {
    {black, black, black, amarillo, amarillo, black, black, black},   // 1
    {black, black, amarillo, black, black, amarillo, black, black},   // 2
    {black, amarillo, black, black, black, black, amarillo, black},   // 3
    {amarillo, black, black, black, black, black, black, amarillo},   // 4
    {black, black, black, black, black, black, black, black}, // 5
    {black, black, black, black, black, black, black, black}, // 6
    {black, black, black, black, black, black, black, black}, // 7
    {black, black, black, black, black, black, black, black}, // 8
  },
  {
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
}};

CRGB redM[ESCENAS][8][8] = {
  {
    {black, black, black, rojo, rojo, black, black, black},   // 1
    {black, black, rojo, black, black, rojo, black, black},   // 2
    {black, rojo, black, rojo, rojo, black, rojo, black},     // 3
    {rojo, black, rojo, black, black, rojo, black, rojo},     // 4
    {black, rojo, black, black, black, black, rojo, black},   // 5
    {rojo, black, black, black, black, black, black, rojo},   // 6
    {black, black, black, black, black, black, black, black}, // 7
    {rojo, rojo, rojo, rojo, rojo, rojo, rojo, rojo},         // 8
  },
  {
    {black, black, black, rojo, rojo, black, black, black},   // 1
    {black, black, rojo, black, black, rojo, black, black},   // 2
    {black, rojo, black, rojo, rojo, black, rojo, black},     // 3
    {rojo, black, rojo, black, black, rojo, black, rojo},     // 4
    {black, rojo, black, black, black, black, rojo, black},   // 5
    {rojo, black, black, black, black, black, black, rojo},   // 6
    {black, black, black, black, black, black, black, black}, // 7
    {black, black, black, black, black, black, black, black}, // 8
  },
  {
    {black, black, black, rojo, rojo, black, black, black},   // 1
    {black, black, rojo, black, black, rojo, black, black},   // 2
    {black, rojo, black, black, black, black, rojo, black},   // 3
    {rojo, black, black, black, black, black, black, rojo},   // 4
    {black, black, black, black, black, black, black, black}, // 5
    {black, black, black, black, black, black, black, black}, // 6
    {black, black, black, black, black, black, black, black}, // 7
    {black, black, black, black, black, black, black, black}, // 8
  },
  {
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
    {black, black, black, black, black, black, black, black},
}};

void setup() {
  LEDS.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  plantower_serial.begin(9600);
  Serial.begin(115200);
  FastLED.setBrightness(BRIGHTNESS);
  // set pointer
  dir_pm2_5 = &pm2_5;
  // setup time
  setTime(time_hours, time_minutes, time_seconds, 30, 11, 19);
  pms.wakeUp();
}

void loop(){
  readPlantower();
}

void readPlantower() {
  int sec = second();
  if (sec == 0 || sec == 15 || sec == 20 || sec == 25 || sec == 30 || sec == 40 || sec == 50 ||
      sec == 5 || sec == 10 || sec == 35 || sec == 35 || sec == 45 || sec == 55) {
    if (pms.read(data)) { 
      pm2_5 = data.PM_AE_UG_2_5; }
      Serial.print("Leyendo pm 2.5 "); 
      Serial.println(*dir_pm2_5);
    }
  else {
    // Serial.print("ultimo pm leido 2.5 "); 
    // Serial.println(*dir_pm2_5);
    Serial.println("animando pantalla"); 
    animateLeds(); 
  }
}

void animateLeds() {
  if (*dir_pm2_5 < 800) {
    animateBlue();
  } else if (*dir_pm2_5 < 1500){
    animateYellow();
  } else if (*dir_pm2_5 > 1800){
    animateRed();
  }
}

void animateBlue() {
  for (int i = 0; i < kMatrixHeight; i++) {
    for (int j = 0; j < kMatrixWidth; j++) {
      leds[i * kMatrixWidth + j] = blueM[loop_cnt % ESCENAS][i][j];
    }
  }
  FastLED.show();
  delay(500);
  for (int i = 0; i < kMatrixHeight; i++) {
    for (int j = 0; j < kMatrixWidth; j++) {
      leds[i * kMatrixWidth + j] = black;
    }
  }
  delay(500);
  loop_cnt++;
}

void animateRed() {
  for (int i = 0; i < kMatrixHeight; i++) {
    for (int j = 0; j < kMatrixWidth; j++) {
      leds[i * kMatrixWidth + j] = redM[loop_cnt % ESCENAS][i][j];
    }
  }
  FastLED.show();
  delay(500);
  for (int i = 0; i < kMatrixHeight; i++) {
    for (int j = 0; j < kMatrixWidth; j++) {
      leds[i * kMatrixWidth + j] = black;
    }
  }
  delay(20);
  loop_cnt++;
}

void animateYellow() {
  for (int i = 0; i < kMatrixHeight; i++) {
    for (int j = 0; j < kMatrixWidth; j++) {
      leds[i * kMatrixWidth + j] = yellowM[loop_cnt % ESCENAS][i][j];
    }
  }
  FastLED.show();
  delay(500);
  for (int i = 0; i < kMatrixHeight; i++) {
    for (int j = 0; j < kMatrixWidth; j++) {
      leds[i * kMatrixWidth + j] = black;
    }
  }
  delay(250);
  loop_cnt++;
}