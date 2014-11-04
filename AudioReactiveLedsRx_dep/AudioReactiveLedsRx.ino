// Audio reactive LEDs Receiver

//
//  Beat Sync
//  A music visualiztion device.
//  Created by
//  Mike Newell
//

#include "RH_ASK.h"
#include <SPI.h> // Not actualy used but needed to compile

#include "fix_fft.h"
#include "Adafruit_NeoPixel.h"

#define N_PIXELS  1  // Number of pixels you are using
#define LED_PIN   6  // NeoPixel LED strand is connected to GPIO #0 / D0
#define SENSITIVITY 10 //  how sensitive the sensor is 9
#define AUDIOPIN 2
#define POT_PIN 5

RH_ASK driver;
 
Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// fading
int lastValue = 0;

//  FFT
int x = 0;
char im[128], data[128];
char data_avgs[14];
int i=0,val;


void setup()
{
  strip.begin();
  Serial.begin(9600);
  
  if (!driver.init())
     Serial.println("RF init failed");
}

void loop()
{
  
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  // PUT LED RECEIVED IN HERE
  if (driver.recv(buf, &buflen)) {
    int i;
    
    Serial.print("Buffer length: ");
    Serial.print(buflen);
    Serial.print(" Brightness: ");
    Serial.print((uint8_t)buf[0]);
    Serial.print(" R: ");
    Serial.print((uint8_t)buf[1]);
    Serial.print(" G: ");
    Serial.print((uint8_t)buf[2]);
    Serial.print(" B: ");
    Serial.println((uint8_t)buf[3]);

//    printBuffer("got: ", buf, buflen);
    
    strip.setBrightness(buf[0]);
    strip.setPixelColor(0,strip.Color((uint8_t)buf[1],(uint8_t)buf[2],(uint8_t)buf[3]));
    strip.show(); // Update strip 
    
    // Message with a good checksum received, dump it.
//    driver.printBuffer("Got:", buf, buflen);
    
    
    
    // POWER THE LIGHTS HERE
    // FIGURE OUT WHAT I NEED TO SAY TO FADE UP & DOWN
//    if(lastValue < value) {
//      fadeUp(((value * SENSITIVITY) - (lastValue * SENSITIVITY)), value * SENSITIVITY);
//    } else if(lastValue > value) {
//      fadeDown(((lastValue * SENSITIVITY) - (value * SENSITIVITY)), lastValue * SENSITIVITY);
//    }
   
  }
  
}

void fadeUp(int diff, int startLow) {
  Serial.println(sin(startLow));
  int count = 0;
  while(diff > count) {
    write_strip((startLow + count));
    count++;
    delayMicroseconds(1);
  }
} 

void fadeDown(int diff, int startHigh) {
  Serial.println(cos(startHigh));
  int count = 0;
  while(diff > count) {
    write_strip((startHigh - count));
    count++;
    delayMicroseconds(diff*10);
  }
}

void write_strip(int brightness) {
  if(brightness > 255) {
    brightness = 255;
  }
  strip.setBrightness(brightness);
  strip.setPixelColor(0,strip.Color(abs(sin(brightness)*25),brightness,255));
  strip.show(); // Update strip 
}

// Diagnostic help
void printBuffer(const char* prompt, const uint8_t* buf, uint8_t len)
{
    uint8_t i;

    Serial.println(prompt);
    for (i = 0; i < len; i++)
    {
	if (i % 16 == 15)
	    Serial.println(buf[i], HEX);
	else
	{
	    Serial.print(buf[i], HEX);
	    Serial.print(' ');
	}
    }
    Serial.println("");
}
