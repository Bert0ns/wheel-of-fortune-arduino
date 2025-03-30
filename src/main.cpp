#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS (144 * 2)  // 2 metri * 144 LED/metro
#define MAX_LED_NUMBER_SELECTED (NUM_LEDS * 5)
#define DATA_PIN (3)
#define BUTTON_PIN (4)
#define LEDS_PER_STEP (10)   // Lunghezza della porzione illuminata
#define NUM_PIE_SLICES (8)   // Numero di spicchi della ruota
#define BLINKING_RATE (250)  // Frequenza di lampeggio in millisecondi
#define NUM_BLINKS (12)      // Numero di lampeggi

CRGB leds[NUM_LEDS];

enum State {
    STATE_IDLE,
    STATE_ROTATING,
    STATE_WINNING_ANIMATION,
};

State currentState = STATE_IDLE;
int buttonState = 0;
int lastButtonState = 0;
int rotationPosition = 0;
char ledBrightness = 10;
int randomLedSelected = 1;

const CRGB colorViolaFesta = CRGB(242, 0, 255);
const CRGB colorBluFesta = CRGB(92, 220, 237);

void doIdleAnimation(uint8_t colorIndex = 0) {
    FastLED.clear();
    //fill_gradient_RGB(leds, NUM_LEDS, colorViolaFesta, colorBluFesta, colorViolaFesta, colorBluFesta);
    // fill_noise16(leds, NUM_LEDS, 8, 8, 8, 8, 8, 8, 8);
    // fill_rainbow(leds, NUM_LEDS, 0);
    uint8_t brightness = 255;
    TBlendType currentBlending = LINEARBLEND;
    CRGBPalette16 currentPalette = RainbowColors_p;
    for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }

    FastLED.show();
}

void transitionToStateIdle() {
    currentState = STATE_IDLE;
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

bool BilnkingState = 0;
void doWinningAnimation(int ledSelected) {
    static CRGB winningColor = CRGB(255, 0, 255);
    static CRGB backgroundColor = CRGB(0, 0, 0);

    static int numberOfBlinks = 0;
    if (numberOfBlinks >= NUM_BLINKS) {
        numberOfBlinks = 0;
        transitionToStateIdle();
        return;
    }
    numberOfBlinks++;

    ledSelected = ledSelected % NUM_LEDS;
    int ledPerSlice = NUM_LEDS / NUM_PIE_SLICES;

    if (BilnkingState == 0) {
        fill_solid(leds, NUM_LEDS, backgroundColor);
        BilnkingState = 1;
    } else {
        for (int i = 0; i < NUM_PIE_SLICES; i++) {
            int startLed = i * ledPerSlice;
            int sliceSelected = ledSelected / ledPerSlice;

            if (i == sliceSelected) {
                fill_solid(leds + startLed, ledPerSlice, winningColor);
            } else {
                fill_solid(leds + startLed, ledPerSlice, backgroundColor);
            }
        }
        BilnkingState = 0;
    }
    FastLED.show();
    delay(BLINKING_RATE);
}

void transitionTo_StateWinningAnimation() {
    currentState = STATE_WINNING_ANIMATION;
}

void loop() {
    static uint8_t startIndex = 0;
    readAllInputs();

    switch (currentState) {
        case STATE_IDLE:
            handleIdleStateTransitions();
            startIndex++; /* motion speed */
            doIdleAnimation(startIndex);
            break;
        case STATE_ROTATING:
            handleRotatingState();
            transitionTo_StateWinningAnimation();
            break;
        case STATE_WINNING_ANIMATION:
            doWinningAnimation(randomLedSelected);
            break;
        default:
            break;
    }
}
