#define BLYNK_TEMPLATE_ID "TMPL3bo9wyLgD"
#define BLYNK_TEMPLATE_NAME "IOT OVEN"
#define BLYNK_AUTH_TOKEN "0YtZQvvVgyMM96AYeJuxKHAqVSOrBPpK"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
//#include <SevenSegmentFun.h>

char auth[] = "0YtZQvvVgyMM96AYeJuxKHAqVSOrBPpK";// Enter your Auth token
char ssid[] = "Androidx";//Enter your WIFI SSIS
char pass[] = "1234qwert";//Enter your WIFI password

int Dflag=0; // flag for Utensil detection
int Cflag=0; // flag for enabling whistle count function
int whcount = 0; // whistle count variable
int Walarmflag= 3; // variable to trigger whistle alarm
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
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  display.begin(); // initializes the display
  display.setBacklight(100); // set the brightness to 100 %
  servo1.write(180); // set servo to visual 0 indicator
  timer.setInterval(100L, Sensorsend);
  timer.setInterval(100L, Detect);
  timer.setInterval(100L, GAlarm);
  timer.setInterval(100L, Count);
  timer.setInterval(100L, WAlarm);
  //timer.setInterval(100L, disp);
}

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
     Blynk.logEvent("gas_leak_detected","Warning!leake Gas detected .Please Check your Cooktop");
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
  