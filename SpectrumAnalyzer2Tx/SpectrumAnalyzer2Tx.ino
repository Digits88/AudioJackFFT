// TODO: OPTIMIZE

#include <Adafruit_NeoPixel.h>
#include <VirtualWire.h>

// leds
#define N_PIXELS  120  // Number of pixels you are using
#define LED_PIN   13  // NeoPixel LED strand is connected to GPIO #0 / D0
#define SPECTRUM 0 // the spectrum to follow
#define HUE_SPEED 0.00001; // .001 takes about 20 minutes
Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
float hue;
int skip;

//For spectrum analyzer shield, these three pins are used.
//You can move pinds 4 and 5, but you must cut the trace on the shield and re-route from the 2 jumpers. 
int spectrumReset=5;
int spectrumStrobe=4;
int spectrumAnalog=0;  //0 for left channel, 1 for right.
int Spectrum[7]; // Spectrum analyzer read values will be kept here.

// keep a running avg to cancel out volume adjustment
const int numReadings = 150;
int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

// virtual wire
uint8_t message[2]; // the message to send, spectrum & spread

void setup() {
//  byte Counter;
  
  Serial.begin(9600);
  Serial.println("setup");
  
  initLeds();
  delay(100);
  
  // running avg
  initRunningAvg();
  delay(100);

  initSpectrum();
  delay(100);
  
  initVirtualWire();
  
}

void loop() {

  // read spectrum
  readSpectrum();
  
  // running avg smoothing
  calcRunningAvg();
  
  // leds
  showLeds();
  
  delay(15);  //We wait here for a little while until all the values to the LEDs are written out.
  
}

void initVirtualWire() {
  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
//  vw_setup(2000);	 // Bits per sec
  vw_setup(4800);	 // Bits per sec
  vw_set_tx_pin(3); //Pin 3 is connected to "Digital Output" of receiver
}

void sendMessage(uint8_t * msg) {
  vw_send(msg, 2);
  vw_wait_tx(); // Wait until the whole message is gone
}

void initLeds() {
  // leds
  hue = 0.0;
  skip = 7;
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}


// leds
void showLeds() {
  // send spectrum, spread
//  strip.setBrightness(constrain((Spectrum[SPECTRUM]), 0, 255));
  if(skip <= 0) {
//    float equalizer = (120.0 / average);
//    int spread = constrain(random(0, (average * equalizer)), 0, 120); // get an even spread across the pixels
//    Serial.println(spread);
//    uint32_t col = strip.Color(abs(sin(Spectrum[SPECTRUM])*25*hue),abs(cos(Spectrum[SPECTRUM])*25),constrain(Spectrum[SPECTRUM] - hue, 0, 255));
//    strip.setPixelColor(spread,col);  
    skip = abs((10000*average) / (Spectrum[SPECTRUM]*Spectrum[SPECTRUM]));
    
    // send over virtual wire
    message[0] = Spectrum[SPECTRUM];
    message[1] = average;
    sendMessage(message);
  }
  skip--;
//  strip.show(); // Update strip 
//  hue += HUE_SPEED; // slowly adjust the hue overtime to red instead of blue
}


// running avg
void initRunningAvg() {
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
}

void calcRunningAvg() {
  total= total - readings[index]; // subtract the last reading:
  readings[index] = Spectrum[SPECTRUM]; // read from the spectrum:  
  total= total + readings[index]; // add the reading to the total:
  index = index + 1; // advance to the next position in the array:  
  if (index >= numReadings) // if we're at the end of the array...
    index = 0; // ...wrap around to the beginning: 
  average = total / numReadings; // calculate the average:
}


// spectrum
void initSpectrum() {
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

    
