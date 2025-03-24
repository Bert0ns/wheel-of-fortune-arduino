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

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); //WS2815 è compatibile con WS2812B -> collegare i due data pin insieme
  pinMode(BUTTON_PIN, INPUT);

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
  //delay(50);
}

void readAllInputs() {
  buttonState = digitalRead(BUTTON_PIN);
}

void handleIdleState() {
  FastLED.clear();
  FastLED.show();
}

void handleIdleStateTransitions() {
  if (lastButtonState == LOW && buttonState == HIGH) { // Pulsante premuto, fronte salita
      currentState = STATE_ROTATING;
    }
  lastButtonState = buttonState;
}

void handleRotatingState() {
  for (int i = 0; i < NUM_LEDS; i++) {
    updateRotation();
    FastLED.show();
    delay(10); // Velocità di rotazione
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