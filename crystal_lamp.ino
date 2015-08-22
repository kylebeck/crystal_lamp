//#include <Octahedron.h>
#include <FastLED.h>

#define LED_PIN 9
#define NUM_LEDS 5
#define BRIGHTNESS          255
#define FRAMES_PER_SECOND   26

CRGB leds[NUM_LEDS];

uint8_t colorIndex = 0;
uint8_t colorDir = 1;

// Noise function parameters
uint32_t x,time,hue_x;  // x, & time values
uint8_t octaves=3;  // how many octaves to use for the brightness function
uint8_t hue_octaves=2;  // how many octaves to use for the hue function
int scale=10;  // the 'distance' between points on the x axis
int hue_scale=75;  // the 'distance' between x points for the hue noise
int time_speed=1;  // how fast we move through time & hue noise
int x_speed=10; // adjust these values to move along the x axis between frames

void setup() {
  delay(3000);
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  LEDS.setBrightness(BRIGHTNESS);
  FastLED.setDither(0);

  // initialize the x, hue_x and v_time values
  random16_set_seed(8934);
  random16_add_entropy(analogRead(2));
  hue_x = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  x = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
  time = (uint32_t)((uint32_t)random16() << 16) + (uint32_t)random16();
}

void loop() {
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  edgeNoise(octaves, x, scale, hue_octaves, hue_x, hue_scale, time);
  
  // adjust the intra-frame time values
  x += x_speed;
  time += time_speed;

  colorIndex += colorDir;
  
  EVERY_N_SECONDS( 10 )
  {
    if (colorDir == 1) {
      colorDir = -1;
    }
    else {
      colorDir = 1;
    }
  }
}

void edgeNoise(uint8_t octaves, uint16_t x, int scale, uint8_t hue_octaves, 
               uint16_t hue_x, int hue_scale, uint16_t time) {
  uint8_t V[NUM_LEDS];
  uint8_t H[NUM_LEDS];
  
  memset(V,0,NUM_LEDS);
  memset(H,0,NUM_LEDS);
  
  fill_raw_noise16into8(V,NUM_LEDS,octaves,x,scale,time);
  fill_raw_noise8(H,NUM_LEDS,hue_octaves,hue_x,hue_scale,time);
  
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    Serial.print(V[i]);
//    leds[i] = CHSV(H[i]+colorIndex,255,qadd8(V[i],128));
    leds[i] = CHSV(H[i]+colorIndex,qadd8(V[i],128),255);
  }
}
