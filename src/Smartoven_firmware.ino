#define BLYNK_TEMPLATE_ID "TMPL3msp38OA8"
#define BLYNK_TEMPLATE_NAME "SmartOven"
#define BLYNK_AUTH_TOKEN "o-2_0XGVMHX3p3M0gk1pcnx0tNag7-VS"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>

char ssid[] = "Androidx";
char pass[] = "1234qwert";
char auth[] = "o-2_0XGVMHX3p3M0gk1pcnx0tNag7-VS";

int Dflag = 0;
int Cflag = 0;
int whcount = 0;
int Walarmflag = 3;
int lastGasValue = -1;
int lastProxValue = -1;
int lastSoundValue = -1;
int blynkprox = 0;
int dispreset = 0;

BlynkTimer timer;
Servo servo1;

#define CLK D3
#define DIO D4
#define Buzzer D8
#define Proxy D1
#define Sound D2
#define Servo1pin D5
#define Gas A0

SevenSegmentExtended display(CLK, DIO);

// === Recipe Mode Variables ===
int recipeMode = 0;
bool recipeActive = false;
unsigned long recipeStartTime = 0;
int recipeDuration = 0;

BLYNK_WRITE(V4) {
  servo1.write(180 - param.asInt());
}

BLYNK_WRITE(V6) {
  Cflag = param.asInt();
}

BLYNK_WRITE(V7) {
  Walarmflag = param.asInt();
}

BLYNK_WRITE(V11) {
  recipeMode = param.asInt();
  int prox = digitalRead(Proxy);

  if (prox == 0) { // utensil is placed
    switch (recipeMode) {
      case 1: recipeDuration = 30000; break; // Quick Noodles
      case 2: recipeDuration = 60000; break; // Boiled Eggs
      case 3: recipeDuration = 90000; break; // Rice
      default: recipeDuration = 0; break;
    }

    if (recipeDuration > 0) {
      servo1.write(90); // start
      recipeStartTime = millis();
      recipeActive = true;
      String msg = String("Started recipe mode: ") + recipeMode;
      Blynk.logEvent("recipe_started", msg);
    }
  } else {
    Blynk.virtualWrite(V11, 0);
    Blynk.logEvent("no_utensil", "Place utensil before starting recipe.");
  }
}

String getRecipeName(int mode) {
  switch (mode) {
    case 1: return "Quick Noodles";
    case 2: return "Boiled Eggs";
    case 3: return "Rice";
    default: return "Unknown Recipe";
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(Buzzer, OUTPUT);
  pinMode(Gas, INPUT);
  pinMode(Proxy, INPUT);
  pinMode(Sound, INPUT);
  servo1.attach(Servo1pin, 500, 2400);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  display.begin();
  display.setBacklight(100);
  servo1.write(180);

  timer.setInterval(1000L, Sensorsend);
  timer.setInterval(1000L, Detect);
  timer.setInterval(1000L, GAlarm);
  timer.setInterval(1000L, Count);
  timer.setInterval(1000L, WAlarm);
}

void Sensorsend() {
  int gas = analogRead(Gas);
  gas = map(gas, 0, 1024, 0, 100);
  int prox = digitalRead(Proxy);
  int sound = digitalRead(Sound);

  if (abs(gas - lastGasValue) >= 5) {
    Blynk.virtualWrite(V1, gas);
    lastGasValue = gas;
  }

  if (prox != lastProxValue) {
    blynkprox = 1 - prox;
    Blynk.virtualWrite(V2, blynkprox);
    lastProxValue = prox;
  }

  if (sound != lastSoundValue) {
    Blynk.virtualWrite(V3, sound);
    lastSoundValue = sound;
  }
}

void Detect() {
  int prox = digitalRead(Proxy);
  if (prox == 1 && Dflag == 1) {
    digitalWrite(Buzzer, HIGH);
    delay(700);
    digitalWrite(Buzzer, LOW);
    Dflag = 0;
  } else if (prox == 0 && Dflag == 0) {
    digitalWrite(Buzzer, HIGH);
    delay(200);
    digitalWrite(Buzzer, LOW);
    Dflag = 1;
  } else {
    digitalWrite(Buzzer, LOW);
  }
}

void Count() {
  int sound = digitalRead(Sound);
  if (Cflag == 1 && sound == 1) {
    delay(1000);
    if (sound == 1) {
      whcount++;
      Serial.println('\n');
      Serial.print(whcount);
      disp();
      delay(5000);
    }
  } else {
    if (Cflag == 1) disp();
    else dispidle();
  }
}

void WAlarm() {
  if (whcount == Walarmflag && Cflag == 1) {
    Blynk.logEvent("cooking_done", "Pressure Cooking Done!!");
    beep();
    servo1.write(180);
  } else {
    digitalWrite(Buzzer, LOW);
  }
}

void GAlarm() {
  int gas = analogRead(Gas);
  gas = map(gas, 0, 1024, 0, 100);
  if (gas > 50) {
    Blynk.logEvent("gas_leak_detected", "Warning! Gas leak detected. Check your Cooktop immediately");
    beep();
    servo1.write(180);
  } else {
    digitalWrite(Buzzer, LOW);
  }
}

void disp() {
  display.on();
  display.clear();
  display.home();
  display.print(whcount);
}

void dispidle() {
  display.clear();
  whcount = 0;
  display.print(dispreset);
}

void beep() {
  digitalWrite(Buzzer, HIGH);
  delay(400);
  digitalWrite(Buzzer, LOW);
  delay(800);
}

void handleRecipeMode() {
  if (recipeActive && millis() - recipeStartTime >= recipeDuration) {
    servo1.write(180); // stop
    recipeActive = false;
    String msg = "Started: " + getRecipeName(recipeMode);
    Blynk.logEvent("recipe_done", msg);
    Blynk.virtualWrite(V11, 0); // reset selection
  }
}

void loop() {
  Blynk.run();
  timer.run();
  handleRecipeMode();
}
