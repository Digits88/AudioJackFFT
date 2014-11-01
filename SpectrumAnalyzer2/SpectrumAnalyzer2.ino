// TODO: cancel out volume adjustment from the computer
// TODO: make whole strip more active when louder, less active when softer

#include <Adafruit_NeoPixel.h>

#define N_PIXELS  120  // Number of pixels you are using
#define LED_PIN   13  // NeoPixel LED strand is connected to GPIO #0 / D0
#define SPECTRUM 0 // the spectrum to follow
#define HUE_SPEED 0.001; // .001 takes about 20 minutes

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

void setup() {
  byte Counter;
  
  Serial.begin(9600);
  
  hue = 0.0;
  skip = 7;
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

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

  int Counter, Counter2, Counter3;
  
  // slowly adjust the hue overtime to red instead of blue
  hue += HUE_SPEED;
    
//  showSpectrum();
  readSpectrum();
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
    Serial.println(Spectrum[SPECTRUM]);
    digitalWrite(spectrumStrobe,HIGH);
    digitalWrite(spectrumStrobe,LOW);

    strip.setBrightness(Spectrum[SPECTRUM]);
      
      if(skip <= 0) {
        int spread = random(0, 10000)/Spectrum[SPECTRUM]; // get an even spread across the pixels
        uint32_t col = strip.Color(abs(sin(Spectrum[SPECTRUM])*25*hue),abs(cos(Spectrum[SPECTRUM])*25),constrain(Spectrum[SPECTRUM] - hue, 0, 255));
        strip.setPixelColor(spread,col);  
        skip = 10000 / (Spectrum[SPECTRUM]*Spectrum[SPECTRUM]);
      }
      skip--;
      
    strip.show(); // Update strip 
  }
}

    
