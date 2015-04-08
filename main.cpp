/*
 * This is the program that will be loaded onto the
 * Proteus to compete in the Final Competition. This is it.
 *
 * ROBOT NOTE:
 * The right wheel is correct, but the left wheel is backwards.
 * This applies when driving with the forklift forwards.
 */

#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#include <FEHServo.h>
#include <FEHBattery.h>
//The number of counts when an inch is travelled
#define COUNTS_PER_INCH 40
//The number of counts when 1 degree is turned
#define COUNTS_PER_DEGREE 233/90
//The point South of the starting zone
#define POINT_A 20.0
//The point at the bottom of the ramp
#define POINT_B 28.5
//The point at the top of the ramp
#define POINT_C 52.1
//The point at the crank
#define POINT_D 57.7
//The necessary position for cranking
#define POINT_E 30.7
//The point by the ramp wall
#define POINT_F 28.5
//The point at the top of the ramp
#define POINT_G 43.4
//The point in front of the top of the ramp
#define POINT_H 48.7
//The point to which the snow is being plowed
#define POINT_I 10.2
//The point before the garage
#define POINT_J 12.3
//The point at the garage
#define POINT_K 6.3
//The point in front of, but a bit back from, the buttons
#define POINT_L 60.2
//The point directly in front of the buttons
#define POINT_M 13.5
//The point at the bottom of the ramp (heading down)
#define POINT_N 21.8
//The point north of the pump
#define POINT_O 17.1
//The standard driving percentage
#define STD_DRIVE 35
//The fast driving percentage
#define FAST_DRIVE 50
//The slow driving percentage
#define SLOW_DRIVE 12
//The max driving speed. Only for use in extreme circumstances.
#define HYPER_DRIVE 100 //Think before you do this.
//The standard sleep time
#define SLEEP_TIME 500
//The short sleep time used in RPS checks
#define SHORT_SLEEP 50
//The CdS Threshold
#define CDS_THRESHOLD .55
//The crank threshold
#define CDS_CRANK .54
//The angle to press the red button
#define RED_ANGLE 39
//The angle to press the blue button
#define BLUE_ANGLE 164
//The angle to press the white button
#define WHITE_ANGLE 91
//Servo motor minimum
#define SERVO_MIN 510
//Servo motor maximum
#define SERVO_MAX 2260
//Soft (temporary) min limit of servo distance
#define SERVO_SMIN 20
//Hard max limit of servo distance
#define SERVO_HMAX 180
//Hard min limit of servo distance
#define SERVO_HMIN 0
//Declare the various sensors and motors
ButtonBoard buttons(FEHIO::Bank3); //Button Board
FEHServo disk( FEHServo::Servo0); //Disk Mechanism
FEHMotor right_motor( FEHMotor::Motor0); //Right Motor (Backwards)
FEHMotor left_motor( FEHMotor::Motor1); //Left Motor
DigitalEncoder right_encoder( FEHIO::P0_0); //Right encoder
DigitalEncoder left_encoder( FEHIO::P0_2); //Left encoder
AnalogInputPin CdS( FEHIO::P1_0); //CdS cell
//DigitalInputPin right_bump( FEHIO::P1_2); //Right bump switch (if used)
//Prototypes for each function
void readings();
void timedDrive(int percent, float time);
void curveCheck(int x_coordinate, bool fire);
void turnRight(int percent, int degrees, int input_counts);
void turnLeft(int percent, int degrees, int input_counts);
void driveForward(int percent, int inches, int input_counts);
void checkXPlus(float x_coordinate, bool fire);
void checkXMinus(float x_coordinate, bool fire);
void checkYPlus(float y_coordinate, bool fire);
void checkYMinus(float y_coordinate, bool fire);
void checkHeading(float heading, bool fire);
void buttonsOrder(bool fire);
void crankDirection();
void oilRun();
void saltRun(bool RPSFire);
void crankRun(bool RPSFire);
void garageRun(bool RPSFire);
void buttonsRun(bool RPSFire);
void bottomRun(bool RPSFire);

int main(void)
{
    //Variable for whether the RPS works
    bool RPSFire = false;
    //Set up LCD
    LCD.Clear( FEHLCD::Green );
    LCD.SetFontColor( FEHLCD::Black );
    //Set servo limits and initial position
    disk.SetMin(SERVO_MIN);
    disk.SetMax(SERVO_MAX);
    disk.SetDegree(SERVO_SMIN);
    //Ask whether the RPS is working
    LCD.WriteLine("Is the RPS on fire? \n(L = Y, R = N)");
    while(!buttons.MiddlePressed()) {
        if (buttons.LeftPressed()) {
            RPSFire = true;
            LCD.WriteLine("Press middle now.");
            Sleep(SLEEP_TIME);
        } else if (buttons.RightPressed()) {
            Sleep(SHORT_SLEEP);
            RPSFire = false;
            RPS.InitializeMenu();
            LCD.WriteLine("Press middle now.");
            Sleep(SLEEP_TIME);
        }
    }
    Sleep(SLEEP_TIME);
    LCD.Clear( FEHLCD::Green );
    float time = TimeNow() + 35;
    while(true) {
        LCD.WriteLine("Press the middle button to begin");
        while(!buttons.MiddlePressed());
        while(buttons.MiddlePressed());
        LCD.WriteLine("Will start on light now");
        while(CdS.Value()>CDS_THRESHOLD && TimeNow() < time) {
            LCD.WriteRC(CdS.Value(), 5, 12);
            LCD.WriteRC(Battery.Voltage(), 7, 12);
            Sleep(1.0);
        }
        LCD.Clear( FEHLCD::Green );
        //Start and get the salt bag
        saltRun(RPSFire);
        //Drive to the crank
        crankRun(RPSFire);
        //Turn the crank in the proper direction
        crankDirection();
        //Deposit the Salt Bag
        garageRun(RPSFire);
        //Drive to the buttons
        buttonsRun(RPSFire);
        //Press the buttons in order
        buttonsOrder(RPSFire);
        //Drive to the lever
        bottomRun(RPSFire);
        //Drive into the lever in the correct direction
        oilRun();
        //Get readings
        readings();
        }

}
/*************************************************
 * Use this to get RPS readings and CdS Readings *
 *************************************************/
void readings() {
    while(!buttons.MiddlePressed()) {
            LCD.Clear();
            LCD.WriteLine("Press Middle to exit");
            LCD.Write("X: ");
            LCD.WriteLine(RPS.X());
            LCD.Write("Y: ");
            LCD.WriteLine(RPS.Y());
            LCD.Write("Heading: ");
            LCD.WriteLine(RPS.Heading());
            LCD.Write("CdS: ");
            LCD.WriteLine(CdS.Value());
            Sleep(SLEEP_TIME);
        }
}
/*******************************************************
 * Drive forward by time with no shaft encoder usage. *
 *******************************************************/
void timedDrive(int percent, float time) {
    //Turn motors on at given percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(percent);
    //Sleep for given time
    Sleep(time);
    //Turn motors off
    left_motor.Stop();
    right_motor.Stop();
}
/******************************************
 * Check X Position by curving (untested) *
 ******************************************/
void curveCheck(int x_coordinate, bool fire)
{
    while(RPS.X() < x_coordinate - .5 || RPS.X() > x_coordinate + .5)
    {
        //Drive diagonally towards the correct y coordinate.
        if(RPS.X() > x_coordinate)
        {
            right_motor.Stop();
            left_motor.Stop();
            checkHeading(180, fire);
            right_motor.SetPercent(20);
            left_motor.SetPercent(15);
        }
        else if(RPS.X() < x_coordinate)
        {
            right_motor.Stop();
            left_motor.Stop();
            checkHeading(180, fire);
            right_motor.SetPercent(15);
            left_motor.SetPercent(20);
        }
    }
    right_motor.Stop();
    left_motor.Stop();
    checkHeading(180, fire);
}
/*********************
 * Turn to the right *
 *********************/
void turnRight(int percent, int degrees, int input_counts) //using encoders
{
    //Make input degrees and convert degrees to counts
    int counts = degrees*COUNTS_PER_DEGREE + input_counts;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    //hint: set right motor forward, left motor backward
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);
    //While the average of the left and right encoder are less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
/********************
 * Turn to the left *
 ********************/
void turnLeft(int percent, int degrees, int input_counts) //using encoders
{
    //Make input degrees and convert degrees to counts
    int counts = degrees*COUNTS_PER_DEGREE + input_counts;
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(-percent);
    //While the average of the left and right encoder are less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}
/************************************************
 * Drive in the forward or backwards directions *
 ************************************************/
void driveForward(int percent, int inches, int input_counts) {
    //Calculate input counts
    int counts = inches*COUNTS_PER_INCH + input_counts;
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(percent);
    /*if (percent > 0) {
        left_motor.SetPercent(percent + 2);
    } else if (percent < 0) {
        left_motor.SetPercent(percent - 2);
    }*/
    float time = TimeNow();
    while ((left_encoder.Counts() + right_encoder.Counts())/2.<counts && TimeNow() - time < 10);
    if (TimeNow() - time > 10) {
        right_motor.SetPercent(percent);
        left_motor.SetPercent(-percent);
        Sleep(.25);
    }
    right_motor.Stop();
    left_motor.Stop();
}
/*********************************************
 * Check for x position for robot facing x + *
 *********************************************/
void checkXPlus(float x_coordinate, bool fire) {
    //Check if the position is already correct
    while(!fire && (RPS.X() < x_coordinate - .5 || RPS.X() > x_coordinate + .5)) {
        if(RPS.X() > x_coordinate) {
            //If position is bigger than wanted, drive backwards
            driveForward(-SLOW_DRIVE, 0, 2);
        } else if(RPS.X() < x_coordinate) {
            //If the position is smaller than wanted, drive forwards
            driveForward(SLOW_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
    }
}
/*********************************************
 * Check for x position for robot facing x - *
 *********************************************/
void checkXMinus(float x_coordinate, bool fire) {
    //Check if the position is already correct
    while(!fire && (RPS.X() < x_coordinate - .5|| RPS.X() > x_coordinate + .5)) {
        if(RPS.X() > x_coordinate) {
            //If position is bigger than wanted, drive forward
            driveForward(SLOW_DRIVE, 0, 2);
        } else if(RPS.X() < x_coordinate) {
            //If the position is smaller than wanter, drive backwards
            driveForward(-SLOW_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
    }
}
/*********************************************
 * Check for x position for robot facing y - *
 *********************************************/
void checkYMinus(float y_coordinate, bool fire) {
    //Check if the position is already correct
    while(!fire && (RPS.Y() < y_coordinate - .5 || RPS.Y() > y_coordinate + .5)) {
        if(RPS.Y() > y_coordinate) {
            //If the position is bigger than wanted, drive forward
            driveForward(SLOW_DRIVE, 0, 2);
        } else if(RPS.Y() < y_coordinate) {
            //If the position is smaller than wanted, drive backwards
            driveForward(-SLOW_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
    }
}
/*********************************************
 * Check for x position for robot facing y + *
 *********************************************/
void checkYPlus(float y_coordinate, bool fire) {
    //Check if the position is already correct
    while(!fire && (RPS.Y() < y_coordinate - .5 || RPS.Y() > y_coordinate + .5)) {
        if(RPS.Y() > y_coordinate) {
            //If the position is bigger than wanted, drive backwards
            driveForward(-SLOW_DRIVE, 0, 2);
        } else if(RPS.Y() < y_coordinate) {
            //If the position is smaller than wanted, drive forwards
            driveForward(SLOW_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
    }
}
/**************************************
 * Check for the heading of the robot *
 **************************************/
void checkHeading(float heading, bool fire) {
    //Check if the heading being looked for is 0 degrees
    if (!fire && (-1 < heading && heading < 1)) {
        heading += 180;
        while((RPS.Heading() + 180) < heading - .5 || (RPS.Heading() +  180) > heading + .5) {
            if((RPS.Heading() + 180) > heading) {
                //If the heading is greater than wanted, turn right
                turnRight(STD_DRIVE, 0, 1);
            } else if((RPS.Heading() + 180) < heading) {
                //If the heading is smaller than wanted, turn left
                turnLeft(STD_DRIVE, 0, 1);
            }
            Sleep(SHORT_SLEEP);
        }
    }
    //Check if the heading is already correct
    while(!fire && (RPS.Heading() < heading - .5 || RPS.Heading() > heading + .5)) {
        if(RPS.Heading() > heading) {
            //If the heading is greater than wanted, turn right
            turnRight(STD_DRIVE, 0, 1);
        } else if(RPS.Heading() < heading) {
            //If the heading is smaller than wanted, turn left
            turnLeft(STD_DRIVE, 0, 1);
        }
        Sleep(SHORT_SLEEP);
    }
}
/****************************
 * Hit the buttons in order *
 ****************************/
void buttonsOrder(bool fire)
{
    int red = 1, blue = 2;
    if (!fire) {
        red = RPS.RedButtonOrder();
        blue = RPS.BlueButtonOrder();
    }
    int time = TimeNow();
    //int white = RPS.WhiteBurronOrder();
    while (!(RPS.BlueButtonPressed() && RPS.RedButtonPressed() && RPS.WhiteButtonPressed()) && TimeNow() - time < 10) {
        if (red == 1) {
            //Press red
            disk.SetDegree(RED_ANGLE);
            driveForward(STD_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            timedDrive(-STD_DRIVE, .5);
            Sleep(SLEEP_TIME);
            if (blue == 2) {
                //Press blue
                disk.SetDegree(BLUE_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
                //Press white
                disk.SetDegree(WHITE_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
            }
            else {
                //Press white
                disk.SetDegree(WHITE_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
                //Press blue
                disk.SetDegree(BLUE_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
            }
        }
        else if (blue == 1) {
            //Press blue
            disk.SetDegree(BLUE_ANGLE);
            driveForward(STD_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            timedDrive(-STD_DRIVE, .5);
            Sleep(SLEEP_TIME);
            if (red == 2) {
                //Press red
                disk.SetDegree(RED_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
                //Press white
                disk.SetDegree(WHITE_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
            }
            else {
                //Press white
                disk.SetDegree(WHITE_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
                //Press red
                disk.SetDegree(RED_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
            }
        }
        else {
            //Press white
            disk.SetDegree(WHITE_ANGLE);
            driveForward(STD_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            timedDrive(-STD_DRIVE, .5);
            Sleep(SLEEP_TIME);
            if (blue == 2) {
                //Press blue
                disk.SetDegree(BLUE_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
                //Press red
                disk.SetDegree(RED_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
            }
            else {
                //Press red
                disk.SetDegree(RED_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
                //Press blue
                disk.SetDegree(BLUE_ANGLE);
                driveForward(STD_DRIVE, 2, 0);
                Sleep(SLEEP_TIME);
                timedDrive(-STD_DRIVE, .5);
                Sleep(SLEEP_TIME);
            }
        }
        Sleep(200);
    }
    LCD.WriteLine(RPS.ButtonsPressed());
}
/*******************************************
 * Turns the crank in the proper direction *
 *******************************************/
void crankDirection() {
    //Crank needs turned Right (CW)
    if (CdS.Value() < CDS_CRANK) {
        LCD.WriteLine("CW (RED)");
        for (int i = 0; i < 4; i++) {
            right_motor.SetPercent(SLOW_DRIVE+7);
            left_motor.SetPercent(-SLOW_DRIVE-7);
            disk.SetDegree(SERVO_HMAX);
            Sleep(SLEEP_TIME + 300);
            right_motor.Stop();
            left_motor.Stop();
            driveForward(STD_DRIVE, 1, 0);
            Sleep(SLEEP_TIME);
            disk.SetDegree(SERVO_SMIN);
            Sleep(SLEEP_TIME);
            timedDrive(-STD_DRIVE, .5);
            Sleep(SLEEP_TIME);
        }
    } else {     //Crank needs turned Left (CCW)
        LCD.WriteLine("CCW (BLUE)");
        for (int i = 0; i < 4; i++) {
            right_motor.SetPercent(SLOW_DRIVE+7);
            left_motor.SetPercent(-SLOW_DRIVE-7);
            disk.SetDegree(SERVO_SMIN);
            Sleep(SLEEP_TIME + 300);
            right_motor.Stop();
            left_motor.Stop();
            driveForward(STD_DRIVE, 1, 0);
            Sleep(SLEEP_TIME);
            disk.SetDegree(SERVO_HMAX);
            Sleep(SLEEP_TIME);
            timedDrive(-STD_DRIVE, .5);
            Sleep(SLEEP_TIME);
        }
    } /*else {
        LCD.WriteLine("The CdS cell cannot read the light color.");
    }*/
    Sleep(SLEEP_TIME);
}
/****************************
 * Drive into the oil lever *
 ****************************/
void oilRun() {
    while(RPS.OilPress() == 0) {
        if (RPS.OilDirec() == 1) {
            //Push the lever mechanism down
            disk.SetDegree(SERVO_HMIN);
            Sleep(SLEEP_TIME*2);
            //Drive into the lever
            timedDrive(FAST_DRIVE, 2.0);
            turnLeft(FAST_DRIVE, 20, 0);
        } else if (RPS.OilDirec() == 0) {
            //Drive past the lever
            driveForward(STD_DRIVE, 10, 0);
            timedDrive(STD_DRIVE, .8);
            //Push the lever mechanism down
            disk.SetDegree(SERVO_HMIN);
            Sleep(SLEEP_TIME*2);
            //Drive into the lever
            timedDrive(-FAST_DRIVE, .5);
            turnRight(FAST_DRIVE, 20, 0);
        }
    }
}
/********************
 * Get the salt bag *
 ********************/
void saltRun(bool RPSFire) {
    //Check Initial Heading
    checkHeading(180, RPSFire);
    //Drive Forward to first salt bag position
    driveForward(STD_DRIVE, 10, 20);
    driveForward(SLOW_DRIVE, 2, 0);
    checkYMinus(POINT_A, RPSFire);
    Sleep(SLEEP_TIME);
    //Turn towards the salt bag
    turnLeft(STD_DRIVE, 42, 0);
    checkHeading(222, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive towards the salt bag
    driveForward(STD_DRIVE, 10, 0);
    Sleep(SHORT_SLEEP);
    checkHeading(222, RPSFire);
    for (int i = 0; i < 2; i++) {
        //Drive into the salt bag
        timedDrive(SLOW_DRIVE, 1.0);
        timedDrive(STD_DRIVE, 0.5);
        timedDrive(FAST_DRIVE, 1.5 );
        Sleep(SLEEP_TIME);
        //Drive back from the salt bag
        driveForward(-STD_DRIVE, 5, 0);
        Sleep(SLEEP_TIME);
    }
}
/**********************
 * Drive to the crank *
 **********************/
void crankRun (bool RPSFire) {
    //Turn towards the ramp wall
    turnRight(STD_DRIVE, 80, 0);
    checkHeading(140, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive towards the ramp wall
    driveForward(-STD_DRIVE, 9, 20);
    checkXMinus(POINT_F, RPSFire);
    Sleep(SLEEP_TIME);
    //Turn towards the ramp
    turnLeft(STD_DRIVE, 30, 0);
    checkHeading(180, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive up the ramp
    driveForward(-FAST_DRIVE, 32, 0);
    driveForward(-STD_DRIVE, 2, 0);
    driveForward(-SLOW_DRIVE, 1, 0);
    checkYMinus(POINT_C, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive to the light and set disk to proper angle
    checkHeading(180, RPSFire);
    disk.SetDegree(108);
    timedDrive(-STD_DRIVE, 1.5);
    Sleep(SLEEP_TIME);
}
/*************************
 * Drive into the garage *
 *************************/
void garageRun(bool RPSFire) {
    //Drive back a little and check heading
    driveForward(STD_DRIVE, 1, 0);
    checkHeading(180, RPSFire);
    Sleep(SHORT_SLEEP);
    //Drive backwards from the crank
    driveForward(STD_DRIVE, 6, 25);
    checkYMinus(POINT_H, RPSFire);
    Sleep(SLEEP_TIME);
    //Turn towards the snow pile
    turnRight(STD_DRIVE, 100, 0);
    checkHeading(80.7, RPSFire);
    Sleep(SLEEP_TIME);
    //Plow the snow forward
    timedDrive(STD_DRIVE, 2.5);
    Sleep(SLEEP_TIME);
    //Pull back from snow
    driveForward(-STD_DRIVE, 2, 20);
    checkXMinus(POINT_J, RPSFire);
    Sleep(SLEEP_TIME);
    //Turn towards the garage
    turnRight(STD_DRIVE, 34, 0);
    checkHeading(46, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive into the garage
    timedDrive(FAST_DRIVE, 1.5);
    //checkXMinus(POINT_K, RPSFire);
    Sleep(SLEEP_TIME);
}
/************************
 * Drive to the buttons *
 ************************/
void buttonsRun(bool RPSFire) {
    //Drive back from the garage
    driveForward(-STD_DRIVE, 2, 0);
    Sleep(SLEEP_TIME);
    //Turn towards the positive-y wall
    turnLeft(STD_DRIVE, 90, 0);
    checkHeading(132, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive forward
    driveForward(-STD_DRIVE, 6, 10);
    checkYMinus(POINT_L, RPSFire);
    Sleep(SLEEP_TIME);
    //Turn towards the buttons
    turnLeft(STD_DRIVE, 90, 0);
    checkHeading(222, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive towards the buttons
    driveForward(-STD_DRIVE, 2, 0);
    checkXPlus(POINT_M, RPSFire);
    Sleep(SLEEP_TIME);
}
/**********************************
 * Drive back to the bottom level *
 **********************************/
void bottomRun(bool RPSFire) {
    //Back up
    driveForward(STD_DRIVE, 13, 0);
    Sleep(SLEEP_TIME);
    //Turn back towards the wall
    turnRight(STD_DRIVE, 100, 0);
    checkHeading(90, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive towards the wall
    driveForward(-STD_DRIVE, 11, 0);
    checkXMinus(POINT_F, RPSFire);
    Sleep(SLEEP_TIME);
    //Turn toward the ramp
    turnLeft(STD_DRIVE, 90, 0);
    checkHeading(180, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive down the ramp
    driveForward(STD_DRIVE, 30, 0);
    checkYMinus(POINT_N, RPSFire);
    Sleep(SLEEP_TIME);
    //Turn towards negative-x wall
    turnRight(STD_DRIVE, 90, 0);
    checkHeading(90, RPSFire);
    Sleep(SLEEP_TIME);
    //Drive towards the negative-x wall
    driveForward(STD_DRIVE, 11, 30);
    checkXMinus(POINT_O, RPSFire);
    Sleep(SLEEP_TIME);
    //Turn towards the negative-y wall
    turnLeft(STD_DRIVE, 90, 0);
    checkHeading(180, RPSFire);
    Sleep(SLEEP_TIME);
}
