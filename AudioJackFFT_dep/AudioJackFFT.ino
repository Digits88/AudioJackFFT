//
//  Beat Sync
//  A music visualiztion device.
//  Created by
//  Mike Newell
//

#include "fix_fft.h"
#include "Adafruit_NeoPixel.h"

#define N_PIXELS  1  // Number of pixels you are using
#define LED_PIN   6  // NeoPixel LED strand is connected to GPIO #0 / D0
#define SENSITIVITY 10 //  how sensitive the sensor is 9
#define AUDIOPIN 2
#define POT_PIN 5
 
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
}

void loop()
{
  // FFT
  for (i=0; i < 128; i++){                                   
    val = analogRead(AUDIOPIN);                                  
    data[i] = val;                                     
    im[i] = 0;                                                   
  };

  fix_fft(data,im,7,0);

  for (i=0; i< 64;i++){                                    
    data[i] = sqrt(data[i] * data[i] + im[i] * im[i]);  // this gets the absolute value of the values in the
    //array, so we're only dealing with positive numbers
  };   

  // average bars together
  for (i=0; i<14; i++) {
    data_avgs[i] = data[i*4] + data[i*4 + 1] + data[i*4 + 2] + data[i*4 + 3];   // average together
  
    data_avgs[i] = map(data_avgs[i], 0, 30, 0, 9);                              // remap values for LoL
  }
  int value = data_avgs[1];//0 for bass
  
  if(value > 0) {
    Serial.print("value: ");
    Serial.print(value);
    Serial.print(" last value: ");
    Serial.println(lastValue);
    
    
    // fade
    if(lastValue < value) {
      fadeUp(((value * SENSITIVITY) - (lastValue * SENSITIVITY)), value * SENSITIVITY);
    } else if(lastValue > value) {
      fadeDown(((lastValue * SENSITIVITY) - (value * SENSITIVITY)), lastValue * SENSITIVITY);
    }
    
    lastValue = value;
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
//  Serial.println(cos(startHigh));
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
