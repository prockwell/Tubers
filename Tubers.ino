// TUBER CONTROL MODULE
#include "AccelStepper.h"
#include "Arduino.h"

#define DIR_PIN 8
#define STEP_PIN 9
#define BUTTON_PIN 10
#define TOP_REED_SWITCH_PIN 4
#define BOTTOM_REED_SWITCH_PIN 3
#define LED_PIN 13
#define HIDE_BUTTON_PIN 12

#define UPWARDS true
#define DOWNWARDS false

// Define stepper with pins
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

//STEPPER CONTROL
const float MAX_SPEED = 10000.0;
const float DOWN_SPEED = 4000.0;
const float UP_SPEED = -100.0;
boolean currentDir = UPWARDS; //true = upwards, false = downward
boolean active = true;

//STEPPER JITTER
const float JITTER_DOWN_SPEED = 10000.0;
const float JITTER_UP_SPEED = -JITTER_DOWN_SPEED;
const int JITTER_MAX_DIRECTION_TIME = 20;
int jitterElapsedTime = 0;
boolean jitterDir = DOWNWARDS;
boolean jitter = false;             //set true when the tube has reached the apex of its movement and begins jittering in place

//BUTTON CONTROL
boolean buttonState = true;     // the current state of the output pin
int reading;           			// the current reading from the input pin
int previous = LOW;   			// the previous reading from the input pin
long time = 0;         			// the last time the output pin was toggled
const long debounce = 200;   	// the debounce time, increase if the output flickers

//HIDE BUTTON CONTROL
boolean hideButtonState = true;
int hideReading;
int hidePrevious = LOW;
long hideTime = 0;

//REED SWITCH CONTROL
int topReedReading;
int bottomReedReading;

//Functions
void moveTube(boolean direction);
void stopTube();
void checkButtonState();
void checkHideButtonState();
void checkTopReedSwitch();
void checkBottomReedSwitch();
void startJitter();

void setup()
{
	pinMode(BUTTON_PIN, OUTPUT);
    pinMode(HIDE_BUTTON_PIN, OUTPUT);
	pinMode(TOP_REED_SWITCH_PIN, INPUT);
	pinMode(BOTTOM_REED_SWITCH_PIN, INPUT);
	digitalWrite(BUTTON_PIN, HIGH); //used for pull-up button control.
    digitalWrite(HIDE_BUTTON_PIN, HIGH);
	
    //Sets max speed - must be a positive value
    stepper.setMaxSpeed(MAX_SPEED);
    
	//Begin moving tube upwards
    moveTube(currentDir);
    
	Serial.begin(9600);
   	Serial.println("--- SERIAL MONITOR ONLINE --- \n");
}

void loop()
{
	checkButtonState();
    checkHideButtonState();
	checkTopReedSwitch();
	checkBottomReedSwitch();
	
	if (active == true)
	{
		stepper.runSpeed();
	}
    else if (jitter == true)
    {
        Serial.print("JITTER:");
        Serial.println(jitterElapsedTime);
        //if the max time in a direction has been reached then reverse the direction
        if(jitterElapsedTime > JITTER_MAX_DIRECTION_TIME)
        {
            jitterDir = !jitterDir;
            Serial.println("JITTER: Change direction");
            
            //reverse movement direction
            if(jitterDir == UPWARDS)
            {
                stepper.setSpeed(JITTER_UP_SPEED);
            }
            else
            {
                stepper.setSpeed(JITTER_DOWN_SPEED);
            }
            
            //reset the elapsed time
            jitterElapsedTime = 0;
        }
        
        stepper.runSpeed();
        jitterElapsedTime++;
    }
}

void checkTopReedSwitch()
{
	//if direction is already downwards then return immediately
	if(currentDir == DOWNWARDS || jitter)
	{
		return;
	}
	
	topReedReading = digitalRead(TOP_REED_SWITCH_PIN);
	
	if (topReedReading == HIGH)
	{
		Serial.println("-- TOP REED SWITCH TRIGGERED --");
		digitalWrite(LED_PIN, HIGH);
		
		//Stop and hold tube
		stopTube();
	}
	else
	{
		digitalWrite(LED_PIN,LOW);
	}
}

void checkBottomReedSwitch()
{
	//if direction is already upwards then return immediately
	if(currentDir == UPWARDS)
	{
		return;
	}
	
	bottomReedReading = digitalRead(BOTTOM_REED_SWITCH_PIN);
	
	if (bottomReedReading == HIGH)
	{
		Serial.println("-- BOTTOM REED SWITCH TRIGGERED --");
		digitalWrite(LED_PIN, HIGH);
		
		//Turn direction up.
		moveTube(UPWARDS);
	}
	else
	{
		digitalWrite(LED_PIN, LOW);
	}
}

//Returns the state of the toggle button
void checkButtonState()
{
	reading = digitalRead(BUTTON_PIN);
	
	// if the input just went from LOW and HIGH and we've waited long enough
	// to ignore any noise on the circuit, toggle the output pin and remember
	// the time
	if (reading == HIGH && previous == LOW && millis() - time > debounce)
	{
		if (buttonState == true)
		{
			buttonState = false;
			
			//set direction up and begin moving tube
			moveTube(UPWARDS);
		}
	    else
		{
			buttonState = true;
            active = false;
		}
		
		Serial.print("BUTTON PRESSED - state set to: ");
        Serial.println(buttonState);
        
	    time = millis();
    }
	
	previous = reading;
}

void checkHideButtonState()
{
    //if already going downwards then return immediately
    if (currentDir == DOWNWARDS)
    {
        return;
    }
    
    hideReading = digitalRead(HIDE_BUTTON_PIN);
    
    if (hideReading == LOW && hidePrevious == HIGH && millis() - hideTime > debounce)
	{
        moveTube(DOWNWARDS);
		
		Serial.println("HIDE BUTTON PRESSED - moving downwards");
        
	    hideTime = millis();
    }
	
	hidePrevious = hideReading;
}

// Moves the inner tube upwards (dir == true) or downwards (dir == false)
void moveTube(boolean direction)
{
	currentDir = direction;
    active = true;
    jitter = false;
	
	float speed;
    
	if (currentDir == true)
	{
		//UPWARDS
		speed = UP_SPEED;
	}
	else
	{
		//DOWNWARDS
		speed = DOWN_SPEED;
	}
    
	stepper.setSpeed(speed);
	
	Serial.print("Move Tube in direction: ");
	Serial.println(currentDir);
}

// Stops the tube movement immediately
void stopTube()
{
    active = false;
    jitter = true;
    Serial.println("Stop Tube.");
    
    //start jittering the tube
    startJitter();
}

// Begins jittering the tube in place when it has reached its apex
void startJitter()
{
    //if jittering is already active then return immediately
    if(jitter)
    {
        return;
    }
    
    Serial.println("Start Jittering.");
    
    jitter = true;
    
    //set jitter properties
    jitterDir = DOWNWARDS;
    stepper.setSpeed(JITTER_DOWN_SPEED);
    jitterElapsedTime = 0;
}


