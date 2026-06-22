
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#define BLYNK_AUTH_TOKEN "iLdJIBykaYxpURx-952hz8KWuicCIkgH"



int whcount = 0;
char auth[] = BLYNK_AUTH_TOKEN;// Enter your Auth token
char ssid[] = "Androidx";//Enter your WIFI SSIS
char pass[] = "1234qwert";//Enter your WIFI password
int Dflag=0;
BlynkTimer timer;
Servo servo1;

#define CLK D3 // display clk
#define DIO D4 // Display out pin
#define Buzzer D8
#define Proxy D1
#define Sound D2
#define Servo1pin D5
#define Gas A0
SevenSegmentExtended display(CLK, DIO);
BLYNK_WRITE(V4) {
servo1.write(param.asInt());
}
/*
BLYNK_WRITE(V5) {
digitalWrite(D7,param.asInt());
}
*/
void setup() {
  Serial.begin(9600);
  pinMode(Buzzer, OUTPUT);
  pinMode(Gas, INPUT);
  pinMode(Proxy,INPUT);
  pinMode(Sound,INPUT);
  servo1.attach(Servo1pin);//servo  pin defined in Servo1pin
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  display.begin(); // initializes the display
  display.setBacklight(100); // set the brightness to 100 %
  timer.setInterval(100L, Sensorsend);
  timer.setInterval(100L, Detect);
  timer.setInterval(100L, Alarm);
  timer.setInterval(100L, Count);
  timer.setInterval(100L, disp);
  timer.setInterval(100L, Count);
}
//BLYNK_WRITE(V0) {
  //pinValue = param.asInt();


//}

void Sensorsend(){
  int gas = analogRead(Gas);
      gas = map(gas, 0, 1024, 0, 100);
        Blynk.virtualWrite(V1, gas);
  int prox = digitalRead(Proxy);  
        Blynk.virtualWrite(V2, prox);
  int sound = digitalRead(Sound);
        Blynk.virtualWrite(V3, sound);
}


void Detect (){

  int  prox = digitalRead(Proxy);
  if (prox==1 && Dflag==1){
//when utensil is removed
digitalWrite(Buzzer, HIGH);
delay(700);
digitalWrite(Buzzer, LOW);
      Dflag=0;
  }
  else if (prox==0 && Dflag== 0){
  //when utensil is placed
digitalWrite(Buzzer, HIGH);
delay(300);
digitalWrite(Buzzer, LOW);
      Dflag=1;
      
  }
  else {
    digitalWrite(Buzzer, LOW);
    
  }
}

void Count(){
int sound = digitalRead(Sound);
if (sound== 1){
  whcount++;
  Serial.println('\n');
  Serial.print(whcount);
  disp();
  delay(5000);
  
} 
else return ;



}

void disp(){

 // display.printDualCounter(0,whcount);
 display.print(whcount);

}

void Alarm(){
  int gas = analogRead(Gas);
      gas = map(gas, 0, 1024, 0, 100);
    if (gas <= 50) {
     digitalWrite(Buzzer, LOW);
    } 
    else if (gas > 50) {
     // Blynk.logEvent("Gas","Warning! Gas leak detected");
    digitalWrite(Buzzer, HIGH);
    servo1.write(0);
    }
   else {
    digitalWrite(Buzzer, LOW);
    
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
  