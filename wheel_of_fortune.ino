#include <FastLED.h>

// Configurazione della striscia LED
#define LED_PIN     3        // Pin di connessione al Data Input della striscia LED
#define NUM_LEDS    288      // Numero totale di LED nella striscia
#define BUTTON_PIN  4        // Pin per il pulsante
#define LEDS_PER_STEP 5      // Numero di LED da illuminare contemporaneamente

// Colori
#define LED_COLOR   CRGB::Blue  // Colore dei LED attivi

// Definizione degli stati della macchina a stati
enum State {
  IDLE,           // In attesa dell'input
  ROTATING,       // Esecuzione della rotazione
};

// Variabili globali
CRGB leds[NUM_LEDS];
State currentState = IDLE;
int currentPosition = 0;
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 50;  // Intervallo di aggiornamento in ms
boolean buttonPressed = false;
boolean lastButtonState = HIGH;  // Assumiamo pull-up
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // Tempo di debounce in ms

void setup() {
  // Inizializzazione della striscia LED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);  // Imposta la luminosità (0-255)
  
  // Imposta il pin del pulsante come ingresso con pull-up interno
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Inizializza tutti i LED spenti
  FastLED.clear();
  FastLED.show();
  
  // Per debug
  Serial.begin(9600);
  Serial.println("Sistema inizializzato");
}

void loop() {
  // Lettura dello stato del pulsante con debouncing
  debounceButton();
  
  // Esecuzione della macchina a stati
  switch (currentState) {
    case IDLE:
      handleIdleState();
      break;
    case ROTATING:
      handleRotatingState();
      break;
  }
}

// Gestisce lo stato di attesa
void handleIdleState() {
  // Se il pulsante viene premuto, avvia la rotazione
  if (buttonPressed) {
    buttonPressed = false;
    currentState = ROTATING;
    Serial.println("Transizione a ROTATING");
  }
}

// Gestisce lo stato di rotazione
void handleRotatingState() {
  unsigned long currentTime = millis();
  
  // Aggiorna la posizione dei LED solo se è passato abbastanza tempo
  if (currentTime - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentTime;
    
    // Spegni tutti i LED
    FastLED.clear();
    
    // Accendi 5 LED consecutivi nella posizione corrente
    for (int i = 0; i < LEDS_PER_STEP; i++) {
      int ledPos = (currentPosition + i) % NUM_LEDS;
      leds[ledPos] = LED_COLOR;
    }
    
    // Mostra i LED
    FastLED.show();
    
    // Incrementa la posizione per la prossima iterazione
    currentPosition = (currentPosition + 1) % NUM_LEDS;
    
    // Se siamo tornati alla posizione iniziale, torna allo stato IDLE
    if (currentPosition == 0) {
      currentState = IDLE;
      Serial.println("Transizione a IDLE");
    }
  }
  
  // Se il pulsante viene premuto di nuovo, ferma la rotazione
  if (buttonPressed) {
    buttonPressed = false;
    currentState = IDLE;
    FastLED.clear();
    FastLED.show();
    Serial.println("Rotazione interrotta, transizione a IDLE");
  }
}

// Gestisce il debounce del pulsante
void debounceButton() {
  // Leggi lo stato corrente del pulsante
  boolean reading = digitalRead(BUTTON_PIN);
  
  // Se lo stato è cambiato, resetta il timer di debounce
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  // Se lo stato è stabile da abbastanza tempo
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Se lo stato è cambiato rispetto all'ultimo stato stabile
    if (reading == LOW && lastButtonState == HIGH) {
      buttonPressed = true;
      Serial.println("Pulsante premuto");
    }
  }
  
  // Salva lo stato di lettura come ultimo stato noto
  lastButtonState = reading;
}
