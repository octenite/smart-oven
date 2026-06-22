
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_AUTH_TOKEN "iLdJIBykaYxpURx-952hz8KWuicCIkgH"


char auth[] = BLYNK_AUTH_TOKEN;// Enter your Auth token
char ssid[] = "Androidx";//Enter your WIFI SSIS
char pass[] = "1234qwert";//Enter your WIFI password
BlynkTimer timer;
int pinValue = 0;

#define Buzzer D8
#define Proxy D1
#define Sound D2
#define Servo D5
#define Gas A0

void setup() {
  Serial.begin(9600);
  pinMode(Buzzer, OUTPUT);
  pinMode(Gas, INPUT);
  pinMode(Proxy,INPUT);
  pinMode(Sound,INPUT);
  pinMode(Servo,OUTPUT);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(100L, Sensorsend);
}
BLYNK_WRITE(V0) {
  pinValue = param.asInt();
}

void Sensorsend(){
  int gas = analogRead(Gas);
      gas = map(Gas, 0, 1024, 0, 100);
        Blynk.virtualWrite(V1, gas);
  int proxy = digitalRead(Proxy);  
        Blynk.virtualWrite(V2, proxy);
  int sound = digitalRead(Sound);
        Blynk.virtualWrite(V3, sound);
}



void loop() {
  Blynk.run();
  timer.run();
}
  /* 
  int Gas = analogRead(Gas);
 // Serial.println(Gas);
  gas = map(Gas, 0, 1024, 0, 100);
  if (pinValue == 1) {
    if (gas <= 50) {
     digitalWrite(Buzzer, LOW);
     //Blynk.logEvent("Gas","Gas value:Normal");
    } 
    else if (gas > 50) {
     // Blynk.logEvent("Gas","Warning! Gas leak detected");
      digitalWrite(Buzzer, HIGH);
      
    }
    Blynk.virtualWrite(V1, gas);
  } else {
    digitalWrite(Buzzer, LOW);
    
  }
  int proxy= digitalRead(Proxy);
  Serial.println(proxy);
  if (pinValue == 1)
  {
    if (proxy== 0 && flag== 0){
      digitalWrite(Buzzer, HIGH);
      delay(1000);
      digitalWrite(Buzzer, LOW);
     //Blynk.logEvent("Utensil","Utensil placed");
     Blynk.virtualWrite(V3, proxy);
    }
    else if (proxy==1){
     // digitalWrite(Buzzer, HIGH);
      //delay(1000);
      digitalWrite(Buzzer, LOW);
    // Blynk.logEvent("Utensil","NO Utensil placed");
     Blynk.virtualWrite(V3, proxy);

    }
  }
  else {
    digitalWrite(Buzzer, LOW);
    Blynk.virtualWrite(V3, proxy);
  } 
  */