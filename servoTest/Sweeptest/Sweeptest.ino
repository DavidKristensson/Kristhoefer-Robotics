/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myLEFTservo;  // create servo object to control a servo
Servo myRIGHTservo;
// twelve servo objects can be created on most boards

int Xin = A0; // X Input Pin
int Yin = A1; // Y Input Pin

int xVal;
int yVal;

int posRIGHTservo = 0;    // variable to store the rightservo position

int posLEFTservo = 0;    // variable to store the leftservo position

#define LOW_END_SERVO_VALUE 
#define HIGH_END_SERVO_VALUE 

void setup() {
  myLEFTservo.attach(10);  // attaches the servo on pin 9 to the servo object
  myRIGHTservo.attach(3);

  Serial.begin(9600);
}

void loop() {
    //Serial.println(myLEFTservo.read());

     xVal = analogRead (Xin);






    
    //Serial.print("Left servo value is: ");
    //Serial.println (myLEFTservo.read());
    //delay(30);
    


    
    //myLEFTservo.write(mappedJoystickValue);

    // LEFT SERVO TEST
    yVal = analogRead (Yin);
    byte mappedJoystickValue = map(analogRead(Yin), 0, 1023, 60, 160);
    Serial.print ("Y = ");
    Serial.println (yVal, DEC);

    analogSpakTest(mappedJoystickValue, myLEFTservo.read());
    /*
    // high to low
    for (posLEFTservo = 160; posLEFTservo >= 60; posLEFTservo -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(posLEFTservo);              // tell servo to go to position in variable 'pos'
    Serial.println(posLEFTservo);
    delay(15);                       // waits 15ms for the servo to reach the position   
    }
    
    //low to high
    for (pos = 60; pos <= 160; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(posLEFTservo);              // tell servo to go to position in variable 'pos'
    Serial.println(posLEFTservo);
    delay(15);                       // waits 15ms for the servo to reach the position   
    }
    */



    
    /*Serial.print("X = ");
    Serial.println (xVal, DEC);
    */
    
    
/*
    Serial.print ("Y = ");
    Serial.println (yVal, DEC);

*/
    //Serial.println(posRIGHTservo);

   //myservo.write(150);
    // low to high
    /*

  */


  
}

void analogSpakTest(int mappedAnalogStickValue, int previousServoValue){

    if(mappedAnalogStickValue > previousServoValue){
      for(int i = previousServoValue; i < mappedAnalogStickValue; i++){
        myLEFTservo.write(i);
        delay(5);
      }
    } 
    
    else if(mappedAnalogStickValue < previousServoValue){
      for(int i = previousServoValue; i > mappedAnalogStickValue; i--){
        myLEFTservo.write(i);
        delay(5);
      }
    }  
}
