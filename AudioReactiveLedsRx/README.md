Audio Reactive LEDS
===================

Dependancies
------------

[Adafruit_neopixel library](https://github.com/adafruit/Adafruit_NeoPixel)
[fix_fft](https://github.com/newshorts/AudioJackFFT/tree/colorshifting/AudioJackFFT/fix_fft)
[Radio Head](http://www.airspayce.com/mikem/arduino/RadioHead/index.html)

What you need...
----------------

This example includes a fast fourier transform on audio signals coming from a headphone jack. The headphone signal is first filtered by an Op-Amp circuit. You can find a good example of one [here](http://123d.circuits.io/circuits/404587-simple-op-amp-low-pass-filter-for-3-5mm-audio-jack).

Once we've transformed the signal into something manageable, we save the height of the signal and do a quick calculation to see how much we need to fade the rbg led to the new height. I am using ws2812b available from any electronic store online, adafruit, sparkfun, etc.

Finally, once we are ready to do some work on the leds, we need to transmit from our rf transmitter to our receiving adruinos. This can be done using the radio head library (formerly known as virtual wire).  