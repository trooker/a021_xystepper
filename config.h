#ifndef CONFIG_H
#define CONFIG_H
//------------------------------------------------------------------------------
// 2 Axis CNC Demo
// dan@marginallycelver.com 2013-08-30
//------------------------------------------------------------------------------
// Copyright at end of file.
// please see http://www.github.com/MarginallyClever/GcodeCNCDemo for more information.


//------------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------------
// supported control boards
//#define DEBUG 1

#define AMS1 (1)
#define AMS2 (2)

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>
#include <SPI.h>
#include <SD.h>
#include "rgb_lcd.h"




// change this line to select a different control board for your CNC.
#define CONTROLLER AMS1


/* Machine Parameters*/
// define the parameters of our machine.

/* Radioshack
float X_STEPS_PER_INCH = 30.72;  //48;
float X_STEPS_PER_MM = 25.6;     //40;
int X_MOTOR_STEPS   = 64;  //100;  5.625 degree per step
*/

float X_STEPS_PER_INCH = 30.72;  //48;
float X_STEPS_PER_MM = 25.6;     //40;
int X_MOTOR_STEPS   = 320;  //100;  1.125 degree per step

/* HP
float Y_STEPS_PER_INCH = 96;   //48;
float Y_STEPS_PER_MM  = 80;  //40;
int Y_MOTOR_STEPS   = 200;  //100;  1.8 degree per step
*/

float Y_STEPS_PER_INCH = 30.72;  //48;
float Y_STEPS_PER_MM = 25.6;     //40;
int Y_MOTOR_STEPS   = 320;  //100;  1.125 degree per step


/* furture  */
float Z_STEPS_PER_INCH = 96;   //48;
float Z_STEPS_PER_MM  = 80;  //40;
int Z_MOTOR_STEPS   = 200;  //100;  1.8 degree per step

/*
float Z_STEPS_PER_INCH = 48;
float Z_STEPS_PER_MM   = 40;
int Z_MOTOR_STEPS    = 100;
*/

float A_STEPS_PER_INCH = 30.72;  //48;
float A_STEPS_PER_MM = 25.6;     //40;
int A_MOTOR_STEPS   = 320;  //100;  1.125 degree per step



//our maximum feedrates
long FAST_XY_FEEDRATE = 2000;
long FAST_Z_FEEDRATE = 2000;
long FAST_A_FEEDRATE = 2000;

// Units in curve section
//180804_tr may need to adjust for I and J
#define CURVE_SECTION_INCHES 0.019685
#define CURVE_SECTION_MM 0.5

// Set to one if sensor outputs inverting (ie: 1 means open, 0 means closed)
// RepRap opto endstops are *not* inverting.
int SENSORS_INVERTING = 1;

/****************************************************************************************
* digital i/o pin assignment
*
* this uses the undocumented feature of Arduino - pins 14-19 correspond to analog 0-5
****************************************************************************************/

int X_STEP_PIN = 8;
int X_DIR_PIN = 11;
int X_ENABLE_PIN = 4;
int X_MIN_PIN = A4;
int X_MAX_PIN = A5;

int Y_STEP_PIN = 12;
int Y_DIR_PIN = 13;
int Y_ENABLE_PIN = 4;
int Y_MIN_PIN = A1;
int Y_MAX_PIN = A0;

int Z_STEP_PIN = A3;
int Z_DIR_PIN = 9;
int Z_ENABLE_PIN = 4;
int Z_MIN_PIN = A1;
int Z_MAX_PIN = A0;
int Z_ENABLE_SERVO = 1;


// our point structure to make things nice.
struct LongPoint {
  long x;
  long y;
  long z;
  long a;
  long i;
  long j;
};

typedef struct  {
  float x;
  float y;
  float z;
  float a;
  float i;
  float j;
} FloatPoint_type;

  //init baby!
  FloatPoint_type fp; // gcode command string from g0, g1, g2, g3
  //fp.x derived from GCode X input
  //fp.y derived from GCode Y input
  //fp.z derived from GCode Z input


FloatPoint_type current_loc;   //current location
FloatPoint_type current_units;
FloatPoint_type target_units;
FloatPoint_type delta_units;

FloatPoint_type current_steps;
FloatPoint_type target_steps;
FloatPoint_type delta_steps;

boolean abs_mode = false;   //0 = incremental; 1 = absolute

//default to inches for units
float x_units = X_STEPS_PER_MM;
float y_units = Y_STEPS_PER_MM;
float z_units = Z_STEPS_PER_MM;
float a_units = A_STEPS_PER_MM;

float curve_section = CURVE_SECTION_MM;

//our direction vars
byte x_direction = 1;
byte y_direction = 1;
byte z_direction = 1;
byte a_direction = 1;


//our feedrate variables.
float feedrate = 0.0;
long feedrate_micros = 0;

float extrude = 0.0;


//Dev
int kntX = 0;
int kntY = 0;
int kntZ = 0;
int kntA = 0;



//init our variables
long max_delta;
long x_counter;
long y_counter;
long z_counter;
long a_counter;

bool x_can_step;
bool y_can_step;
bool z_can_step;
bool a_can_step;
int milli_delay;




//For dev 170331
#define FORWARD 1
#define BACKWARD 2


#define MICROSTEP 4



#define VERSION        (1)  // firmware version
#define BAUD           (9600)  // was 11How fast is the Arduino talking?
#define MAX_BUF        (64)  // What is the longest message Arduino can store?
#define STEPS_PER_TURN (200)  // depends on your stepper motor.  most are 200.
#define MIN_STEP_DELAY (50.0)
#define MAX_FEEDRATE   (1000000.0/MIN_STEP_DELAY)
#define MIN_FEEDRATE   (0.01)


#define COMMAND_SIZE 128


// for arc directions
#define ARC_CW          (1)
#define ARC_CCW         (-1)
// Arcs are split into many line segments.  How long are the segments?
#define MM_PER_SEGMENT  (10)

int sderrFlg = -1;  // -1: XCare 0: okay  1: Err state


//------------------------------------------------------------------------------
// METHODS
//------------------------------------------------------------------------------
extern void m1step(int dir);
extern void m2step(int dir);
extern void m3step(int dir);
extern void disable();
extern void setup_controller();
extern void run_steppers();
extern void init_steppers(); //ams1


extern void parse_cmdln();
extern void init_process_string(); //process_string



/**
* This file is part of GcodeCNCDemo.
*
* GcodeCNCDemo is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* GcodeCNCDemo is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Foobar. If not, see <http://www.gnu.org/licenses/>.
*/
#endif

