// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo tsidud;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
Servo angle;
 Servo trigger;
 
int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  tsidud.attach(9);  // attaches the servo on pin 9 to the servo object 
  angle.attach(5);
  trigger.attach(10);
  
  Serial.begin(9600);
} 
 
 
void loop() 
{ 
  //tsidud.write(0);
  //angle.write(0);
  //delay(150);
  //tsidud.write(0);
  //angle.write(0);
  //delay(150);
  //for(pos = 0; pos < 100; pos += 1)  // goes from 0 degrees to 180 degrees 
  //{                                  // in steps of 1 degree 
    while (Serial.available() > 0) {
      //delay(1000);
      int tsidudAngle = Serial.parseInt();
      int angleAngle = Serial.parseInt();
      int fire = Serial.parseInt();
      //Serial.println(tsidudAngle);
      //Serial.println(angleAngle);
      //Serial.println(fire);
      
      if (Serial.read() == '\n') {
        
        tsidud.write(tsidudAngle);
        delay(5);
        angle.write(angleAngle);
        delay(5);
        if (fire == 0) {
          trigger.write(30);
          delay(1000);
          trigger.write(90);
          delay(500);
        }
      }
    }
 //   tsidud.write(pos);              // tell servo to go to position in variable 'pos' 
 //   angle.write(pos);
 //   delay(15);                       // waits 15ms for the servo to reach the position 
 // } 
 // for(pos = 100; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
 // {                                
 //   tsidud.write(pos);              // tell servo to go to position in variable 'pos' 
 //   angle.write(pos);
 //   delay(15);                       // waits 15ms for the servo to reach the position 
 // } 
} 

void setZeroTsidud()
{
  tsidud.write(0);
}

void setZeroAngle()
{
    angle.write(0);
}

void fire()
{
  trigger.write(100);
  delay(1000);
  trigger.write(0);
}


