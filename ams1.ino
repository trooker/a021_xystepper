/*
 * 170421_tr Rebuild for Sloeber
 * 170330_tr Adjust for Adafruit_MotorShield  and Stepper
 * 
 */
//------------------------------------------------------------------------------
// 2 Axis CNC Demo
// dan@marginallycelver.com 2013-08-30
//------------------------------------------------------------------------------
// Copyright at end of file.
// please see http://www.github.com/MarginallyClever/GcodeCNCDemo for more information.

#if CONTROLLER == AMS1

//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
//#include <Adafruit_MotorShield.h>
//#include <AccelStepper.h>
#include "config.h"



//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------

Adafruit_MotorShield AFMStop(0x61); // Rightmost jumper closed
Adafruit_MotorShield AFMSbot(0x60); // Default address, no jumpers

Adafruit_StepperMotor *m1 = AFMSbot.getStepper(320, 1);   // radioshack 2730767 (5.625 degrees
Adafruit_StepperMotor *m2 = AFMSbot.getStepper(320, 2);   // HP Stepper (1.8 degree)
Adafruit_StepperMotor *m3 = AFMStop.getStepper(320, 1);   // HP Stepper (1.8 degree)


AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);
AccelStepper stepper3(forwardstep3, backwardstep3);



void forwardstep1() {  
 // Serial.println("Called forward1()");
  //showStepperMove();
  m1->onestep(FORWARD, SINGLE);
  kntX++;
}
void backwardstep1() {  
//  Serial.println("Called backward1()");
 // showStepperMove();
  m1->onestep(BACKWARD, SINGLE);
  kntX--;
}
// wrappers for the second motor!
void forwardstep2() {  
 // Serial.println("      Called forward2()");
 // showStepperMove();
   m2->onestep(FORWARD, SINGLE);
   kntY++;
  //m2->step(50,FORWARD,DOUBLE);
}
void backwardstep2() {  
 // Serial.println("m2->onestep(BACKWARD, SINGLE);");
//  Serial.println("      Called backward2()");
//  showStepperMove();
  m2->onestep(BACKWARD, SINGLE);
  kntY--;
 // m2->step(50,BACKWARD,DOUBLE);
}

// wrappers for the second motor!
void forwardstep3() {
 // Serial.println("      Called forward2()");
 // showStepperMove();
   m3->onestep(FORWARD, SINGLE);
   kntZ++;
  //m2->step(50,FORWARD,DOUBLE);
}
void backwardstep3() {
 // Serial.println("m2->onestep(BACKWARD, SINGLE);");
//  Serial.println("      Called backward2()");
//  showStepperMove();
  m3->onestep(BACKWARD, SINGLE);
  kntZ--;
 // m2->step(50,BACKWARD,DOUBLE);
}




void m1step(int dir) {
   if (dir > 0)
   {forwardstep1();}
 else
   {backwardstep1();}

  }

void m2step(int dir) {
  if (dir > 0)
   {forwardstep2();}
 else
   {backwardstep2();}


  //m2->step(10,dir,DOUBLE);
}


void m3step(int dir) {
  if (dir > 0)
   {forwardstep3();}
 else
   {backwardstep3();}
}


void disable()
{
    m1->release();
    m2->release();
    m3->release();
}

void zeroLoc()
{
  fp.x = 0.0;
  fp.y = 0.0;
  fp.z = 0.0;
  current_loc.x = 0;
  current_loc.y = 0;
  current_loc.z = 0;
}

void setup_controller()
{
     AFMSbot.begin(); // Start the top shield
     AFMStop.begin(); // Start the top shield

    stepper1.setMaxSpeed(200.0);
    stepper1.setAcceleration(100.0);
    stepper1.moveTo(24);

    stepper2.setMaxSpeed(300.0);
    stepper2.setAcceleration(100.0);
    stepper2.moveTo(1000000);


    stepper3.setMaxSpeed(300.0);
    stepper3.setAcceleration(100.0);
    stepper3.moveTo(1000000);
showDisplay("Setup_controller");
}

void run_steppers()
{
 // Serial.println("run_steppers1 & 2");

	//170709_tr  may need ot adjust this for zAxis
    if (stepper1.distanceToGo() == 0)
    stepper2.moveTo(-stepper2.currentPosition());
    if (stepper2.distanceToGo() == 0)
    stepper1.moveTo(-stepper1.currentPosition());
    if ((stepper2.distanceToGo() == 0) && (stepper1.distanceToGo() == 0))
    stepper3.moveTo(-stepper3.currentPosition());
    stepper1.run();
    stepper2.run();
    stepper3.run();
}

void init_steppers()
{
  //turn them off to start.
  disable();
  
  //init our points.
  current_units.x = 0.0;
  current_units.y = 0.0;
  current_units.z = 0.0;
  target_units.x = 0.0;
  target_units.y = 0.0;
  target_units.z = 0.0;
  
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(X_MIN_PIN, INPUT_PULLUP);
  pinMode(X_MAX_PIN, INPUT_PULLUP);
  
  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(Y_MIN_PIN, INPUT_PULLUP);
  pinMode(Y_MAX_PIN, INPUT_PULLUP);
  
  pinMode(Z_STEP_PIN, OUTPUT);
  pinMode(Z_DIR_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);
  pinMode(Z_MIN_PIN, INPUT_PULLUP);
  pinMode(Z_MAX_PIN, INPUT_PULLUP);
  
  //figure our stuff.
  translate_point();

#ifdef DEV_SENSORZERO  
     Serial.println("Skip Machine Zero()");   
#else
    goto_machine_zero();
#endif 

}


void goto_machine_zero()
{
Serial.println("top machine_zero"); 

//  output("X_MIN_PIN",X_MIN_PIN);
//  output("X_MAX_PIN",X_MAX_PIN);
//  output("X_STEP_PIN",X_STEP_PIN);
//  output("X_DIR_PIN",X_DIR_PIN);
//  output("Dir",0);
  
   move_to_max(X_MIN_PIN, X_MAX_PIN, X_DIR_PIN, 0);//X

//Serial.println();
//Serial.println("second stage machine_zero");

//  output("Y_MIN_PIN",Y_MIN_PIN);
//  output("Y_MAX_PIN",Y_MAX_PIN);
//  output("Y_STEP_PIN",Y_STEP_PIN);
//  output("Y_DIR_PIN",Y_DIR_PIN);
//  output("Dir",0);
  
   move_to_max(Y_MIN_PIN, Y_MAX_PIN, Y_DIR_PIN, 0);//y
//Serial.println("bottom machine_zero");
}


void move_to_max(int limiter_pin, int stepper_pin, int stepper_dir_pin,int dir)
{
int maxDistance = 3;
int kntr = 0;
Serial.println("move_to_max");
 //   while(can_step(limiter_pin, limiter_pin, 0, 1, dir))
 Serial.println("First while");
   while(can_step(limiter_pin, stepper_pin, 0, 1, dir) && (kntr < maxDistance))
    {
 Serial.print(kntr);     
 Serial.println(" ....do_step forward");  
 Serial.println("ln 121 need something meaningful for move to max");  
    do_step(current_loc);  //(stepper_pin, stepper_dir_pin, 0);
    kntr++;
    delay(1);
    }
  // slowly back unitl pin is released
  Serial.println("Second while");
  while(!can_step(limiter_pin, stepper_pin, 0, 1, dir)&& (kntr > maxDistance -5))
  {
   Serial.print(kntr);     
   Serial.println(" ....do_step backward");    
   do_step(current_loc);  // (stepper_pin, stepper_dir_pin, 1);

    kntr--;
    delay(100);
  }
}



void dda_move(long micro_delay)
{
  //enable our steppers
  digitalWrite(X_ENABLE_PIN, HIGH);
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(Z_ENABLE_PIN, HIGH);
  
  //figure out our deltas
  max_delta = max(delta_steps.x, delta_steps.y);
  max_delta = max(delta_steps.z, max_delta);

  //init stuff.
  long x_counter = -max_delta/2;
  long y_counter = -max_delta/2;
  long z_counter = -max_delta/2;
  
  //our step flags
  bool x_can_step = 0;
  bool y_can_step = 0;
  bool z_can_step = 0;
  
  if (micro_delay >= 16383)
    milli_delay = micro_delay / 1000;
  else
    milli_delay = 0;

  //do our DDA line!
  do
  {
    x_can_step = can_step(X_MIN_PIN, X_MAX_PIN, current_steps.x, target_steps.x, x_direction);
    y_can_step = can_step(Y_MIN_PIN, Y_MAX_PIN, current_steps.y, target_steps.y, y_direction);
    z_can_step = can_step(Z_MIN_PIN, Z_MAX_PIN, current_steps.z, target_steps.z, z_direction);

    if (x_can_step)
    {
      x_counter += delta_steps.x;
      
      if (x_counter > 0)
      {
        do_step(current_loc);
        x_counter -= max_delta;
        
        if (x_direction)
          current_steps.x++;
        else
          current_steps.x--;
      }
    }

    if (y_can_step)
    {
      y_counter += delta_steps.y;
      
      if (y_counter > 0)
      {
        do_step(current_loc);
        y_counter -= max_delta;

        if (y_direction)
          current_steps.y++;
        else
          current_steps.y--;
      }
    }
    
    if (z_can_step)
    {
      z_counter += delta_steps.z;
      
      if (z_counter > 0)
      {
         if(Z_ENABLE_SERVO==0){
          do_step(current_loc);
                                }
        z_counter -= max_delta;
        
        if (z_direction)
          current_steps.z++;
        else
          current_steps.z--;
      }
    }
    
    
        
    //wait for next step.
    if (milli_delay > 0)
      delay(milli_delay);     
    else
      delayMicroseconds(micro_delay);
  }
  while (x_can_step || y_can_step || z_can_step);
  
  //set our points to be the same
  current_units.x = target_units.x;
  current_units.y = target_units.y;
  current_units.z = target_units.z;
  translate_point();
}



           //   long current,   // where we are at t=0 for latest gcode cmd
           //   long target,    // where we want to stepper to halt after latest gcode cmd


bool can_step(byte min_pin, byte max_pin, long current, long target, byte direction)
{

//  output("min_pin",min_pin);
//  output("max_pin",max_pin);
//  output("current",current);
//  output("target",target);
//  output("direction",direction);
  //stop us if we're on target
  if (target == current)
    return false;
  //stop us if we're at home and still going 
  else if (read_switch(min_pin) && !direction)
    return false;
  //stop us if we're at max and still going
  else if (read_switch(max_pin) && direction)
    return false;

  //default to being able to step
  return true;
}
            //byte pinA, byte pinB, byte dir
            
            
void do_step(FloatPoint_type curLoc)  //old line
{
  line(curLoc.x, curLoc.y); 
}
  


bool read_switch(byte pin)
{
  //dual read as crude debounce
  //SENSOR_INVERTING (ie: 1 means open, 0 means closed)
  if ( SENSORS_INVERTING ) 
    return !digitalRead(pin) && !digitalRead(pin);
  else
    return digitalRead(pin) && digitalRead(pin);
}





long to_steps(float steps_per_unit, float units)
{
  return steps_per_unit * units;
}

void set_target(float x, float y, float z)
{
  target_units.x = x;
  target_units.y = y;
  target_units.z = z;
  
  translate_point();
}

void set_position(float x, float y, float z)
{
  current_units.x = x;
  current_units.y = y;
  current_units.z = z;
  
  translate_point();
}



void translate_point()
{
  //figure our deltas.
  delta_units.x = abs(target_units.x - current_units.x);
  delta_units.y = abs(target_units.y - current_units.y);
  delta_units.z = abs(target_units.z - current_units.z);
        
  //set our steps current, target, and delta
  current_steps.x = to_steps(x_units, current_units.x);
  current_steps.y = to_steps(y_units, current_units.y);
  current_steps.z = to_steps(z_units, current_units.z);

  target_steps.x = to_steps(x_units, target_units.x);
  target_steps.y = to_steps(y_units, target_units.y);
  target_steps.z = to_steps(z_units, target_units.z);

  delta_steps.x = abs(target_steps.x - current_steps.x);
  delta_steps.y = abs(target_steps.y - current_steps.y);
  delta_steps.z = abs(target_steps.z - current_steps.z);
  
  //what is our direction
  x_direction = (target_units.x >= current_units.x);
  y_direction = (target_units.y >= current_units.y);
  z_direction = (target_units.z >= current_units.z);

  //set our direction pins as well
//170424_tr Serial.println("cal delta and spin");
//  m1step(x_direction);
//  m2step(y_direction);
  //m3step(z_direction);
  //Replaces
  //digitalWrite(X_DIR_PIN, x_direction);
  //digitalWrite(Y_DIR_PIN, y_direction);
  //digitalWrite(Z_DIR_PIN, z_direction);
  
}

long calculate_feedrate_delay(float feedrate)
{
    //how long is our line length?
  float distance = sqrt(delta_units.x*delta_units.x + delta_units.y*delta_units.y + delta_units.z*delta_units.z);
  long master_steps = 0;
  
  //find the dominant axis.
  if (delta_steps.x > delta_steps.y)
  {
    if (delta_steps.z > delta_steps.x)
      master_steps = delta_steps.z;
    else
      master_steps = delta_steps.x;
  }
  else
  {
    if (delta_steps.z > delta_steps.y)
      master_steps = delta_steps.z;
    else
      master_steps = delta_steps.y;
  }

  //calculate delay between steps in microseconds.  this is sort of tricky, but not too bad.
  //the formula has been condensed to save space.  here it is in english:
  // distance / feedrate * 60000000.0 = move duration in microseconds
  // move duration / master_steps = time between steps for master axis.

  return ((distance * 60000000.0) / feedrate) / master_steps; 

}


long getMaxSpeed()
{
    if (delta_steps.z > 0)
    return calculate_feedrate_delay(FAST_Z_FEEDRATE);
  else
    return calculate_feedrate_delay(FAST_XY_FEEDRATE);

}


void showStepperMove()
{
	  Serial.print("Distance along X-axis/Y-axis/z-axis.....");  // my test ln
	  Serial.print(kntX / x_units);
	  Serial.print(" / ");
	  Serial.println(kntY / y_units);
	  Serial.print(" / ");
	  Serial.println(kntZ / z_units);

}

/*
* End Stepper_control 
*/


#endif

