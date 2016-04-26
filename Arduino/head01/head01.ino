#include <Adafruit_NeoPixel.h>

#define PIN            6
#define NUMPIXELS      7

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int trigger = 1;
int flag1 = 0;
int flag2 = 0;
int counter = 0;
int counter2 = 0;
int counter3 = 0;

unsigned long previousPixelMillis = 0;
unsigned long previousTriggerMillis = 0;
long pixelTime = 800;
long triggerTime = 150;


void setup() {
pixels.begin(); // This initializes the NeoPixel library.

pinMode(3, INPUT_PULLUP);
  
}


void loop() {
  unsigned long currentMillis = millis();
      
  
  if (currentMillis - previousPixelMillis >= pixelTime && flag1 == 0) {
    pixels.setPixelColor(0, pixels.Color(50,50,0));
    pixels.setPixelColor(1, pixels.Color(100,0,0));
    pixels.setPixelColor(2, pixels.Color(0,50,50));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousPixelMillis = currentMillis; 

    flag1 = 1;
  }

    if (currentMillis - previousPixelMillis >= pixelTime && flag1 == 1) {
    pixels.setPixelColor(0, pixels.Color(100,0,0));
    pixels.setPixelColor(1, pixels.Color(100,0,0));
    pixels.setPixelColor(2, pixels.Color(0,0,100));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousPixelMillis = currentMillis; 

    flag1 = 2;
  }

    if (currentMillis - previousPixelMillis >= pixelTime && flag1 == 2) {
    pixels.setPixelColor(0, pixels.Color(100,0,0));
    pixels.setPixelColor(1, pixels.Color(50,0,50));
    pixels.setPixelColor(2, pixels.Color(0,0,100));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousPixelMillis = currentMillis; 

    flag1 = 3;
  }

    if (currentMillis - previousPixelMillis >= pixelTime && flag1 == 3) {
    pixels.setPixelColor(0, pixels.Color(100,0,0));
    pixels.setPixelColor(1, pixels.Color(0,0,0));
    pixels.setPixelColor(2, pixels.Color(0,0,100));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousPixelMillis = currentMillis; 

    flag1 = 4;
  }

    if (currentMillis - previousPixelMillis >= pixelTime && flag1 == 4) {
    pixels.setPixelColor(0, pixels.Color(100,0,0));
    pixels.setPixelColor(1, pixels.Color(50,0,50));
    pixels.setPixelColor(2, pixels.Color(0,0,0));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousPixelMillis = currentMillis; 

    flag1 = 0;
  }

  trigger = digitalRead(3);

  if (trigger == 0) {
    flag2 = 1;
  }

   if (currentMillis - previousTriggerMillis >= triggerTime && flag2 == 1) {
    pixels.setPixelColor(5, pixels.Color(50,50,50));
    pixels.setPixelColor(6, pixels.Color(50,50,50));
    pixels.setPixelColor(3, pixels.Color(50,50,50));
    pixels.setPixelColor(4, pixels.Color(50,50,50));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousTriggerMillis = currentMillis; 

    flag2 = 2;
  }

    if (currentMillis - previousTriggerMillis >= (triggerTime*1.5) && flag2 == 2) {
    pixels.setPixelColor(5, pixels.Color(0,0,0));
    pixels.setPixelColor(6, pixels.Color(50,50,50));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousTriggerMillis = currentMillis; 

    flag2 = 1;
    counter = counter +1;
  }

    if (counter == 10) {
      counter = 0;
      flag2 = 3;

      pixels.setPixelColor(6, pixels.Color(0,0,0));
      pixels.show(); // This sends the updated pixel color to the hardware.    
    }

    if (currentMillis - previousTriggerMillis >= (triggerTime*3) && flag2 == 3) {
    pixels.setPixelColor(5, pixels.Color(50,50,50));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousTriggerMillis = currentMillis; 
    flag2 = 4;
    counter2 = counter2 +1;
    }

    if (currentMillis - previousTriggerMillis >= (triggerTime*3) && flag2 == 4) {
    pixels.setPixelColor(5, pixels.Color(0,0,0));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousTriggerMillis = currentMillis; 
    flag2 = 3;
    counter2 = counter2 +1;
    }

    if (counter2 == 4) {
      counter2 = 0;
      flag2 = 0;
    pixels.setPixelColor(3, pixels.Color(0,0,0));
    pixels.setPixelColor(4, pixels.Color(0,0,0));
    pixels.setPixelColor(3, pixels.Color(0,0,0));
    pixels.setPixelColor(4, pixels.Color(0,0,0));
    pixels.show(); // This sends the updated pixel color to the hardware.
    previousTriggerMillis = currentMillis;

    flag2 = 1;    
    counter3 = counter3 + 1;
    }

    if (counter3 == 2) {
      flag2 = 0;
      counter3 = 0;
    }    
  
  
 
}


