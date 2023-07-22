#define GREEN_PIN 9
#define RED_PIN 10
#define BLUE_PIN 11

#define UP_PIN PIN_A5
#define DOWN_PIN PIN_A4

#define NUM_STATES 4

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) < (y) ? (y) : (x))

class State {
public:
  byte r = 0;
  byte g = 0;
  byte b = 0;

  State(byte r, byte g, byte b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }
};

State *states[NUM_STATES] = {
  new State(0x80, 0x15, 0x2),   // White
  new State(0xFF, 0x30, 0x8),   // Orange warm
  new State(0xFF, 0x60, 0x30),  // Wake up
  new State(0xFF, 0x80, 0x80)   // Daylight
};

class StateRepository {
private:
  int index = 1;
  State **states;
  int maxIndex;

public:
  StateRepository(State **states) {
    this->states = states;
    this->maxIndex = NUM_STATES - 1;
  }

  State *getCurrentState() {
    return states[index];
  }

  State *next() {
    index = min(index + 1, maxIndex);
    return index;
  }

  State *prev() {
    index = max(index - 1, 0);
    return index;
  }
};

class Hal {
private:
  byte redPin = RED_PIN;
  byte greenPin = GREEN_PIN;
  byte bluePin = BLUE_PIN;

public:
  void setupPins() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
  }

  void applyState(State *state) {
    analogWrite(redPin, state->r);
    analogWrite(greenPin, state->g);
    analogWrite(bluePin, state->b);
  }

  bool readUpButton() {
    return digitalRead(UP_PIN);
  }

  bool readDownButton() {
    return digitalRead(DOWN_PIN);
  }
};

Hal *hal = new Hal();
StateRepository *repo = new StateRepository(states);

void setup() {
  // put your setup code here, to run once:
  hal->setupPins();
}

enum LogicLevel {
  LogicLevel_None,
  LogicLevel_Rise,
  LogicLevel_Fall,
  LogicLevel_Hi,
  LogicLevel_Lo
};

class Debouncer {
private:
  int sampleDurationMicro = 100000;
  int count = sampleDurationMicro / 2;
  long lastSampleMicros = 0;
  int resolutionUs = 1000;

public:
  LogicLevel onLevel(bool level) {
    int previousCount = count;
    long elapsedMicros = (micros() - lastSampleMicros);
    lastSampleMicros = micros();
    count = level
              ? min(count + elapsedMicros, sampleDurationMicro)
              : max(count - elapsedMicros, 0);
    if (count == 0) {
      return previousCount > count ? LogicLevel_Fall : LogicLevel_Lo;
    } else if (count == sampleDurationMicro) {
      return previousCount < count ? LogicLevel_Rise : LogicLevel_Hi;
    } else {
      return LogicLevel_None;
    }
  }
};

bool anythingChanged = true;
Debouncer *upArrowDebouncer = new Debouncer();
Debouncer *downArrowDebouncer = new Debouncer();

void loop() {
  if (anythingChanged) {
    anythingChanged = false;
    State *newState = repo->getCurrentState();
    hal->applyState(newState);
  }

  if (upArrowDebouncer->onLevel(hal->readUpButton() == LogicLevel_Rise)) {
    anythingChanged = true;
    repo->next();
  } else if (downArrowDebouncer->onLevel(hal->readDownButton() == LogicLevel_Rise)) {
    anythingChanged = true;
    repo->prev();
  }


  //  hal->applyState(&State(0xff, 0x30, 0x8));
  // long ms = millis();
  // // put your main code here, to run repeatedly:
  // if (ms - refTime > 10) {
  //   State state = State(1.0, 0, 0);
  //   hal->applyState(&state);
  //   blue += 0.01;
  //   refTime = ms;
  // }
}
