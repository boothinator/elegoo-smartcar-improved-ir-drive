#include <IRremote.h>
#include <IRremoteInt.h>


#define ENABLE_LEFT 5
#define ENABLE_RIGHT 6
#define INPUT_1 7
#define INPUT_2 8
#define INPUT_3 9
#define INPUT_4 11

#define RECV_PIN 12
decode_results results;
IRrecv irrecv(RECV_PIN);

#define CONTINUE_CODE 4294967295
#define FORWARD_CODE 16736925 // FF 629D
#define FORWARD_CODE_ALT 5316027
#define REVERSE_CODE 16754775
#define REVERSE_CODE_ALT 2747854299
#define LEFT_CODE 16720605
#define LEFT_CODE_ALT 1386468383
#define RIGHT_CODE 16761405
#define RIGHT_CODE_ALT 553536955
#define OK_CODE 16712445
#define OK_CODE_ALT 3622325019

unsigned long lastIRCode = 0;
unsigned long lastIRCodeTime = 0;
unsigned long currentIRCode = 0;
unsigned long currentIRCodeTime = 0;
int motorPower = 0;

void setup() {
  // put your setup code here, to run once:

  pinMode(ENABLE_LEFT, OUTPUT);
  pinMode(ENABLE_RIGHT, OUTPUT);
  pinMode(INPUT_1, OUTPUT);
  pinMode(INPUT_2, OUTPUT);
  pinMode(INPUT_3, OUTPUT);
  pinMode(INPUT_4, OUTPUT);

  irrecv.enableIRIn();
  Serial.begin(9600);  // debug output at 9600 baud
}

void setPower() {
  analogWrite(ENABLE_LEFT, motorPower);
  analogWrite(ENABLE_RIGHT, motorPower);
}

void forward() {
  analogWrite(ENABLE_LEFT, motorPower);
  digitalWrite(INPUT_1, HIGH);
  digitalWrite(INPUT_2, LOW);
  
  analogWrite(ENABLE_RIGHT, motorPower);
  digitalWrite(INPUT_3, LOW);
  digitalWrite(INPUT_4, HIGH);
}

void reverse() {
  analogWrite(ENABLE_LEFT, motorPower);
  digitalWrite(INPUT_1, LOW);
  digitalWrite(INPUT_2, HIGH);
  
  analogWrite(ENABLE_RIGHT, motorPower);
  digitalWrite(INPUT_3, HIGH);
  digitalWrite(INPUT_4, LOW);
}

void brake() {
  analogWrite(ENABLE_LEFT, 0);
  digitalWrite(INPUT_1, HIGH);
  digitalWrite(INPUT_2, HIGH);
  
  analogWrite(ENABLE_RIGHT, 0);
  digitalWrite(INPUT_3, HIGH);
  digitalWrite(INPUT_4, HIGH);
}

void coast() {
  analogWrite(ENABLE_LEFT, 0);
  digitalWrite(INPUT_1, LOW);
  digitalWrite(INPUT_2, LOW);
  
  analogWrite(ENABLE_RIGHT, 0);
  digitalWrite(INPUT_3, LOW);
  digitalWrite(INPUT_4, LOW);
}

void right() {
  analogWrite(ENABLE_LEFT, motorPower);
  digitalWrite(INPUT_1, HIGH);
  digitalWrite(INPUT_2, LOW);
  
  analogWrite(ENABLE_RIGHT, motorPower);
  digitalWrite(INPUT_3, HIGH);
  digitalWrite(INPUT_4, LOW);
}

void left() {
  analogWrite(ENABLE_LEFT, motorPower);
  digitalWrite(INPUT_1, LOW);
  digitalWrite(INPUT_2, HIGH);
  
  analogWrite(ENABLE_RIGHT, motorPower);
  digitalWrite(INPUT_3, LOW);
  digitalWrite(INPUT_4, HIGH);
}

void loop() {
  
  if (irrecv.decode(&results)) {
    lastIRCode = results.value;
    irrecv.resume();
    
    lastIRCodeTime = millis();
    Serial.println(lastIRCode);

    if (lastIRCode == CONTINUE_CODE) {
      
    } else {
      currentIRCode = lastIRCode;
      currentIRCodeTime = millis();
    }
    
    if (lastIRCode == FORWARD_CODE || lastIRCode == FORWARD_CODE_ALT) {
      motorPower = 100;
      forward();
    } else if (lastIRCode == REVERSE_CODE || lastIRCode == REVERSE_CODE_ALT) {
      motorPower = 100;
      reverse();
    } else if (lastIRCode == LEFT_CODE || lastIRCode == LEFT_CODE_ALT) {
      motorPower = 150;
      left();
    } else if (lastIRCode == RIGHT_CODE || lastIRCode == RIGHT_CODE_ALT) {
      motorPower = 150;
      right();
    } else if (lastIRCode == OK_CODE || lastIRCode == OK_CODE_ALT) {
      motorPower = 0;
      brake();
    }


  }
  
  if (currentIRCodeTime > 0) {
    unsigned long commandTime = millis() - currentIRCodeTime;
    unsigned long irCodeTime = millis() - lastIRCodeTime;

    // Command continuation
    if (!(irCodeTime < 200 && (lastIRCode == CONTINUE_CODE || lastIRCode == currentIRCode))) {
      Serial.println("Cancelling command");
      currentIRCode = 0;
      currentIRCodeTime = 0;
      motorPower = 0;
      commandTime = 0;
      coast();
    }
    
    // Power ramp up
    if (commandTime > 800 && motorPower < 250) {
      Serial.println("Increasing power to 250");
      motorPower = 250;
      setPower();
    } else if (commandTime > 600 && motorPower < 200) {
      Serial.println("Increasing power to 200");
      motorPower = 200;
      setPower();
    } else if (commandTime > 400 && motorPower < 150) {
      Serial.println("Increasing power to 150");
      motorPower = 150;
      setPower();
    }
  }
}
