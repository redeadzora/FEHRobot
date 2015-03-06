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
#define POINT_A 18
//The point East of POINT_A
#define POINT_B 31.3
//The point North of POINT_B
#define POINT_C 52.1
//The point West of POINT_C
#define POINT_D 63.7
//The standard driving percentage
#define STD_DRIVE 25
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
#define CDS_THRESHOLD .6
//The angle to press the red button
#define RED_ANGLE 90
//The angle to press the blue button
#define BLUE_ANGLE 90
//The angle to press the white button
#define WHITE_ANGLE 90
//Servo motor minimum
#define SERVO_MIN 510
//Servo motor maximum
#define SERVO_MAX 2280
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
void turnLeft(int percent, int degrees, int input_counts);
void driveForward(int percent, int inches, int input_counts);
void checkXPlus(float x_coordinate);
void checkXMinus(float x_coordinate);
void checkYPlus(float y_coordinate);
void checkYMinus(float y_coordinate);
void checkHeading(float heading);
void buttonsOrder();

int main(void)
{
    LCD.Clear( FEHLCD::White );
    LCD.SetFontColor( FEHLCD::Black );
    //Set servo limits
    disk.SetMin(SERVO_MIN);
    disk.SetMax(SERVO_MAX);
    disk.SetDegree(20);
    RPS.InitializeMenu();
    while(true) {
        LCD.WriteLine("Press the middle button to begin");
        while(!buttons.MiddlePressed());
        while(buttons.MiddlePressed());
        //LCD.WriteLine("Will start on light now");
        //while(CdS.Value()>CDS_THRESHOLD);
        //LCD.Clear( FEHLCD::White );
       // driveForward(STD_DRIVE, 13, 0);
        //checkYMinus(POINT_A);
        //Sleep(SLEEP_TIME);
        //turnLeft(STD_DRIVE, 90, 0);
        //checkHeading(90);
        //Sleep(SLEEP_TIME);
        //driveForward(STD_DRIVE, 11, -50);
        //checkXPlus(POINT_B);
        //Sleep(SLEEP_TIME);
       //turnLeft(STD_DRIVE, 90, 0);
        //checkHeading(180);
        Sleep(SLEEP_TIME);
        driveForward(FAST_DRIVE, 35, 0);
        driveForward(STD_DRIVE, 2, 0);
        driveForward(SLOW_DRIVE, 1, 0);
        checkYPlus(POINT_C);
        Sleep(SLEEP_TIME);
        turnLeft(STD_DRIVE, 56, 0);
        checkHeading(236);
        Sleep(SLEEP_TIME);
        driveForward(STD_DRIVE, 20, 0);
        checkYPlus(POINT_D);
        turnRight(STD_DRIVE, 15, 0);
        checkHeading(221.5);
        //buttonsOrder();
        //RAM THAT MOFO
        right_motor.SetPercent(-FAST_DRIVE);
        left_motor.SetPercent(FAST_DRIVE);
        Sleep(2.0);
        driveForward(-STD_DRIVE, 1, 0);
        Sleep(1.0);
        right_motor.SetPercent(-FAST_DRIVE);
        left_motor.SetPercent(FAST_DRIVE);
        Sleep(2.0);
        driveForward(-STD_DRIVE, 1, 0);
        right_motor.SetPercent(-FAST_DRIVE);
        left_motor.SetPercent(FAST_DRIVE);
        Sleep(2.0);
        driveForward(-STD_DRIVE, 1, 0);
        Sleep(1.0);
        right_motor.SetPercent(-FAST_DRIVE);
        left_motor.SetPercent(FAST_DRIVE);
        Sleep(2.0);
        driveForward(-STD_DRIVE, 1, 0);
        Sleep(1.0);
    }
    /*while(true) {
        LCD.WriteLine(CDS.Value());
        Sleep(.2);
    }*/
}

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
            driveForward(-STD_DRIVE, 0, 2);

        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            driveForward(STD_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
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
            driveForward(STD_DRIVE, 0, 2);

        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            driveForward(-STD_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
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

            driveForward(STD_DRIVE, 0, 2);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            driveForward(-STD_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
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

            driveForward(-STD_DRIVE, 0, 2);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            driveForward(STD_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
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

            turnRight(STD_DRIVE, 0, 1);
        }
        else if(RPS.Y() < heading)
        {
            //pulse the motors for a short duration in the correct direction

            turnLeft(STD_DRIVE, 0, 1);
        }
        Sleep(SHORT_SLEEP);
    }
}

void buttonsOrder()
{
    int red = RPS.RedButtonOrder();
    int blue = RPS.BlueButtonOrder();
    //int white = RPS.WhiteBurronOrder();
    if (red == 1) {
        if (blue == 2) {
            //Press red
            disk.SetDegree(RED_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press blue
            disk.SetDegree(BLUE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press white
            disk.SetDegree(WHITE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
        }
        else {
            //Press red
            disk.SetDegree(RED_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press white
            disk.SetDegree(WHITE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press blue
            disk.SetDegree(BLUE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
        }
    }
    else if (blue == 1) {
        if (red == 2) {
            //Press blue
            disk.SetDegree(BLUE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press red
            disk.SetDegree(RED_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press white
            disk.SetDegree(WHITE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
        }
        else {
            //Press blue
            disk.SetDegree(BLUE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press white
            disk.SetDegree(WHITE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press red
            disk.SetDegree(RED_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
        }
    }
    else {
        if (blue == 2) {
            //Press white
            disk.SetDegree(WHITE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press blue
            disk.SetDegree(BLUE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press red
            disk.SetDegree(RED_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
        }
        else {
            //Press white
            disk.SetDegree(WHITE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press red
            disk.SetDegree(RED_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
            //Press blue
            disk.SetDegree(BLUE_ANGLE);
            driveForward(SLOW_DRIVE, 2, 0);
            Sleep(SLEEP_TIME);
            driveForward(-SLOW_DRIVE, 2, 0);
        }
    }
}
