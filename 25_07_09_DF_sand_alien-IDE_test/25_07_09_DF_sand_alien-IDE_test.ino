// 7/14/25 notes coppied from birds/devil
//program RX cyle time is 3.15 sec, of which 3 sec is RX
//based on this, should be able to delete added 2 sec of 
//RX extension (TX is 4.5 sec) - drops code occasionally
//doesn't work! try extending by one second, not two - works
//plan - change other two heads


// 7/9/25 notes 
//this was downloaded to head from IDE 2.3.6 - new IDE works on heads
//next, shorten xmit time for "111' by making function "xmit3"
//done, not tested - also deleted code at end which was commented out
//test with all three heads - works
//next, delete old notes, unused code

 
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

//new for ultrasonic-----------------------
#define trigger 12 
#define echo 13
int distance = 0;
float time = 0;
int dist = 100;  //set for 30cm
int msg_dly = 3000;

int u = 1; // for counting the ultrasonic sensor loop
int w = 1; //for counting the 010 loop
int s = 1;  //for counting the 011 loop
int p = 1;  //for countng the 101 loop
int r = 1;  //for counting the 110 loop
int b = 1;   //for counting the 001 loop
int c = 1;   //for counting the 111 loop

//new for pulse generator---------------------------
#define MOUTH 5  //PWM for mouth servo
int val = 0;   // not currently used

int val0 = 0; // from analog pin 0
int value = 0; //value from analog pin 1

int i=0;  //number of pulses in servo drive, and set xmit times
int k=0;  //duration of audio sample for servo drive
int dur =0; //length of servo pulses 

int pulseMax = 2300;
int pulseMin = 1200;
int pulseLength = 1200;
//int pulseCen = 2200;  //for test
int pulseCen = 1200;  //from 1350 4/3

int mult = 20;
float expt = .50;

//new for radio receover functions
#define D2 2
#define D1 3
#define D0 4

#define T2 7  // transmitter functions
#define T1 8    
#define T0 9

int vx =0; // digital read D0
int vy=0; // digital read D1
int vz=0; //digital read D2

int VX=0;  //detected value of DO
int VY=0;
int VZ=0;



//_____________________________________________________ 5 pulse generator
 void pulseGen(int pulseLength) {
//pulsetrain with calcuated pulseLength
for (i= 0; i < 5;i++) { 
digitalWrite(MOUTH, HIGH); //start servo pulse
delayMicroseconds(pulseLength);
digitalWrite(MOUTH, LOW);
delay(20);
}
}

//____________________________________________________ center the servo
void center(int pulseCen)  {
 for (i= 0; i < 10; i++) { 
digitalWrite(MOUTH, HIGH); //start servo pulse
delayMicroseconds(pulseCen);
digitalWrite(MOUTH, LOW);
delay(20);  
} 
}

//___________________________________________________ generate servo drive
void servo(int val0 , int dur) {
//________________ __________________pulse generator (adjust k for audio length
for (k=0; k<dur; k++) {

val0 = analogRead(0);  //calculate pulseLength and limit its range
pulseLength = pulseMin+ mult*pow(val0, expt);
 
if (pulseLength< pulseMin) pulseLength = pulseMin;
if (pulseLength> pulseMax) pulseLength = pulseMax;

pulseGen(pulseLength); // 5 pulse generator
}
 
center(pulseCen);  // center the servo
}
// ______________________________________________end servo drive



//_____________________________________ transmitter control one digit
void transmit(int VX, int VY, int VZ) {
for (i=0; i<10; i++) {
if (VX==HIGH) {
digitalWrite(T0, LOW);
delay(50);
digitalWrite(T0, HIGH);
delay(400);  
}
if(VY==HIGH) {
digitalWrite(T1, LOW);
delay(50);
digitalWrite(T1, HIGH);
delay(400);  
}
if(VZ==HIGH) {
digitalWrite(T2, LOW);
delay(50);
digitalWrite(T2, HIGH);
delay(400);
}    //_______________________________end of transmitter control one digit
}    // xmit time= 10*(.05+.40)sec = 4.5 seconds
}


//_____________________________________ transmitter control two digits
void xmit2(int VX, int VY, int VZ) {
for (i=0; i<5; i++) {
if (VX==HIGH) {
digitalWrite(T0, LOW);
delay(50);
digitalWrite(T0, HIGH);
delay(400);  
}
if(VY==HIGH) {
digitalWrite(T1, LOW);
delay(50);
digitalWrite(T1, HIGH);
delay(400);  
}
if(VZ==HIGH) {
digitalWrite(T2, LOW);
delay(50);
digitalWrite(T2, HIGH);
delay(400);
}
}    //   xmit time= 5*2*(.05+.40)sec = 4.5 seconds
}    //________________________end of transmitter control two digits


//_____________________________________ transmitter control three digits
void xmit3(int VX, int VY, int VZ) {
for (i=0; i<3; i++) {
if (VX==HIGH) {
digitalWrite(T0, LOW);
delay(50);
digitalWrite(T0, HIGH);
delay(450);  
}
if(VY==HIGH) {
digitalWrite(T1, LOW);
delay(50);
digitalWrite(T1, HIGH);
delay(450);  
}
if(VZ==HIGH) {
digitalWrite(T2, LOW);
delay(50);
digitalWrite(T2, HIGH);
delay(450);
}
}    //  xmit time= 3*3*(.05+.45)sec = 4.5 seconds
}    //________________________end of transmitter control three digits




void setup()   //___________________________________________________________________
{
pinMode(trigger,OUTPUT);  // ultrasonic sensor
pinMode(echo, INPUT);
pinMode(MOUTH,OUTPUT);  // servo output for mouth

pinMode(D2, INPUT);  //radio 
pinMode(D1, INPUT);
pinMode(D0, INPUT);

pinMode(T2, OUTPUT);
pinMode(T1, OUTPUT);
pinMode(T0, OUTPUT);
  
mySoftwareSerial.begin(9600);
Serial.begin(115200);

Serial.println();
Serial.println(F("Giant Heads"));
Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
Serial.println(F("Unable to begin:"));
Serial.println(F("1.Please recheck the connection!"));
Serial.println(F("2.Please insert the SD card!"));
while(true);
 }
Serial.println(F("DFPlayer Mini online."));

myDFPlayer.volume(30);  //Set volume value. From 0 to 30

digitalWrite(T0, HIGH);  //this may cause problems at start-up
digitalWrite(T1, HIGH);
digitalWrite(T2, HIGH);

//added 7/7/25
myDFPlayer.playMp3Folder(5);  //_____________ did you say something, I thought I
dur=50;
servo(val0, dur);
myDFPlayer.pause();

u=81;
w=1;
s=1;
p=1;
r=1;
}





void loop()    //______________________________________________________________
{
digitalWrite(T0, HIGH);
digitalWrite(T1, HIGH);
digitalWrite(T2, HIGH);

//comment out 7/8/25
/*
Serial.println();
Serial.print(" start Sandman");
Serial.println();
delay(100); 
*/


val0 = analogRead(0);  //analog voltage from pin "0"

//comment out 7/8/25
/*
Serial.println();
Serial.print("val0  ");
Serial.print(val0);
Serial.println();
delay(100);
*/


//____________________________________________________________ ultrasonic loop start

digitalWrite(trigger,LOW);  // ___________ ultrasonic sensor and calculate distance
delayMicroseconds(2);
digitalWrite(trigger,HIGH);
delayMicroseconds(10);
digitalWrite(trigger,LOW);
delayMicroseconds(2);
 
time=pulseIn(echo,HIGH); //inherent delay is 10 - 100ms only
distance=time*340/20000;


if (u >84) {u=81;}  //__________________________________segment "u" player 
if (distance < dist) { 
myDFPlayer.volume(30);
myDFPlayer.playMp3Folder(u);   //Sandman
if (u==81) {dur=425;}  //around 45 seconds 
if (u==82) {dur=230;}
if (u==83) {dur=300;}
if (u==84) {dur=402;}

servo(val0, dur);
myDFPlayer.pause();


if (u==81) {  /////_________________________________ segment "u" transmit
 VZ=LOW; VY=LOW; VX=HIGH;
transmit(VX,VY,VZ);   
}

if (u==82) {
VZ=HIGH; VY=LOW; VX=LOW;
transmit(VX,VY,VZ);   
}

if (u==83) {
 VZ=LOW; VY=LOW; VX=HIGH;
transmit(VX,VY,VZ);   
}

if (u==84) {
 VZ=HIGH; VY=LOW; VX=LOW;
transmit(VX,VY,VZ);   
}
u=u+1;
}

 Serial.println();     //monitor "U" loop
Serial.print(u);
Serial.print(" u");
Serial.println();
   //_____________________________________________end ultrasonic loop

delay(20);
myDFPlayer.volume(30);


//_____________________________________________________________RF signal detector 
//samples and saves any HIGH states

VX=LOW;
VY=LOW;
VZ=LOW;

for (i=0; i<300; i++) {    //receive time = 300*.01= 3 seconds
vx= digitalRead(D0);
vy=digitalRead(D1);
vz= digitalRead(D2);

delay(10);

if (vx==HIGH)  { VX=HIGH;  }
if (vy==HIGH)  { VY=HIGH;  }
if (vz==HIGH)  { VZ=HIGH;  }
}  //________________________________________________ end of RF signal detect part 1



// 7/15/25 reduce time from 2 sec to 1 sec

//________________________________________extend detect time 2 sec if signal is detected
if (VX==HIGH||VY==HIGH||VZ==HIGH)  {

for (i=0; i<100; i++) {   
vx= digitalRead(D0);
vy=digitalRead(D1);
vz= digitalRead(D2);

delay(10);

if (vx==HIGH)  { VX=HIGH;  }
if (vy==HIGH)  { VY=HIGH;  }
if (vz==HIGH)  { VZ=HIGH;  }
}  //_______________________________________________________ end of RF signal detect part 2
}



if(VZ==LOW&&VY==LOW&&VX==HIGH) {   //_______________________________ 0 0 1 code detect
if (b>2) {b=1;}

if(b==1){
myDFPlayer.playMp3Folder(5);  //_____________ did you say something, I thought I
dur=60;
servo(val0, dur);
myDFPlayer.pause();
}

if(b==2){
myDFPlayer.playMp3Folder(21);  //______Do we have to hear about the dog dream again?
dur=50;
servo(val0, dur);
myDFPlayer.pause();
} 

b=b+1;

VZ=LOW; VY=HIGH; VX=LOW;
transmit(VX,VY,VZ);
VZ=LOW; VY=LOW; VX=HIGH;
}



if(VZ==LOW&&VY==HIGH&&VX==LOW) {   //_______________________________ 0 1 0 code detect
if (w>2) {w=1;}

if(w==1) {
myDFPlayer.volume(30);  
myDFPlayer.playMp3Folder(7); //_________________  I didn't hear anything
dur=50;
servo(val0, dur);
myDFPlayer.pause();
}

if(w==2) {
myDFPlayer.volume(30);  
myDFPlayer.playMp3Folder(8);//__________________ not me, I didn't hear anything
dur=50;
servo(val0, dur);
myDFPlayer.volume(20);
myDFPlayer.pause();
}

Serial.println();     //monitor "w" loop
Serial.print(w);
Serial.print(" w");
Serial.println();

w= w+1;

VZ=LOW; VY=HIGH; VX=HIGH;
xmit2(VX,VY,VZ);
VZ=LOW; VY=HIGH; VX=LOW;
}



if(VZ==LOW&&VY==HIGH&&VX==HIGH) {   //____________________________ 0 1 1 code detect "s" loop
if (s>2) {s=1;}

if(s==1) {
myDFPlayer.playMp3Folder(25); //________________I didn't hear anything, but did you hear this?
dur=50;
servo(val0, dur);
myDFPlayer.pause();

myDFPlayer.playMp3Folder(84); //________________ Never on Sunday
dur=50;
servo(val0, dur);
myDFPlayer.pause();
}

if(s==2) {
myDFPlayer.playMp3Folder(26); //___________Are you inspired by chordettes?
dur=50;
servo(val0, dur);
myDFPlayer.pause();

myDFPlayer.playMp3Folder(18); //_________ crow
dur=50;
servo(val0, dur);
myDFPlayer.pause();
}

Serial.println();     //monitor "s" loop
Serial.print(s);
Serial.print(" s");
Serial.println();

s= s+1;
}



if(VZ==HIGH&&VY==LOW&&VX==LOW) {   //_______________________________ 1 0 0 code detect
myDFPlayer.volume(30); //added 7/3/25 probably not needed
myDFPlayer.playMp3Folder(6); //__________________ Radio head
dur=95;  //from 80 4/3
servo(val0, dur);
myDFPlayer.pause();

VZ=HIGH; VY=LOW; VX=HIGH;
xmit2(VX,VY,VZ);
VZ=HIGH; VY=LOW; VX=LOW;
}


if(VZ==HIGH&&VY==LOW&&VX==HIGH) {   //_________________ 1 0 1 code detect   "p" loop
if (p>2) {p=1;}

if(p==1) {
myDFPlayer.playMp3Folder(11); //__________________ that's not funny 
dur=20;
servo(val0, dur);
myDFPlayer.pause();
}

if(p==2) {
myDFPlayer.playMp3Folder(12); //__________________ you think that's funny, it's not
dur=30;
servo(val0, dur);
myDFPlayer.pause();
}

Serial.println();     //monitor "p" loop
Serial.print(p);
Serial.print(" p");
Serial.println();

p= p+1;

VZ=HIGH; VY=HIGH; VX=LOW;
xmit2(VX,VY,VZ);
VZ=HIGH; VY=LOW; VX=HIGH;
}



if(VZ==HIGH&&VY==HIGH&&VX==LOW) {   //____________________________  1 1 0 code detect "r" loop

if (r>2) {r=1;}

if(r==1) {
myDFPlayer.playMp3Folder(13); //______________you think its funny when you say it
dur=40;
servo(val0, dur);
myDFPlayer.pause();
}

if(r==2) {
myDFPlayer.playMp3Folder(14); //_______________ its a little funny
dur=40;
servo(val0, dur);
myDFPlayer.pause();
}

Serial.println();     //monitor "r" loop
Serial.print(r);
Serial.print(" r");
Serial.println();

r= r+1;

VZ=HIGH; VY=HIGH; VX=HIGH;
xmit3(VX,VY,VZ);
VZ=HIGH; VY=HIGH; VX=LOW;
}



if(VZ==HIGH&&VY==HIGH&&VX==HIGH) {   //___________________ 1 1 1 code detect "c" loop

if (c>2) {c=1;}

if(c==1) {
myDFPlayer.volume(30);   
myDFPlayer.playMp3Folder(27); //______________the life you lead can be your own
dur=40;
servo(val0, dur);
myDFPlayer.volume(30); 
myDFPlayer.pause();
}

if(c==2) {
myDFPlayer.playMp3Folder(28); //_________I'm only a talking head, but I can tell..realistic
dur=70;
servo(val0, dur);
myDFPlayer.pause();
}


c= c+1;

}

//give something to sync 7/8/25
//digitalWrite(trigger,HIGH);
//delay(50);
//digitalWrite(trigger,LOW);


//change form 300 on 7/8/25
delay(100);
 
}
