/*
 *This is the program that will be loaded onto the
 *Proteus to complete Performance Test 3.
 * ROBOT NOTE:
 * The left wheel is correct, but the right wheel is backwards.
 */

#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#include <FEHServo.h>
//The number of counts when an inch is travelled
#define COUNTS_PER_INCH 40
//The number of counts when 1 degree is turned
#define COUNTS_PER_DEGREE 233/90
//The point South of the starting zone
#define POINT_A 23
//The point East of POINT_A
#define POINT_B 17
//The point North of POINT_B
#define POINT_C 10
//The point West of POINT_C
#define POINT_D 43
//The standard driving percentage
#define STD_DRIVE 25
//The fast driving percentage
#define FAST_DRIVE 50
//The slow driving percentage
#define SLOW_DRIVE 12
//The max driving speed. Only for use in extreme circumstances
#define HYPER_DRIVE 100
//The standard sleep time
#define SLEEP_TIME 500
//The CdS Threshold
#define CDS_THRESHOLD .6
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
void turnRight(int percent, int degrees, int input_counts);
void turnLeft(int percent, int degrees int input_counts);
void driveForward(int percent, int inches, int input_counts);
void checkXPlus(float x_coordinate);
void checkXMinus(float x_coordinate);
void checkYPlus(float y_coordinate);
void checkYMinus(float y_coordinate);
void checkHeading(float heading);

FEHMotor right( FEHMotor::Motor0);
FEHMotor left( FEHMotor::Motor1);
int main(void)
{
    LCD.Clear( FEHLCD::White );
    LCD.SetFontColor( FEHLCD::Black );
    while(true) {
        LCD.WriteLine("Press the middle button to begin");
        while(!buttons.MiddlePressed());
        while(buttons.MiddlePressed());
        LCD.WriteLine("Will start on light now");
        while(CdS.Value()>CDS_THRESHOLD);
        LCD.Clear( FEHLCD::White );
        driveForward(STD_DRIVE, 13, 0);
        checkYMinus(POINT_A);
        Sleep(SLEEP_TIME);
        turnLeft(STD_DRIVE, 90, 0);
        checkHeading(90);
        Sleep(SLEEP_TIME);
        driveForward(STD_DRIVE, 11, 0);
        checkXPlus(POINT_B);
        Sleep(SLEEP_TIME);
        turnLeft(STD_DRIVE, 90, 0);
        checkHeading(180);
        Sleep(SLEEP_TIME);
        driveForward(FAST_DRIVE, 30, 0);
        checkYPlus(POINT_C);
        Sleep(SLEEP_TIME);
        turnLeft(STD_DRIVE, 90, 0);
        checkHeading(270);
        Sleep(SLEEP_TIME);
        driveForward(STD_DRIVE, 20, 0);
        checkXMinus(POINT_D);
    }
    /*while(true) {
        LCD.WriteLine(CDS.Value());
        Sleep(.2);
    }*/
}

void turnRight(int percent, int degrees, int input_counts) //using encoders
{
    //Make input degrees and convert degrees to counts
    counts = degrees*COUNTS_PER_DEGREE + input_counts;
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

void turnLeft(int percent, int degrees, int input_counts) //using encoders
{   
    //Make input degrees and convert degrees to counts
    counts = degrees*COUNTS_PER_DEGREE + input_counts;
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
void driveForward(int percent, int inches, int input_counts) {
    int counts = inches*COUNTS_PER_INCH + input_counts;
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(percent);
    while ((left_encoder.Counts() + right_encoder.Counts())/2.<counts);
    right_motor.Stop();
    left_motor.Stop();
}
void checkXPlus(float x_coordinate) //using RPS while robot is in the +x direction
{
    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            move_forward(-25, 2);

        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            move_forward(25, 2);
        }
        Sleep(.05);
    }
}

void checkXMinus(float x_coordinate) //using RPS while robot is in the -x direction
{
    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            move_forward(25, 2);

        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            move_forward(-25, 2);
        }
        Sleep(.05);
    }
}

void checkYMinus(float y_coordinate) //using RPS while robot is in the -y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            move_forward(25, 2);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            move_forward(-25, 2);
        }
        Sleep(.05);
    }
}

void checkYPlus(float y_coordinate) //using RPS while robot is in the +y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            move_forward(-25, 2);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            move_forward(25, 2);
        }
        Sleep(.05);
    }
}

void checkHeading(float heading) //using RPS
{
    //Check if the heading is in acceptable range
    while(RPS.Heading() < heading - 1 || RPS.Heading() > heading + 1)
    {
        if(RPS.Heading() > heading)
        {
            //pulse the motors for a short duration in the correct direction

            turn_right(25, 1);
        }
        else if(RPS.Y() < heading)
        {
            //pulse the motors for a short duration in the correct direction

            turn_left(25, 1);
        }
        Sleep(.05);
    }
}
