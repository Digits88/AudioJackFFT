// TODO: cancel out volume adjustment from the computer
// TODO: make whole strip more active when louder, less active when softer

#include <Adafruit_NeoPixel.h>

#define N_PIXELS  120  // Number of pixels you are using
#define LED_PIN   13  // NeoPixel LED strand is connected to GPIO #0 / D0
#define SPECTRUM 0 // the spectrum to follow
#define HUE_SPEED 0.0001; // .001 takes about 20 minutes

Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

//For spectrum analyzer shield, these three pins are used.
//You can move pinds 4 and 5, but you must cut the trace on the shield and re-route from the 2 jumpers. 
int spectrumReset=5;
int spectrumStrobe=4;
int spectrumAnalog=0;  //0 for left channel, 1 for right.

// leds
float hue;
int skip;

// Spectrum analyzer read values will be kept here.
int Spectrum[7];

// keep a running avg to cancel out volume adjustment
const int numReadings = 150;
int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

void setup() {
//  byte Counter;
  
  Serial.begin(9600);
  
  // leds
  hue = 0.0;
  skip = 7;
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  delay(100);
  
  // running avg
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;

  delay(100);

  //Setup pins to drive the spectrum analyzer. 
  pinMode(spectrumReset, OUTPUT);
  pinMode(spectrumStrobe, OUTPUT);

  //Init spectrum analyzer
  digitalWrite(spectrumStrobe,LOW);
    delay(1);
  digitalWrite(spectrumReset,HIGH);
    delay(1);
  digitalWrite(spectrumStrobe,HIGH);
    delay(1);
  digitalWrite(spectrumStrobe,LOW);
    delay(1);
  digitalWrite(spectrumReset,LOW);
    delay(5);
  // Reading the analyzer now will read the lowest frequency.
  
}

void loop() {

//  int Counter, Counter2, Counter3;

  // slowly adjust the hue overtime to red instead of blue
  hue += HUE_SPEED;

  // read the latest
  readSpectrum();
  
  // RUNNING AVG
  // subtract the last reading:
  total= total - readings[index]; 
  // read from the spectrum:  
  readings[index] = Spectrum[SPECTRUM];
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;
  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;
  // calculate the average:
  average = total / numReadings;
  
  // send it to the computer as ASCII digits
  Serial.println(average);
  
  strip.setBrightness(Spectrum[SPECTRUM]);
  if(skip <= 0) {
    int spread = random(0, 10000)/Spectrum[SPECTRUM]; // get an even spread across the pixels
    uint32_t col = strip.Color(abs(sin(Spectrum[SPECTRUM])*25*hue),abs(cos(Spectrum[SPECTRUM])*25),constrain(Spectrum[SPECTRUM] - hue, 0, 255));
    strip.setPixelColor(spread,col);  
    skip = (1000 * average) / (Spectrum[SPECTRUM]*Spectrum[SPECTRUM]);
  }
  skip--;
  strip.show(); // Update strip 
  
  
  
//  Serial.println(rIdx);
  
  delay(15);  //We wait here for a little while until all the values to the LEDs are written out.
  
}

// Read 7 band equalizer.
void readSpectrum()
{
  // Band 0 = Lowest Frequencies.
  byte Band;
  for(Band=0;Band <7; Band++)
  {
    Spectrum[Band] = (analogRead(spectrumAnalog) + analogRead(spectrumAnalog) ) >>1; //Read twice and take the average by dividing by 2
    
    digitalWrite(spectrumStrobe,HIGH);
    digitalWrite(spectrumStrobe,LOW);
  }
}

    
