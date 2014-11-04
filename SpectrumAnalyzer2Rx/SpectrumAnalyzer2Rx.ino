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

void setup() {
//  byte Counter;
  
  Serial.begin(9600);
  Serial.println("setup");
  
  initLeds();
  delay(100);
  
  initVirtualWire();
  
}

void loop() {
  
  readVirtualWire();
  
}

void initVirtualWire() {
  //Initialize the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
//  vw_setup(2000);	    // Bits per sec
  vw_setup(4800);	    // Bits per sec
  vw_set_rx_pin(2);         //Pin 2 is connected to "Digital Output" of receiver
  vw_rx_start();           // Start the receiver PLL running
}

void readVirtualWire() {
  //Set buffer array based on max message length
  uint8_t buf[VW_MAX_MESSAGE_LEN];

  //Set variable to indicate buffer length
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    
//    digitalWrite(13, true); // Flash status LED to show received data
//    Serial.print("Got: "); // Message with a good checksum received, dump it.
    
    Spectrum[SPECTRUM] = buf[0];
    showLeds((int)buf[1]);

//    for (int i = 0; i < buflen; i++)
//    {
//      Serial.print(buf[i]); // Print message received in buffer through this loop
//      Serial.print(" "); //add space to distinguish characters from each other if showing ASCII decimal #
//      
////      if(buf[i] == (uint8_t)54) {
////        Serial.println("able to match 54");
////      }
//    }
    
//    Serial.println(""); //Print next character on new line
//    digitalWrite(13, false); //Turn off status LED
  }
}

void initLeds() {
  // leds
  hue = 0.0;
  skip = 7;
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}


// leds
void showLeds(int average) {
  strip.setBrightness(constrain((Spectrum[SPECTRUM]), 0, 255));
  float equalizer = (120.0 / average);
  int spread = constrain(random(0, (average * equalizer)), 0, 120); // get an even spread across the pixels
  uint32_t col = strip.Color(abs(sin(Spectrum[SPECTRUM])*25*hue),abs(cos(Spectrum[SPECTRUM])*25),constrain(Spectrum[SPECTRUM] - hue, 0, 255));
  strip.setPixelColor(spread,col);  
  strip.show(); // Update strip 
  hue += HUE_SPEED; // slowly adjust the hue overtime to red instead of blue
}

    
