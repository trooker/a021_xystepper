/*
 * 170424)tr running against 170414 well.  Cut prints
 * 170421_re-build
 * 170405_tr 
 * check config.h
 */



/**
 * See https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 * grubbed bresenham's line algorithm 
 * 
 * Uses bresenham's line algorithm to move both motors
 * @input newx the destination x position
 * @input newy the destination y position
 **/


#include "config.h"


long  step_delay;  // machine version


/**
 * delay for the appropriate number of microseconds
 * @input ms how many milliseconds to wait
 */
void pause(long ms) {
  delay(ms/1000);
  delayMicroseconds(ms%1000);  // delayMicroseconds doesn't work for values > ~16k.
}


/**
 * Set the logical position
 * @input npx new position x
 * @input npy new position y
 */
void set_position(float npx,float npy) {
  // here is a good place to add sanity tests
  current_loc.x=npx;
  current_loc.y=npy;
}



void line(float newx,float newy) {
  long i;
  long errAdj= 0;


Serial.println();
Serial.println("line().......steps......................");

  output("newx fp.x", newx);
  output("newy fp.y", newy);
//  output("current_loc.x: ",current_loc.x);
//  output("current_loc.y: ",current_loc.y);
  
  long difX  = newx-current_loc.x;
  long difY  = newy-current_loc.y;
  int m1stepperPath = difX>0?1:-1;
  int m2stepperPath = difY>0?-1:1;  // because the motors are mounted in opposite directions
  difX = abs(difX);
  difY = abs(difY);

//  output("difX ", difX);
//  output("difY ", difY);

  
  if(difX>difY) {
     errAdj = difX/2;
//output("difX>difY errAdj: ", errAdj);
    for(i=0; i<difX; ++i) {
     m1step(m1stepperPath);
        errAdj += difY;
//output("errAdj += difY: ", errAdj);
        
      if(errAdj>=difX) {
        errAdj -= difX;
//output("errAdj -= difX: ", errAdj);
        
        m2step(m2stepperPath);

      }
      pause(step_delay);
    }
  } else {
    errAdj = difY/2;
    for(i=0; i<difY; ++i) {
      m2step(m2stepperPath);
      errAdj += difX;
      if(errAdj >= difY) {
        errAdj -= difY;
        m1step(m1stepperPath);

      }
      pause(step_delay);
    }
  }
  showStepperMove();
  set_position(newx,newy);
//  current_loc.x = newx;
//  current_loc.y = newy;
}





//init our string processing
void init_parse_cmdln()
{
	//init our command
	for (byte i=0; i<COMMAND_SIZE; i++)
		commands[i] = 0;
	serial_count = 0;
}



//our feedrate variables.
//float feedrate = 0.0;
//long feedrate_micros = 0;

//Read the string and execute instructions
void parse_cmdln(char instruction[], int size)
{

//Serial.println("hitting parse_cmd");

String showStr;
for (int i=0; i < size; i++)
{
   showStr[i] = instruction[i];
}


//Serial.print("process_string 137 Cmd..: ");
//Serial.println(showStr);



	//the character / means delete block... used for comments and stuff.
	if (instruction[0] == '/')
	{
		//Serial.println("ok ln 54");
		return;
	}

	byte code = 0;
	
        
	//did we get a gcode?
	if (!has_command('$', instruction, size)&&(
		has_command('G', instruction, size) ||
		has_command('X', instruction, size) ||
		has_command('Y', instruction, size) ||
		has_command('Z', instruction, size) ||
		has_command('A', instruction, size) ||   //rotary table
		has_command('I', instruction, size) ||   // see LinuxCNC
		has_command('J', instruction, size)	)    // see linuxCNC
	)
	{
		//which one?
		code = (int)search_string('G', instruction, size);
//    Serial.print("we have G code...: ");
//    Serial.println(code);
//    Serial.println(instruction);
		
		// Get co-ordinates if required by the code type given
		switch (code)
		{
			case 0: //G00 line Fast
			case 1: //G01 line
			case 2: //G02 clockwise arc
			case 3: //G03 counter clockwise arc
				if(abs_mode)
				{
					//we do it like this to save time. makes curves better.
					//eg. if only x and y are specified, we dont have to waste time looking up z.
					if (has_command('X', instruction, size))
						fp.x = search_string('X', instruction, size);
					else
						fp.x = current_units.x;
				
					if (has_command('Y', instruction, size))
						fp.y = search_string('Y', instruction, size);
					else
						fp.y = current_units.y;
				
					if (has_command('Z', instruction, size))
						fp.z = search_string('Z', instruction, size);
					else
						fp.z = current_units.z;
					if (has_command('A', instruction, size))
						fp.a = search_string('A', instruction, size);
					else
						fp.a = current_units.a;
					if (has_command('I', instruction, size))
						fp.i = search_string('I', instruction, size);
					else
						{
						    fp.i = 0.0;
						    current_units.i = 0.0;
						}
					if (has_command('J', instruction, size))
						fp.j = search_string('J', instruction, size);
					else
					    {
						fp.j = 0.0;
					    current_units.j = 0.0;
					    }
				}
				else
				{
					fp.x = search_string('X', instruction, size) + current_units.x;
					fp.y = search_string('Y', instruction, size) + current_units.y;
					fp.z = search_string('Z', instruction, size) + current_units.z;
					fp.a = search_string('A', instruction, size) + current_units.a;
					fp.i = search_string('I', instruction, size) + current_units.i;
					fp.j = search_string('J', instruction, size) + current_units.j;
				}

//       Serial.print(fp.x);
//       Serial.print(" ...fp.x.. ");
//       Serial.print(fp.y);
//       Serial.println(" ...fp.y.. ");

      //170404 need to convert to Z AxistargetPosServo = fp.z;
			break;
		}
		//do something!
		switch (code)
		{
			//Rapid Positioning
			//Linear Interpolation
			//these are basically the same thing.
			case 0:
			case 1:
				//set our target.
				set_target(fp.x, fp.y, fp.z);
        //170404 no servo using ZAxis servo.write(targetPosServo); 
				//do we have a set speed?
				 extrude = search_string('E', instruction, size);
						if (extrude > 0)
						{
						    fp.z = 0.0000;  //set Z to zero pint.  http://reprap.org/wiki/G-code sse G10 Tool Offset
							Serial.println("ln 210 process_string: Extrude something");
						}	
		 
				
				if (has_command('G', instruction, size))
				{
					//adjust if we have a specific feedrate.
					if (code == 1)
					{
						//how fast do we move?
						feedrate = search_string('F', instruction, size);
						if (feedrate > 0)
							feedrate_micros = calculate_feedrate_delay(feedrate);
						//nope, no feedrate
						else
							feedrate_micros = getMaxSpeed();
					}
					//use our max for normal moves.
					else
						feedrate_micros = getMaxSpeed();
				}
				//nope, just coordinates!
				else
				{
					//do we have a feedrate yet?
					if (feedrate > 0)
						feedrate_micros = calculate_feedrate_delay(feedrate);
					//nope, no feedrate
					else
						feedrate_micros = getMaxSpeed();
				}

				//finally move.
        Serial.println("ln 259 process_string  Rapid Linear Interpolation");
        
        line(fp.x, fp.y);
        run_steppers();
       
				//dda_move(feedrate_micros);
			break;
			
			//Clockwise arc
			case 2:
			//Counterclockwise arc
			case 3:
				FloatPoint_type cent;

				// Centre coordinates are always relative
				cent.x = search_string('I', instruction, size) + current_units.x;
				cent.y = search_string('J', instruction, size) + current_units.y;
				float angleA, angleB, angle, radius, length, aX, aY, bX, bY;

				aX = (current_units.x - cent.x);
				aY = (current_units.y - cent.y);
				bX = (fp.x - cent.x);
				bY = (fp.y - cent.y);
				
				if (code == 2) { // Clockwise
					angleA = atan2(bY, bX);
					angleB = atan2(aY, aX);
				} else { // Counterclockwise
					angleA = atan2(aY, aX);
					angleB = atan2(bY, bX);
				}

				// Make sure angleB is always greater than angleA
				// and if not add 2PI so that it is (this also takes
				// care of the special case of angleA == angleB,
				// ie we want a complete circle)
				if (angleB <= angleA) angleB += 2 * M_PI;
				angle = angleB - angleA;

				radius = sqrt(aX * aX + aY * aY);
				length = radius * angle;
				int steps, s, step;
				steps = (int) ceil(length / curve_section);

				FloatPoint_type newPoint;
				for (s = 1; s <= steps; s++) {
					step = (code == 3) ? s : steps - s; // Work backwards for CW
					newPoint.x = cent.x + radius * cos(angleA + angle * ((float) step / steps));
					newPoint.y = cent.y + radius * sin(angleA + angle * ((float) step / steps));
					set_target(newPoint.x, newPoint.y, fp.z);

					// Need to calculate rate for each section of curve
					if (feedrate > 0)
						feedrate_micros = calculate_feedrate_delay(feedrate);
					else
						feedrate_micros = getMaxSpeed();

					// Make step
					dda_move(feedrate_micros);
				}
	
			break;

			//Dwell
			case 4:
				delay((int)search_string('P', instruction, size));
			break;

			
			case 20:  //Inches for Units
				x_units = X_STEPS_PER_INCH;
				y_units = Y_STEPS_PER_INCH;
				z_units = Z_STEPS_PER_INCH;
				curve_section = CURVE_SECTION_INCHES;
				
				translate_point();
			break;

			
			case 21:  //mm for Units
				x_units = X_STEPS_PER_MM;
				y_units = Y_STEPS_PER_MM;
				z_units = Z_STEPS_PER_MM;
				curve_section = CURVE_SECTION_MM;
				
				translate_point();
			break;

			
			case 28:  //go home.
				set_target(0.0, 0.0, 0.0);
				goto_machine_zero();
			break;

			
			case 30:  //go home via an intermediate point.
				fp.x = search_string('X', instruction, size);
				fp.y = search_string('Y', instruction, size);
				fp.z = search_string('Z', instruction, size);

				//set our target.
				if(abs_mode)
				{
					if (!has_command('X', instruction, size))
						fp.x = current_units.x;
					if (!has_command('Y', instruction, size))
						fp.y = current_units.y;
					if (!has_command('Z', instruction, size))
						fp.z = current_units.z;
						
					set_target(fp.x, fp.y, fp.z);
				}
				else
					set_target(current_units.x + fp.x, current_units.y + fp.y, current_units.z + fp.z);
				
				//go there.
				dda_move(getMaxSpeed());

				//go home.
				set_target(0.0, 0.0, 0.0);
				goto_machine_zero();
			break;

			//Absolute Positioning
			case 90:
				abs_mode = true;
			break;

			//Incremental Positioning
			case 91:
				abs_mode = false;
                        
			break;

			//Set as home
			case 92:
				set_position(0.0, 0.0, 0.0);
			break;

/*
			//Inverse Time Feed Mode
			case 93:

			break;  //TODO: add this

			//Feed per Minute Mode
			case 94:

			break;  //TODO: add this
*/

			default:
				Serial.print("huh? G");
				Serial.println(code,DEC);
		}
	}
  if (has_command('M', instruction, size))
	{
		code = search_string('M', instruction, size);
		switch (code)
		{
			//TODO: this is a bug because search_string returns 0.  gotta fix that.
			case 0:
				true;
			break;
      case 100:  help();  break;
      case 114:  where();  break;
			
			default:
				Serial.print("Huh? M");
				Serial.println(code);
		}		
	}
        if(has_command('$', instruction, size)){
          
            code = search_string('$', instruction, size);
            switch(code){
              case 1:
             //set XYZ STEP PIN
             if (has_command('X', instruction, size)){
		X_STEP_PIN = search_string('X', instruction, size);
                pinMode(X_STEP_PIN,OUTPUT);
                digitalWrite(X_STEP_PIN,LOW);
            }
            if (has_command('Y', instruction, size)){
		Y_STEP_PIN = search_string('Y', instruction, size);
                pinMode(Y_STEP_PIN,OUTPUT);
                digitalWrite(Y_STEP_PIN,LOW);
            }
            if (has_command('Z', instruction, size)){
		int TEMP_PIN = search_string('Z', instruction, size);
                
                  if(Z_STEP_PIN!=TEMP_PIN){
                    Z_STEP_PIN = TEMP_PIN;
                    if(Z_ENABLE_SERVO==1){
                      //170404 no convert to z Axisservo.attach(Z_STEP_PIN);
                    }else{
                       pinMode(Z_STEP_PIN,OUTPUT);  
                       digitalWrite(Z_STEP_PIN,LOW);   
                    }
                  }
            }
             break;
            case 2:
             //set XYZ DIR PIN
             if (has_command('X', instruction, size)){
		X_DIR_PIN = search_string('X', instruction, size);
                pinMode(X_DIR_PIN,OUTPUT);
                digitalWrite(X_DIR_PIN,LOW);
            }
            if (has_command('Y', instruction, size)){
		Y_DIR_PIN = search_string('Y', instruction, size);
                pinMode(Y_DIR_PIN,OUTPUT);
                digitalWrite(Y_DIR_PIN,LOW);
            }
            if (has_command('Z', instruction, size)){
		Z_DIR_PIN = search_string('Z', instruction, size);
                pinMode(Z_DIR_PIN,OUTPUT);
                digitalWrite(Z_DIR_PIN,LOW);
            }
            break;
            case 3:
             //set XYZ Min PIN
             if (has_command('X', instruction, size)){
		X_MIN_PIN = search_string('X', instruction, size);
                pinMode(X_MIN_PIN,INPUT_PULLUP);
            }
            if (has_command('Y', instruction, size)){
		Y_MIN_PIN = search_string('Y', instruction, size);
                pinMode(Y_MIN_PIN,INPUT_PULLUP);
            }
            if (has_command('Z', instruction, size)){
		Z_MIN_PIN = search_string('Z', instruction, size);
                pinMode(Z_MIN_PIN,INPUT_PULLUP);
            }
            break;
            case 4:
             //set XYZ Max PIN
             if (has_command('X', instruction, size)){
		X_MAX_PIN = search_string('X', instruction, size);
                pinMode(X_MAX_PIN,INPUT_PULLUP);
            }
            if (has_command('Y', instruction, size)){
		Y_MAX_PIN = search_string('Y', instruction, size);
                pinMode(Y_MAX_PIN,INPUT_PULLUP);
            }
            if (has_command('Z', instruction, size)){
		Z_MAX_PIN = search_string('Z', instruction, size);
                pinMode(Z_MAX_PIN,INPUT_PULLUP);
            }
            break;
            case 5:
             //ENABLE SERVO MOTOR FOR Z
             if(has_command('Z',instruction,size)){
               Z_ENABLE_SERVO = search_string('Z', instruction, size);
               
             }
             break;
             case 6:
             //set XYZ STEPS PER MM
             if (has_command('X', instruction, size)){
		X_STEPS_PER_MM = search_string('X', instruction, size);
                x_units = X_STEPS_PER_MM;
                Serial.println(x_units);
            }
            if (has_command('Y', instruction, size)){
		Y_STEPS_PER_MM = search_string('Y', instruction, size);
                y_units = Y_STEPS_PER_MM;
            }
            if (has_command('Z', instruction, size)){
		Z_STEPS_PER_MM = search_string('Z', instruction, size);
                z_units = Z_STEPS_PER_MM;
            }
            break;
            case 7:
             //set XYZ FEEDRATE
             if (has_command('X', instruction, size)){
		FAST_XY_FEEDRATE = search_string('X', instruction, size);
            }else if (has_command('Y', instruction, size)){
		FAST_XY_FEEDRATE = search_string('Y', instruction, size);
            }
            if (has_command('Z', instruction, size)){
		FAST_Z_FEEDRATE = search_string('Z', instruction, size);
            }
            break;
            case 8:
             //set XYZ INVERT LIMIT SWITCH
             if (has_command('S', instruction, size)){
		SENSORS_INVERTING = search_string('S', instruction, size);
            }
            break;
            }
            
        }
	//tell our host we're done.
      if(code==0&&size==1){
        Serial.println("what does this do for us???? start ln 551 of process-string");
      }else{
        //Serial.println("ok ln 446");
      }
//	Serial.println(line, DEC);
}

//look for the number that appears after the char key and return it
double search_string(char key, char instruction[], int string_size)
{
	char temp[10] = "";

	for (byte i=0; i<string_size; i++)
	{
		if (instruction[i] == key)
		{
			i++;      
			int k = 0;
			while (i < string_size && k < 10)
			{
				if (instruction[i] == 0 || instruction[i] == ' ')
					break;

				temp[k] = instruction[i];
				i++;
				k++;
			}
			return strtod(temp, NULL);
		}
	}
	
	return 0;
}

//look for the command if it exists.
bool has_command(char key, char instruction[], int string_size)
{
	for (byte i=0; i<string_size; i++)
	{
		if (instruction[i] == key)
			return true;
	}
	
	return false;
}

