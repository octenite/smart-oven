#define BLYNK_TEMPLATE_ID "TMPL3msp38OA8"
#define BLYNK_TEMPLATE_NAME "SmartOven"
#define BLYNK_AUTH_TOKEN "o-2_0XGVMHX3p3M0gk1pcnx0tNag7-VS"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
//#include <SevenSegmentFun.h>

//This is firmware version: 1.8


//char auth[] = BLYNK_AUTH_TOKEN;// Enter your Auth token
char ssid[] = "Androidx";//Enter your WIFI SSIS
char pass[] = "1234qwert";//Enter your WIFI password
char auth[] = "o-2_0XGVMHX3p3M0gk1pcnx0tNag7-VS";// device auth token
int Dflag=0; // flag for Utensil detection
int Cflag=0; // flag for enabling whistle count function
int whcount = 0; // whistle count variable
int Walarmflag= 3; // variable to trigger whistle alarm
int lastGasValue = -1;  // Last known gas value to track changes
int lastProxValue = -1; // Last known proximity value
int lastSoundValue = -1; // Last known sound value
int blynkprox= 0;

int dispreset= 0;
BlynkTimer timer;
Servo servo1;

#define CLK D3 // display clk
#define DIO D4 // Display out pin
#define Buzzer D8 //Buzzer Pin
#define Proxy D1 //IR Proximity Sensor pin
#define Sound D2 // Sound Sensor Pin
#define Servo1pin D5 // Servo motor Data Pin
#define Gas A0 // MQ2 gas sensor Analog Pin
SevenSegmentExtended display(CLK, DIO);
//SevenSegmentFun Fundisplay(CLK, DIO);

BLYNK_WRITE(V4) {
servo1.write(180-param.asInt()); //for reciving data from V4 blynk virtual pin and changing servo value
}
BLYNK_WRITE(V6){
  Cflag=(param.asInt()); // data from V6 blynk for enabling/disabling count functon
}
BLYNK_WRITE(V7){
  Walarmflag=(param.asInt());
}
void setup() {
  Serial.begin(9600);
  pinMode(Buzzer, OUTPUT);
  pinMode(Gas, INPUT);
  pinMode(Proxy,INPUT);
  pinMode(Sound,INPUT);
  servo1.attach(Servo1pin,500,2400);//servo  pin defined in Servo1pin
  Blynk.begin(auth,ssid, pass, "blynk.cloud", 80);

  display.begin(); // initializes the display
  display.setBacklight(100); // set the brightness to 100 %
  servo1.write(180); // set servo to visual 0 indicator
  timer.setInterval(1000L, Sensorsend);
  timer.setInterval(1000L, Detect);
  timer.setInterval(1000L, GAlarm);
  timer.setInterval(1000L, Count);
  timer.setInterval(1000L, WAlarm);
  //timer.setInterval(100L, disp);
}

void Sensorsend(){
  int gas = analogRead(Gas);
      gas = map(gas, 0, 1024, 0, 100);
       // Blynk.virtualWrite(V1, gas);
  int prox = digitalRead(Proxy);  
        //Blynk.virtualWrite(V2, prox);
  int sound = digitalRead(Sound);
        //Blynk.virtualWrite(V3, sound);


         // Only send data if it has changed
  if (abs(gas- lastGasValue) >= 5) {
    Blynk.virtualWrite(V1, gas);
    lastGasValue = gas;
  }

  if (prox != lastProxValue) {
    blynkprox = 1- prox;
    Blynk.virtualWrite(V2, blynkprox );
    lastProxValue = prox;
  }

  if (sound != lastSoundValue) {
    Blynk.virtualWrite(V3, sound);
    lastSoundValue = sound;
  }
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
delay(200);
digitalWrite(Buzzer, LOW);
      Dflag=1;
      
  }
  else {
    digitalWrite(Buzzer, LOW);
    
  }
}

void Count(){
int sound = digitalRead(Sound);
if (Cflag==1 && sound==1 ){

  delay(1000);
  if (sound==1){
  whcount++;
  Serial.println('\n');
  Serial.print(whcount);
  disp();
  //display.print(whcount);
  delay(5000);
  }
  else return;

}

else{
  if (Cflag==1){
  disp();
}
else dispidle();
    }  
           }


void WAlarm(){
  if (whcount==Walarmflag && Cflag==1){
   Blynk.logEvent("cooking_done","Pressure Cooking Done!!");
   
  beep();
  servo1.write(180);
  }
  else digitalWrite(Buzzer, LOW);
  
}


void disp(){
  display.on();
  display.clear();
  display.home();
 // display.printDualCounter(0,whcount);
 display.print(whcount);

}

void dispidle(){
display.clear();
whcount=0;
display.print(dispreset);  }

void beep(){
  digitalWrite(Buzzer, HIGH);
  delay(400);
  digitalWrite(Buzzer, LOW);
  delay(800);
}

void GAlarm(){
      
      int gas = analogRead(Gas);
      gas = map(gas, 0, 1024, 0, 100);
    if (gas <= 50) {
     digitalWrite(Buzzer, LOW);
    } 
    else if (gas > 50) {
     Blynk.logEvent("gas_leak_detected","Warning! Gas leak detected .Check your Cooktop immediately");
    //digitalWrite(Buzzer, HIGH);
    beep();
    servo1.write(180);
    }
   else {
    digitalWrite(Buzzer, LOW);
    
  }
}

void loop() {
  Blynk.run();
  timer.run();

}
  