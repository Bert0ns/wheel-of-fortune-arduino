#include <FastLED.h>

#define NUM_LEDS (144 * 2) // 2 metri * 144 LED/metro
#define DATA_PIN (3)
#define BUTTON_PIN (4)
#define LEDS_PER_STEP (10) // Lunghezza della porzione illuminata

CRGB leds[NUM_LEDS];

enum State {
  STATE_IDLE,
  STATE_ROTATING
};

State currentState = STATE_IDLE;
int buttonState = 0;
int lastButtonState = 0;
int rotationPosition = 0;
char ledBrightness = 100;

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); //WS2815 è compatibile con WS2812B -> collegare i due data pin insieme
  pinMode(BUTTON_PIN, INPUT);

  FastLED.setBrightness(ledBrightness);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  readAllInputs();

  switch (currentState) {
    case STATE_IDLE:
      handleIdleState();
      handleIdleStateTransitions();
      break;
    case STATE_ROTATING:
      handleRotatingState();
      handleRotatingStateTransitions();
      break;
  }
}

void readAllInputs() {
  buttonState = digitalRead(BUTTON_PIN);
}

void handleIdleState() {
  FastLED.clear();
  //fill_noise16(leds, NUM_LEDS, 8, 8, 8, 8, 8, 8, 8);
  //fill_rainbow(leds, NUM_LEDS, 0);
  FastLED.show();
}

void handleIdleStateTransitions() {
  if (lastButtonState == LOW && buttonState == HIGH) { // Pulsante premuto, fronte salita
      currentState = STATE_ROTATING;
    }
  lastButtonState = buttonState;
}

void handleRotatingState() {
  /*
  for (int i = 0; i < NUM_LEDS; i++) {
    updateRotation();
    FastLED.show();
    delay(10); // Velocità di rotazione
  }
  */
  uint8_t hue = 0;
	for(int i = 0; i < NUM_LEDS; i++) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show(); 
		// now that we've shown the leds, reset the i'th led to black
		//leds[i] = CRGB::Black;
		fadeall();
	}
}

void handleRotatingStateTransitions(){
  currentState = STATE_IDLE; // Torna allo stato IDLE dopo la rotazione
}

void updateRotation() {
  fill_solid(leds, NUM_LEDS, CRGB::Black); // Spegni tutti i LED

  for (int i = 0; i < LEDS_PER_STEP; i++) {
    leds[(rotationPosition + i) % NUM_LEDS] = CRGB::BlueViolet; // Accendi la porzione
  }

  rotationPosition = (rotationPosition + 1) % NUM_LEDS; // Aggiorna la posizione
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }