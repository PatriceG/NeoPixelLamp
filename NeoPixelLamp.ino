//
// This code is inspired from the one at https://www.instructables.com/RGB-Circuit-Sculpture-Lamp/
// The main differences are: 
// - slowed-down animations 
// - current lighting mode memorized in EEPROM and restored at power-on// - code cleaned-up & refatored a bit
//

#include <EEPROM.h>
#include <FastLED.h>

#define MODE_PIN    3
#define LED_PIN     4
#define NUM_LEDS    8
#define BRIGHTNESS 255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];     
int mode = 0;
int lastMode = -1;
int LEDChange = 1; //how much we increment a color in the palette for each led
int LEDDelay = 150; //how much we wait before each loop

DEFINE_GRADIENT_PALETTE (violet) {
  0  , 0 , 211  , 255, //cyan
  73 , 146, 250  , 255, //light cyan
  190, 117, 133, 100, // dark
  255, 0 , 211  , 255, //cyan
}; 

DEFINE_GRADIENT_PALETTE (orange) {
  0  , 255, 172, 0 , //orange
  73 , 255, 240, 44, //light orange
  190, 255, 103, 36, //dark orange
  255, 255, 172, 0, //orange
};

DEFINE_GRADIENT_PALETTE (red) {
  0  , 255, 0, 0 , //red
  110 , 96, 24, 24, //dark red/brown
  140, 96, 24, 24, 
  255, 255, 0, 0 , //red
};

DEFINE_GRADIENT_PALETTE (black) {
  0    , 0, 0, 0 ,
  64   , 0, 0, 0 ,
  128  , 0, 0, 0 ,
  255  , 0, 0, 0 ,
};

CRGBPalette16 currentPalette;
CRGBPalette16 VioletPal = violet;
CRGBPalette16 OrangePal = orange;
CRGBPalette16 RedPal = red;
CRGBPalette16 BlackPal = black;

void setup() {
  delay(750); // power-up safety delay
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  
  currentPalette = RainbowColors_p;

  mode = EEPROM.read(0); //read last selected mode from EEPROM
  if(mode == 255){
    mode = 0;
  }
 }



void setMode(){
  static uint8_t pressed = 0;
  uint16_t val = analogRead(MODE_PIN);  // read the input pin
  if ((val < 100) and (pressed == 0) and (val > 5)) {
    if (analogRead(MODE_PIN) < 100) { 
      mode = mode + 1;      
     }
    pressed = 1;    
    if (mode > 5) {mode = 0;}
  }
  else if ((val > 300) and (val < 1000)) {
    pressed = 0;    
  }
}

void loop() {
    setMode();
    changePalette();
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; // motion speed
    
    fillLEDsFromPaletteColors( startIndex);    
    FastLED.show();
    FastLED.delay(LEDDelay);
}

void changePalette()
{
    if( lastMode != mode) {
        lastMode = mode;
        EEPROM.write(0,mode); //remember value for next power-up
        switch(mode){
           case 0:
               currentPalette = RainbowColors_p; LEDChange = 1; LEDDelay = 130;
              break;
           case 1:
              currentPalette = VioletPal; LEDChange = 50; LEDDelay = 60;
              break;
           case 2:
              currentPalette = OrangePal; LEDChange = 50; LEDDelay = 60;
              break;
           case 3:
              currentPalette = RedPal;LEDChange = 3; LEDDelay = 60;
              break;
           case 4:
               currentPalette = RainbowColors_p; LEDChange = 32; LEDDelay = 90;
               break;
           case 5:
               currentPalette = BlackPal; LEDChange = 0; LEDDelay = 30;
               break;
        }
    }
}

void fillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = BRIGHTNESS;
    
    for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, LINEARBLEND);
        colorIndex += LEDChange;
    }
}
