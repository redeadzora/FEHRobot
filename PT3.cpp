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

#define COUNTS_PER_INCH 40
#define COUNTS_PER_DEGREE 233/90
#define POINT_A 23
#define POINT_B 17
#define POINT_C 10
#define POINT_D 43

ButtonBoard buttons(FEHIO::Bank3);
FEHMotor right_motor( FEHMotor::Motor0);
FEHMotor left_motor( FEHMotor::Motor1);
DigitalEncoder right_encoder( FEHIO::P0_0);
DigitalEncoder left_encoder( FEHIO::P0_2);
AnalogInputPin CDS( FEHIO::P1_0);
DigitalInputPin right_bump( FEHIO::P1_2);

void turnRight(int percent, int degrees);
void turnLeft(int percent, int degrees);
void driveForward(int percent, int inches, int inputCounts);
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
        while(CDS.Value()>.6);
        LCD.Clear( FEHLCD::White );
        driveForward(25, 13, 0);
        checkYMinus(POINT_A);
        Sleep(.5);
        turnLeft(25, 233);
        checkHeading(90);
        Sleep(.5);
        driveForward(25, 11, 0);
        checkXPlus(POINT_B);
        Sleep(.5);
        turnLeft(25, 210);
        checkHeading(180);
        Sleep(.5);
        driveForward(50, 30, 0);
        checkYPlus(POINT_C);
        Sleep(.5);
        turnLeft(25, 233);
        checkHeading(270);
        Sleep(.5);
        driveForward(25, 20, 0);
        checkXMinus(POINT_D);
    }
    /*while(true) {
        LCD.WriteLine(CDS.Value());
        Sleep(.2);
    }*/
}

void turnRight(int percent, int degrees) //using encoders
{
    //Make input degrees and convert degrees to counts
    counts = degrees*COUNTS_PER_DEGREE;
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

void turnLeft(int percent, int degrees) //using encoders
{   
    //Make input degrees and convert degrees to counts
    counts = degrees*COUNTS_PER_DEGREE;
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
void driveForward(int percent, int inches, int inputCounts) {
    int counts = inches*COUNTS_PER_INCH + inputCounts;
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
