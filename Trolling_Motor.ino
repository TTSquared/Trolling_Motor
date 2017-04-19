/* 
Trolling Motor Speed Controller
Terry W. Towle - April 2017

License GNU GPL version 3

Program allows you to control the speed of an outboard motor through a dial. In addition there are various
buttons that will run through a number of programs that vary the speed.

Full speed - Moves the throttle to 90% of 70 degrees for as long as you hold the button down to allow you
to control the boat if the wind picks up or if you need your lines and lures to raise up to go over an
obstacle. The led on the button is lit, letting you know the program is running. When the button is 
released, the throttle returns to the original setting on the dial and the led is turned off.

Idle speed - Moves the throttle to 5% throttle to allow you just enough forward speed to maintain control
and not sink your lines while fighting a fish. The led on the button is lit, letting you know the 
program is running. When the button is pushed again, the speed goes back to what was originally set on the 
dial and the led is turned off.

Surge program - When the button is pushed, after 50 seconds, the speed is increased by 10% for 10 seconds.
After 10 seconds, the speed returns to normal. The led on the button is lit, letting you know the program
is running. Pushing the button a second time, the throttle returns to the original setting on the dial and
the led is turned off. 

Hunt program - When the button is pushed, the throttle up from the base setting 3%, 3%, 2% and then down 3%, 3%,
2% each minute to cover multiple speed ranges to see what speed is working. The led on the button is lit,
letting you know the program is running. When the button is pushed a second time, the throttle returns to the 
original setting on the dial and the led is turned off.

Delay program - When the button is pushed, the speed is decreased by 7%. The led on the button is lit, letting 
you know the program is running. Pushing the button a second time, the throttle returns to the 
original setting on the dial and the led is turned off. 
*/

#include <Servo.h>          //include the servo library
Servo myservo;              // create servo object to control a servo

int potpin = 0;                     // analog pin 0 used to connect the potentiometer
int pinval = 0;                     // variable to read the value from the analog pin
int origpinval = 0;                 // to store orginal pin value
int FBState = 0;                    // variable for reading the Full pushbutton status
int IBState = 0;                    // variable for reading the Idle pushbutton status
int HBState = 0;                    // variable for reading the Hunt pushbutton status
int SBState = 0;                    // variable for reading the Surge pushbutton status
int DBState = 0;                    // variable for reading the Delay pushbutton status
unsigned long previousMillis = 0;   // will store last time program was updated

// modifying these changes timing intervals. An unsigned integer can't be larger than 65,535 or 1 min and 5.54 secs.
unsigned int HInterval = 60000;      // interval at which to change speed in Hunt mode (milliseconds)
unsigned int SInterval1 = 45000;     // interval to run Surge program at normal speed
unsigned int SInterval2 = 55000;     // SInterval2 - SInterval1 gives interval to run Surge at fast speed
unsigned int DInterval1 = 45000;     // interval to run Delay program at normal speed
unsigned int DInterval2 = 55000;     // DInterval2 - DInterval1 gives interval to run Delay at slow speed

const int Fled = 4;         //Full speed button led
const int Iled = 5;         //Idle speed button led
const int Sled = 6;         //Surge progam button led
const int Hled = 7;         //Hunt program button led
const int Dled = 8;         //Delay program button led
const int FButton = 9;      //Full speed button 
const int IButton = 10;     //Idle speed button
const int SButton = 11;     //Surge progam button
const int HButton = 12;     //Hunt program button
const int DButton = 13;     //Delay program button

const int FullPin = 65;     //Full speed setting. (93% of 70 degrees) ** Modify to change amount of throttle **
const int IdlePin = 1;      //Idle speed setting. (1% of 70 degrees) ** Modify to change amount of throttle **
const int Hunt1Pin = 3;     //Hunt 1st speed setting. (4% of 70 degrees) ** Modify to change amount of throttle **
const int Hunt2Pin = 2;     //Hunt 2nd speed setting. (3% of 70 degrees) ** Modify to change amount of throttle **
const int Hunt3Pin = 2;     //Hunt 3rd speed setting. (3% of 70 degrees) ** Modify to change amount of throttle **
const int SurgePin = 4;     //Surge speed setting. (6% of 70 degrees) ** Modify to change amount of throttle **
const int DelayPin = 4;     //Delay speed setting. (6% of 70 degrees) ** Modify to change amount of throttle **


//*********************************************************************************************
void setup()
{
myservo.attach(3);            // attaches the servo on pin 3 to the servo object
myservo.write(70 - pinval);   // sets the initial servo position to zero or full idle (reversed servo)

pinMode(Fled, OUTPUT);        // set the pins for the button leds to outputs
pinMode(Iled, OUTPUT);
pinMode(Sled, OUTPUT);
pinMode(Hled, OUTPUT);
pinMode(Dled, OUTPUT);
pinMode(FButton, INPUT);      // set the pins for the buttons to inputs
pinMode(IButton, INPUT);
pinMode(SButton, INPUT);
pinMode(HButton, INPUT);
pinMode(DButton, INPUT);

// For safety reasons, does not run the program until the dial is turned to idle. (<=2)
pinval = analogRead(potpin);                // reads the value of the potentiometer (value between 0 and 1023)
 // scale it to use it with the servo (value between 0 and 70). Servos usually go from 0 to 180 degrees, this is 
 // set to 70 degrees so it doesn't go too far and bind the throttle linkage.
pinval = map(pinval, 0, 1023, 70, 0);       
  while (pinval > 2) {
    pinval = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
    pinval = map(pinval, 0, 1023, 70, 0);   // scale it to use it with the servo (value between 0 and 70)
  }  //end while
}    //end setup
//**************************************************************************************************************

void loop()
{

  pinval = analogRead(potpin);              // reads the value of the potentiometer (value between 0 and 1023)
  pinval = map(pinval, 0, 1023, 70, 0);     // scale it to use it with the servo (value between 0 and 70)
  pinval = 70 - pinval;                     // servo direction reversed. This done so dial turns clockwise for more power
  myservo.write(pinval);                    // sets the servo position according to the scaled value
  delay(15);                                // waits for the servo to get there

  FBState = digitalRead(FButton);     // read the state of the Full pushbutton value
  if (FBState == HIGH) {            
    delay(250);
    Full();                           // go to Full function
    
  } 
  digitalWrite(Fled, LOW);            // turn off button led

  IBState = digitalRead(IButton);     // read the state of the Idle pushbutton value
  
  if (IBState == HIGH) {              // run the Idle speed function
    delay(250);                       // to debounce button
    Idle();                           // go to Idle function
  } 

  HBState = digitalRead(HButton);     // read the state of the Idle pushbutton value
  
  if (HBState == HIGH) {              // run the Idle speed function
    delay(250);                       // to debounce button
    Hunt();                           // go to Hunt function
  } 

  SBState = digitalRead(SButton);     // read the state of the Surge pushbutton value
  
  if (SBState == HIGH) {              // run the Surge speed function
    delay(250);                       // to debounce button
    Surge();                          // go to Surge function
  } 

  DBState = digitalRead(DButton);     // read the state of the Delay pushbutton value
  
  if (DBState == HIGH) {              // run the Delay speed function
    delay(250);                       // to debounce button
    Delay();                          // go to Delay funtion
  }

}  // end loop
//****************************************************************************************************

void Full() {
  digitalWrite(Fled, HIGH);           // turn on button led
//  origpinval = pinval;                // store original pin value
  while (FBState == HIGH) {
  pinval = 70 - FullPin;              // servo mapped from 0 to 70 degrees. This reverses servo direction.
  myservo.write(pinval);
  FBState = digitalRead(FButton);     // read the state of the Full pushbutton value
  }
//  pinval = origpinval;
//  myservo.write(pinval);              // write the original speed to the servo
  digitalWrite(Fled, LOW);            // turn off button led
  delay(250);
} 

void Idle() {
  
 IBState = 0;                         // set button state to low after pushed
 digitalWrite(Iled, HIGH);            // turn on button led
// origpinval = pinval;                 // store original pin value
 while (IBState == LOW)  {            // run the Idle speed function until the Idle button is pushed again
 pinval = 70 - IdlePin;               // decrease speed. servo mapped from 0 to 70 degrees. This reverses servo direction.
 myservo.write(pinval);
 
 IBState = digitalRead(IButton);      // read the state of the Idle pushbutton value
 }
// pinval = origpinval;
// myservo.write(pinval);               // write the original speed to the servo
 digitalWrite(Iled, LOW);             // turn off button led
 delay(250);                          // to debounce button 
}

void Hunt() {

 unsigned long currentMillis;          // to track time
 int change = 1;                       // to track the various speed changes so previous IF statements can be skipped

 digitalWrite(Hled, HIGH);             // turn on button led
 previousMillis = millis();
 HBState = 0;                          // set button state to low after pushed
 origpinval = pinval;                  // store original pin value

 while (HBState == LOW)  {             // run the Hunt speed function until the Hunt button is pushed again
 currentMillis = millis();             // save the current time

 if (currentMillis - previousMillis >= (HInterval) && change == 1) {
    pinval = pinval - Hunt1Pin;        // increase speed 
    myservo.write(pinval);             // write the new speed to the servo
    change = 2;                        // change variable so this IF statement is skipped when checking time
    previousMillis = millis(); 
    
 }
  if (currentMillis - previousMillis >= (HInterval)&& change == 2) {
    pinval = pinval - Hunt2Pin;        // increase speed 
    myservo.write(pinval);             // write the new speed to the servo
    change = 3;                        // change variable so this IF statement is skipped when checking time
    previousMillis = millis(); 
    
 }
   if (currentMillis - previousMillis >= (HInterval) && change == 3) {
    pinval = pinval - Hunt3Pin;        // increase speed 
    myservo.write(pinval);             // write the new speed to the servo
    change = 4;                        // change variable so this IF statement is skipped when checking time 
    previousMillis = millis(); 
 }
   if (currentMillis - previousMillis >= (HInterval) && change == 4) {
    pinval = pinval + Hunt1Pin;        // decrease speed 
    myservo.write(pinval);             // write the new speed to the servo
    change = 5;                        // change variable so this IF statement is skipped when checking time
    previousMillis = millis(); 
 }
   if (currentMillis - previousMillis >= (HInterval) && change == 5) {
    pinval = pinval + Hunt2Pin;        // decrease speed 
    myservo.write(pinval);             // write the new speed to the servo
    change = 6;                        // change variable so this IF statement is skipped when checking time
    previousMillis = millis(); 
 }
   if (currentMillis - previousMillis >= (HInterval) && change == 6) {
    pinval = pinval + Hunt3Pin;       // decrease speed 
    myservo.write(pinval);            // write the new speed to the servo

    previousMillis = currentMillis;   // save the last time you changed the speed so can start again
    change = 1;                       // change variable back so first IF statement is not skipped
 }

 HBState = digitalRead(HButton);      // read the state of the Hunt pushbutton value
 }

 pinval = origpinval;
 myservo.write(pinval);               // write the original speed to the servo
 previousMillis = currentMillis;      // save the last time you changed the speed so can start again
 digitalWrite(Hled, LOW);             // turn off button led
 delay(250);                          // to debounce button
}

 void Surge() {

 unsigned long currentMillis;         // to track time
 int change = 1;                      // to track the various speed changes so previous IF statements can be skipped
 
 
 digitalWrite(Sled, HIGH);            // turn on button led
 
 SBState = 0;                         // set button state to low after pushed
 origpinval = pinval;                 // store original pin value for servo
 previousMillis = millis();
 
 while (SBState == LOW)  {            // run the Surge speed function until the Surge button is pushed again

 currentMillis = millis();            // save the current time

 if (currentMillis - previousMillis >= SInterval1 && change == 1) {     //run at original speed for 50 seconds
    pinval = pinval - SurgePin;       // increase speed
    myservo.write(pinval);            // write the new speed to the servo
    change = 2;                       // change variable so this IF statement is skipped when checking time
    
 }
  if (currentMillis - previousMillis >= SInterval2 && change == 2) {   // run at fast speed for 10 seconds
    pinval = pinval + SurgePin;       // decrease speed back to normal
    myservo.write(pinval);            // write the new speed to the servo
    change = 1;                       // change variable so this IF statement is skipped when checking time
    previousMillis = currentMillis;   // save the last time you changed the speed so can start again
 }

 SBState = digitalRead(SButton);      // read the state of the Surge pushbutton value
 }
 
 pinval = origpinval;
 myservo.write(pinval);               // write the original speed to the servo
 previousMillis = currentMillis;      // save the last time you changed the speed so can start again
 digitalWrite(Sled, LOW);             // turn off button led
 delay(250);                          // to debounce button

 }

 void Delay() {

 unsigned long currentMillis;         // to track time
 int change = 1;                      // to track the various speed changes so previous IF statements can be skipped
 
 digitalWrite(Dled, HIGH);            // turn on button led
 DBState = 0;                         // set button state to low after pushed
 origpinval = pinval;                 // store original pin value for servo
 previousMillis = millis();
 
 while (DBState == LOW)  {            // run the Delay speed function until the Delay button is pushed again
 currentMillis = millis();            // save the current time
 
 if (currentMillis - previousMillis >= DInterval1 && change == 1) {     //run at original speed for 50 seconds
    pinval = pinval + DelayPin;       // decrease speed
    myservo.write(pinval);            // write the new speed to the servo
    change = 2;                       // change variable so this IF statement is skipped when checking time
 }
  if (currentMillis - previousMillis >= DInterval2 && change == 2) {    // run at fast speed for 10 seconds
    pinval = pinval - DelayPin;       // increase speed back to normal
    myservo.write(pinval);            // write the new speed to the servo
    change = 1;                       // change variable back so first IF statement is not skipped
    previousMillis = currentMillis;   // save the last time you changed the speed so can start changes again
 }

 DBState = digitalRead(DButton);      // read the state of the Delay pushbutton value
 }  //end while
 
 pinval = origpinval;                 // write original pin value to pinval
 myservo.write(pinval);               // write the original speed to the servo
 previousMillis = currentMillis;      // save the last time you changed the speed so can start again NEEDED???
 digitalWrite(Dled, LOW);             // turn off button led
 delay(250);                          // to debounce button

 } //end Delay

  


