#define GREEN_PIN 9
#define RED_PIN 10
#define BLUE_PIN 11

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
};

Hal *hal = new Hal();

void setup() {
  // put your setup code here, to run once:
  hal->setupPins();
}

long refTime = 0;
float blue = 0;
void loop() {
  hal->applyState(&State(0xff, 0x30, 0x8));
  // long ms = millis();
  // // put your main code here, to run repeatedly:
  // if (ms - refTime > 10) {
  //   State state = State(1.0, 0, 0);
  //   hal->applyState(&state);
  //   blue += 0.01;
  //   refTime = ms;
  // }
}
