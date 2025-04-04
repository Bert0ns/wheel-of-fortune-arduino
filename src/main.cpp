#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS (144 * 2 - 24)  // 2 metri * 144 LED/metro
#define MAX_LED_NUMBER_SELECTED (NUM_LEDS * 5)
#define DATA_PIN (7)
#define BUTTON_PIN (4)
#define NUM_PIE_SLICES (8)   // Numero di spicchi della ruota
#define LED_PER_SLICE (NUM_LEDS / NUM_PIE_SLICES)
#define BLINKING_RATE (250)  // Frequenza di lampeggio in millisecondi
#define NUM_TOTAL_BLINKS (12)      // Numero di lampeggi
#define FADE_SCALE (250)  // Fattore di fade (0-255)

CRGB leds[NUM_LEDS];
char ledBrightness = 100; // Luminosità iniziale dei LED (0-255)

enum State {
    IDLE,
    ROTATING,
    WINNING_ANIMATION,
};

State currentState = IDLE;
int buttonState = 0;
int lastButtonState = 0;
int rotationPosition = 0;
int randomLedSelected = 1;

const CRGB colorViolaFesta = CRGB(242, 0, 255);
const CRGB colorBluFesta = CRGB(92, 220, 237);

void fadeLeds(int startLed, int endLed, uint8_t fadeScale);
void fadeAllLeds();
void readAllInputs();
void transitionToState(State newState);

void setup() {
    FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // WS2815 è compatibile con WS2812B -> collegare i due data pin insieme
    pinMode(BUTTON_PIN, INPUT);

    FastLED.setBrightness(ledBrightness);
    FastLED.clear();  // Pulisce i LED all'avvio
    FastLED.show();   // Mostra il colore iniziale (spento)

    transitionToState(State::IDLE);  // Inizializza lo stato a IDLE
}

void handleIdleStateTransitions() {
    if (lastButtonState == LOW && buttonState == HIGH) {  // Pulsante premuto, fronte salita
        randomLedSelected = random(NUM_LEDS, MAX_LED_NUMBER_SELECTED);
        transitionToState(State::ROTATING);  // Passa allo stato ROTATING
    }
    lastButtonState = buttonState;
}

void handleRotatingState() {
    uint8_t hue = 0;
    for (int i = 0; i < randomLedSelected; i++) {
        int currentLed = i % NUM_LEDS;
        leds[currentLed] = CHSV(hue, 255, 255);
        fadeAllLeds();
        //fadeLeds(0, currentLed);
        hue = (hue + 1) % 255;
        FastLED.show();
    }
}

void setIdleAnimation(int animationNumber, CRGBPalette16 &palette, TBlendType &blending, uint8_t &brightness) {
    switch (animationNumber) {
        case 1:
            palette = RainbowStripeColors_p;
            blending = NOBLEND;
            brightness = 240;
            break;
        case 2:
            palette = PartyColors_p;
            blending = NOBLEND;
            brightness = 170;
            break;
        case 3:
            palette = RainbowStripeColors_p;
            blending = LINEARBLEND;
            brightness = 220;
            break;
        case 0:
        default:
            palette = RainbowColors_p;
            blending = LINEARBLEND;
            brightness = 170;
            break;
    }
}

void doIdleAnimation(uint8_t colorIndex = 0) {
    static const int NUMBER_OF_IDLE_ANIMATIONS = 4;  // Numero di animazioni idle
    static const int TIME_PER_ANIMATION = 10000;  // Tempo di animazione in millisecondi
    static int animationNumber = 0;
    static long animationStartTime = millis();
    static uint8_t brightness = 255;
    static TBlendType currentBlending = LINEARBLEND;
    static CRGBPalette16 currentPalette = RainbowColors_p;

    long currentTime = millis();
    if (currentTime - animationStartTime >= TIME_PER_ANIMATION) {
        animationStartTime = currentTime;
        animationNumber = (animationNumber + 1) % NUMBER_OF_IDLE_ANIMATIONS;  // Cambia animazione
        setIdleAnimation(animationNumber, currentPalette, currentBlending, brightness);
        FastLED.clear();
    }

    if(animationNumber==1){
        delay(10); 
    }
    
    for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }

    FastLED.show();
}

void doWinningAnimation(int ledSelected) {
    static const CRGB winningColor1 = CRGB(255, 30, 255);
    static const CRGB winningColor2 = CRGB(225, 100, 42);
    static const CRGB backgroundColor = CRGB(0, 0, 0);
    static int numberOfBlinks = 0;

    if (numberOfBlinks >= NUM_TOTAL_BLINKS) {
        numberOfBlinks = 0;
        transitionToState(State::IDLE);
        return;
    }

    if (numberOfBlinks % 2 == 0) {
        for (int i = 0; i < NUM_PIE_SLICES; i++) {
            int startLed = i * LED_PER_SLICE;
            int sliceSelected = ledSelected / LED_PER_SLICE;

            if (i == sliceSelected) {
                fill_solid(leds + startLed, LED_PER_SLICE, winningColor2);
            } else {
                fill_solid(leds + startLed, LED_PER_SLICE, backgroundColor);
            }
        }
    } else {
        for (int i = 0; i < NUM_PIE_SLICES; i++) {
            int startLed = i * LED_PER_SLICE;
            int sliceSelected = ledSelected / LED_PER_SLICE;

            if (i == sliceSelected) {
                fill_solid(leds + startLed, LED_PER_SLICE, winningColor1);
            } else {
                fill_solid(leds + startLed, LED_PER_SLICE, backgroundColor);
            }
        }
    }
    numberOfBlinks++;
    FastLED.show();
    delay(BLINKING_RATE);
}

void transitionToState(State newState) {
    currentState = newState;

    if (newState == State::IDLE) {
        ledBrightness = 70;
        FastLED.setBrightness(ledBrightness);
    }
    else if (newState == State::ROTATING) {
        ledBrightness = 200;
        FastLED.setBrightness(ledBrightness);
    }
    else if (newState == State::WINNING_ANIMATION) {
        ledBrightness = 200;
        FastLED.setBrightness(ledBrightness);
    }
}

void loop() {
    static uint8_t startIndex = 0;
    readAllInputs();

    switch (currentState) {
        case ROTATING:
            handleRotatingState();

            transitionToState(State::WINNING_ANIMATION);
            break;
        case WINNING_ANIMATION:
            doWinningAnimation(randomLedSelected % NUM_LEDS);
            break;
        case IDLE:
        default:
            handleIdleStateTransitions();
            startIndex++; /* motion speed */
            doIdleAnimation(startIndex);
            break;
    }
}

void readAllInputs() {
    int reading = digitalRead(BUTTON_PIN);
    buttonState = reading;
}

void fadeAllLeds() {
    fadeLeds(0, NUM_LEDS, FADE_SCALE);  // Fade all LEDs
}

void fadeLeds(int startLed, int endLed, uint8_t fadeScale = FADE_SCALE) {
    for (int i = startLed; i < endLed; i++) {
        leds[i].nscale8(FADE_SCALE);
    }
}
