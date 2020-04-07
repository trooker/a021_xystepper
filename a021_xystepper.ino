
/*
 * 200405_tr copied from a021_xystepper
 * 200405_tr_ renamed to suit project from: a021_xystepper_gCodeParser_012.ino
 * 180804_tr started adding a axis, I and J for g02/g03 arch moves.
 * 170421_tr completed rebuild using hte Sloeber  G1 X230 Y300 runs.
 *     G1 X422 Y70 runs steppers
 * 170420 Trying to conver to Sloeber
 *     There is a problem with the input 170420_xy... or output to the log
 * 170419 GtkTerm (ASCII tty_ACM0) 170419_usc18 hangs and awaits firing of
 *        gcode file
 * 170418 GtkTerm and 170418_usc018_eject_logadj.cnc no longer works
 *        Note 170419_*.cnc was built from 170416_log_2158.
 * 170415 Cleanup serial prints and leverge feedrate
 *    Serial Monitor
 *    170414_usc018_eject.cnc line by line
 *    G1 X51.668 Y72.274 F7800.000 => RUns Y okay
 *
 * 170413_work with feedrate, absolute/incremental and units of measure
 *         F, G90/G91, G20/G21
 *         G90;G20; G1 X300 Y100 turns every thing
 *         Corrected for _STEPS_PER_IN/MM
 *         G90; G20; G1 X460 Y10
 * 170411 Added Working on FeedRate
 * 170410 Both XY work to some level via Serial Monitor
 *        G0 X230 Y50
 *        G0 X300 Y5
 *        G0 X-330 Y4
 *          Yeilds
 *          errAdj += difY 275..298
 *                         299, 300
 *             Called backward1()
 *        G0 X500 Y2
 *          errAdj += difY 370, 372,by 2's 400
 *             Called forward1()
 *        G1 X199 Y0
 *        G1 X-195 Y3
 *        Read 002.2_testLine_repeat.cnc from GtkTerm
 *          But no stepper motion
 *        Read boomerangv4_03.cnc from GtkTerm
 *          But no stepper motion
 * 170409 looking at line() function of parse_cmdln
 * 170402_merge GCodeCNCDemo2 with Adafruit Motor shield
 *     drop extraneous controllers
 *     works with G00 code from Serial Monitor command line.
 *   1> Used GtkTerm and boomeragn.cnc to drive steppers
 *   2> GtkTerm copy/paste of r004_002_testLine runlog to drive
 *   3> raw file GtkTerm + 002_testLine_repeast.nc
 *   4> raw file GtkTerm ...
 *
 *
 *
 * 170330
 *
 * https://github.com/MarginallyClever/GcodeCNCDemo/wiki
 * https://github.com/MarginallyClever/GcodeSender
 */


#include "config.h"

//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------

char  buffer[MAX_BUF];  // where we store the message until we get a newline
//int   sofar;            // how much is in the buffer
int copyMax = 1;
int copyKnt = 0;



// settings

//From GCodeParser
//#define COMMAND_SIZE 128
char commands[COMMAND_SIZE];
byte serial_count;
int no_data = 0;
bool comment = false;



//------------------------------------------------------------------------------
// METHODS
//------------------------------------------------------------------------------


//170701_tr
rgb_lcd lcd;

int cnt = 0;
const int LED    = 2;       // the Grove port No. you attached an LED to


String readString = "";
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated
// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)
int doneFlag = -1;              // think tristate
int kntr = 0;                   // loop counter

//Red LCD Background
/*
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;
*/
//Silver LCD background
const int colorR = 192;
const int colorG = 192;
const int colorB = 192;




void showSetup()
{
	  // set up the LCD's number of columns and rows:
	    readString = "Setup fina. 2132.";
	    lcd.begin(16, 2);
	    pinMode(LED, OUTPUT);   //set LED as an OUTPUT device
	    showDisplay(readString);
	    readString = "";  // re-initialize
	    doneFlag++;
}


void showDisplay(String msgStr)
{

    // clear the screen
    lcd.clear();
	lcd.setCursor(0, 0);
    if ((doneFlag < 1) and (kntr < 51))
	{
	    lcd.print(msgStr);
        digitalWrite(LED, 1);
        delay(10);
        digitalWrite(LED, 0);
	}
	else
	{
		//180804 increments past 1 doneFlag++;
		showDone();
	}
}



void showDone()
{
	if (doneFlag == 0)
	{
        lcd.setRGB(colorR, colorG, colorB);
		showDisplay("Fina.  Reset");
	//lcd.noDisplay();
//	https://learn.adafruit.com/multi-tasking-the-arduino-part-1/using-millis-for-timing
//  http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay

    // check to see if it's time to blink the LED; that is, if the
    // difference between the current time and last time you blinked
    // the LED is bigger than the interval at which you want to
    // blink the LED.
    unsigned long currentMillis = millis();

//    if(currentMillis - previousMillis > interval) {
      // save the last time you blinked the LED
 //     previousMillis = currentMillis;

      // if the LED is off turn it on and vice-versa:
//      if (LED == LOW)
//        ledState = HIGH;
//      else
//        ledState = LOW;// the follow variables is a long because the time, measured in miliseconds,
      // set the LED with the ledState of the variable:
//       digitalWrite(LED, ledState);
       doneFlag++;
  //  }
	} //doneFlag == 0
	else if (doneFlag == 1)
	{
        ledState = LOW;// the follow variables is a long because the time, measured in miliseconds,
      // set the LED with the ledState of the variable:
        digitalWrite(LED, ledState);

		//doneFlag++;
	    lcd.noDisplay();
	}
}


void showMills()
{
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(millis()/1000);
    delay(100);
}




void showInstruction()
{

    // clear the screen
    lcd.clear();
    // read all the available characters
    char *ptr=commands;
    for(int i=0; i< serial_count; i++)
     {
      //  ptr=strchr(ptr,'a')+1;
      //  Serial.println(ptr);
        readString = readString + buffer[i];
       // readString += *ptr;
     }
    Serial.print("Read: ");
    Serial.println(readString);
    Serial.println("end");
        // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 0);

        lcd.print(readString);
//        digitalWrite(LED, 1);
//        delay(10);
//        digitalWrite(LED, 0);
     readString = "";
}




/**
 * write a string followed by a float to the serial line.  Convenient for debugging.
 * @input code the string.
 * @input val the float.
 */
void output(const char *code,float val)
{
  Serial.print(code);
  Serial.print(".....");  // my test ln
  Serial.println(val);
}


/**
 * print the current position, feedrate, and absolute mode.
 */
void where()
{
  output("kntX",kntX);
  output("kntY",kntY);
  output("Distance along X Axis",kntX / x_units );
  output("Distance along Y Axis",kntY / y_units);
  output("x units", x_units);
  output("y units", y_units);
  output("Current_LocX",current_loc.x);
  output("Current_LocY",current_loc.y);
  output("Last fp.X requested via gCode ",fp.x);
  output("Last fp.Y requested via gCode ",fp.y);
  output("F",feedrate);
  output("E", extrude);
  output("abs_mode ",abs_mode);
}

void help()
{
	//Serial.println("Need help with Flash memory F thingie");
	  Serial.print(F("GcodeCNCDemo2AxisV1 Help"));
	  Serial.println(VERSION);
	  Serial.println(F("Unit of Measure is in Inches"));
	  Serial.println(F("Commands:"));
	  Serial.println(F("G00 [X(steps)] [Y(steps)] [E(extrude)] [F(feedrate)]; - line"));
	  Serial.println(F("G01 [X(steps)] [Y(steps)]  [E(extrude)] [F(feedrate)]; - line"));
	  Serial.println(F("G02 [X(steps)] [Y(steps)] [I(steps)] [J(steps)] [E(extrude)] [F(feedrate)]; - clockwise arc"));
	  Serial.println(F("G03 [X(steps)] [Y(steps)] [I(steps)] [J(steps)] [E(extrude)] [F(feedrate)]; - counter-clockwise arc"));
	  Serial.println(F("G04 P[seconds]; - delay"));
	  Serial.println(F("G20 Set units to inches"));
	  Serial.println(F("G21 Set units to mm"));
	  Serial.println(F("G90; - absolute mode"));
	  Serial.println(F("G91; - relative mode"));
	  Serial.println(F("G92 [X(steps)] [Y(steps)]; - change logical position"));
	  Serial.println(F("M02; - End of Program"));
	  Serial.println(F("M05; - Stop spindle"));
	  Serial.println(F("M18; - disable motors"));
	  Serial.println(F("M100; - this help message"));
	  Serial.println(F("M114; - report position and feedrate"));
	  Serial.println(F("All commands must end with a newline."));

}


/**
 * prepares the input buffer to receive a new message and tells the serial connected device it is ready for more.
 */
void ready()
{
//  sofar=0;  // clear input buffer
  Serial.print(F(">"));  // signal ready to receive input
}

void mgtCmdLn(char c)
{
	    no_data = 0;
	    //newlines are ends of commands.
	    if (c != '\n')
	    {
	      if(c==0x18)
	      {
	        Serial.println("Grbl 1.0");
	      }
	      else
	      {
             if ((c == ';')or (c == '('))
	         {comment = true;}
	         // If we're not in comment mode, add it to our array.
	         if (!comment)
	         {
	           commands[serial_count] = c;
	           serial_count++;
	         }
	         if (c == ')')
	         { comment = false; }// End of comment - start listening again
	       } //
	    }
	 //}
      else
	  {
	    no_data++;
	    delayMicroseconds(100);

	  //if theres a pause or we got a real command, do it
	  if (serial_count && ((c == '\n'  || (c == 0x0A) ) || no_data > 100))
	  {
	    //process our command!
	  Serial.print("ln 2 cmd..: ");
	  Serial.println(commands);
	  showInstruction();
	    parse_cmdln(commands, serial_count);
	    //clear command.
	    init_parse_cmdln();
	  }
	  }
	  }	   //mgtCmdLn()



void do_serial_read()
{
	  char c;
	  //read in characters if we got them.
	  if (Serial.available() > 0)
	  {
	    c = Serial.read();
        mgtCmdLn(c);
    }
}


void setup()
{
	  Serial.begin(BAUD);  // open coms
	  Serial.println("Serial is runnnig");
	  setup_controller();  //ams1
      set_position(0,0);  // set staring position
	 //init_steppers does this feedrate((MAX_FEEDRATE + MIN_FEEDRATE)/2);  // set default speed

 	  init_parse_cmdln(); //process string
	  init_steppers();   //ams1
	  parse_cmdln("G90",3);//Absolute Position. process_string
      parse_cmdln("G20",3);//Force Inches. process_string

	  Serial.println("start");

	  setup_sd();

	  help();  // say hello
	  ready();

// 170709_tr
      showSetup();


}

// The loop function is called in an endless loop
void loop()
{
    showMills();

	  //loop_parser();
     if (sderrFlg == 0)
     {
    	showDisplay("SD Read");
        loop_sdread();
     }
     else
     {
         while ( copyKnt < copyMax)
         {
    	    do_serial_read();
    	    copyKnt++;
         }
     }
	  //no data?  turn off steppers
	  if (no_data > 1000)
	  {
	    disable();
	    showDone();
	  }
}
