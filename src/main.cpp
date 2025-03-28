#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS (144 * 2)  // 2 metri * 144 LED/metro
#define MAX_LED_NUMBER_SELECTED (NUM_LEDS * 5)
#define DATA_PIN (3)
#define BUTTON_PIN (4)
#define LEDS_PER_STEP (10)  // Lunghezza della porzione illuminata

CRGB leds[NUM_LEDS];

enum State {
    STATE_IDLE,
    STATE_ROTATING
};

State currentState = STATE_IDLE;
int buttonState = 0;
int lastButtonState = 0;
int rotationPosition = 0;
char ledBrightness = 50;
int randomLedSelected = 1;

const CRGB colorViolaFesta = CRGB(255, 0, 157);
const CRGB colorBluFesta = CRGB(255, 0, 157);

void setIdleAnimation() {
    FastLED.clear();
    fill_gradient_RGB(leds, NUM_LEDS, colorViolaFesta, colorBluFesta, colorViolaFesta, colorBluFesta);
    // fill_noise16(leds, NUM_LEDS, 8, 8, 8, 8, 8, 8, 8);
    // fill_rainbow(leds, NUM_LEDS, 0);
    FastLED.show();
}

void transitionToStateIdle() {
    currentState = STATE_IDLE;
    setIdleAnimation();
}

void setup() {
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // WS2815 è compatibile con WS2812B -> collegare i due data pin insieme
    pinMode(BUTTON_PIN, INPUT);

    FastLED.setBrightness(ledBrightness);

    transitionToStateIdle();  // Inizializza lo stato a idle
}

void readAllInputs() {
    int reading = digitalRead(BUTTON_PIN);
    /*unsigned long currentMillis = millis();

    static unsigned long lastDebounceTime = currentMillis;
    static const unsigned long debounceDelay = 100;

    if (reading != lastButtonState) {
        lastDebounceTime = currentMillis;
    }

    if ((currentMillis - lastDebounceTime) > debounceDelay) {
        lastButtonState = buttonState;
        buttonState = reading;
    }
    */
    buttonState = reading;
}

void handleIdleStateTransitions() {
    if (lastButtonState == LOW && buttonState == HIGH) {  // Pulsante premuto, fronte salita
        currentState = STATE_ROTATING;
        randomLedSelected = random(NUM_LEDS, MAX_LED_NUMBER_SELECTED);
    }
    lastButtonState = buttonState;
}

void fadeall() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i].nscale8(250);
    }
}

void handleRotatingState() {
    uint8_t hue = 0;
    for (int i = 0; i < randomLedSelected; i++) {
        leds[i % NUM_LEDS] = CHSV(hue, 255, 255);

        // leds[i] = CRGB::Black;
        fadeall();

        hue = (hue + 1) % 255;

        FastLED.show();
    }
}

void loop() {
    readAllInputs();

    switch (currentState) {
        case STATE_IDLE:
            handleIdleStateTransitions();
            break;
        case STATE_ROTATING:
            handleRotatingState();
            transitionToStateIdle();
            break;
        default:
            break;
    }
}
