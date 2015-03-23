/*
 *This is the program that will be loaded onto the
 *Proteus to complete Performance Test 5.
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
//The number of counts when an inch is travelled
#define COUNTS_PER_INCH 40
//The number of counts when 1 degree is turned
#define COUNTS_PER_DEGREE 233/90
//The point South of the starting zone
#define POINT_A 19.6
//The point at the bottom of the ramp
#define POINT_B 28.5
//The point at the top of the ramp
#define POINT_C 52.1
//The point at the crank
#define POINT_D 57.7
//Point E currently unused
#define POINT_E 24
//The point by the ramp wall
#define POINT_F 29.7
//The point at the top of the ramp
#define POINT_G 43.4
//The point in front of the top of the ramp
#define POINT_H 48.5
//The point the snow is being plowed to
#define POINT_I 10.2
//The point before the garage
#define POINT_J 12.3
//The point at the garage
#define POINT_K 6.3
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
#define CDS_THRESHOLD .4
//The CdS Red Value
#define CDS_RED .223
//The CdS Blue Value
#define CDS_BLUE .347
//The CdS light off value
#define CDS_OFF .89
//The angle to press the red button
#define RED_ANGLE 90
//The angle to press the blue button
#define BLUE_ANGLE 90
//The angle to press the white button
#define WHITE_ANGLE 90
//Servo motor minimum
#define SERVO_MIN 510
//Servo motor maximum
#define SERVO_MAX 2260
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
int crankDirection();

int main(void)
{
    LCD.Clear( FEHLCD::White );
    LCD.SetFontColor( FEHLCD::Black );
    //Set servo limits
    disk.SetMin(SERVO_MIN);
    disk.SetMax(SERVO_MAX);
    disk.SetDegree(0);
    RPS.InitializeMenu();
    LCD.WriteLine("Press the middle button to begin");
    while(true) {
        while(!buttons.MiddlePressed());
        while(buttons.MiddlePressed());
        LCD.WriteLine("Will start on light now");
        while(CdS.Value()>CDS_THRESHOLD) {
            LCD.WriteRC(CdS.Value(), 5, 12);
            Sleep(1.0);
        }
        LCD.Clear( FEHLCD::White );
        //Check Initial Heading
        checkHeading(180);
        //Drive to the first forward position
        driveForward(STD_DRIVE, 14, 0);
        checkYMinus(POINT_A);
        Sleep(SLEEP_TIME);
        //Turn towards the wall
        turnRight(STD_DRIVE, 90, 0);
        checkHeading(90);
        Sleep(SLEEP_TIME);
        //Drive to the wall
        driveForward(-STD_DRIVE, 12, 0);
        checkXMinus(POINT_B);
        Sleep(SLEEP_TIME);
        //Turn towards the ramp
        turnLeft(STD_DRIVE, 90, 0);
        checkHeading(180);
        Sleep(SLEEP_TIME);
        //Drive up the ramp
        driveForward(-FAST_DRIVE, 36, 0);
        driveForward(-STD_DRIVE, 2, 0);
        driveForward(-SLOW_DRIVE, 1, 0);
        checkYMinus(POINT_C);
        Sleep(SLEEP_TIME);
        //Drive to the light
        driveForward(-STD_DRIVE, 5, 0);
        checkYMinus(POINT_D);
        Sleep(SLEEP_TIME);
        //Check light value
        int direction = crankDirection();
        Sleep(SLEEP_TIME);
        //Turn the crank the proper direction
        if(direction == 1) { //CW
            for (int i = 0; i < 3; i++) {
                LCD.WriteLine("CW (RED)");
                disk.SetDegree(0);
                Sleep(SLEEP_TIME);
                driveForward(STD_DRIVE, 1, 0);
                Sleep(SLEEP_TIME);
                disk.SetDegree(180);
                Sleep(SLEEP_TIME);
                driveForward(-STD_DRIVE, 1, 0);
                Sleep(SLEEP_TIME);
            }
        } else if (direction == 2) { //CCW
            for (int i = 0; i < 3; i++) {
                LCD.WriteLine("CCW (BLUE)");
                disk.SetDegree(180);
                Sleep(SLEEP_TIME);
                driveForward(STD_DRIVE, 1, 0);
                Sleep(SLEEP_TIME);
                disk.SetDegree(0);
                Sleep(SLEEP_TIME);
                driveForward(-STD_DRIVE, 1, 0);
                Sleep(SLEEP_TIME);
            }
        } else if (direction == 0) {
            LCD.WriteLine("The CdS cell cannot read the light color.");

        }
        while(true) {
            LCD.WriteRC(CdS.Value(), 5, 12);
            Sleep(1.0);
        }
        /*
        //Drive Forward to first salt bag position
        driveForward(STD_DRIVE, 13, 0);
        checkYMinus(POINT_A);
        Sleep(SLEEP_TIME);
        //Turn towards the salt bag
        turnLeft(STD_DRIVE, 42, 0);
        checkHeading(222);
        Sleep(SLEEP_TIME);
        //Drive towards the salt bag
        driveForward(STD_DRIVE, 10, 0);
        Sleep(SHORT_SLEEP);
        for (int i = 0; i < 2; i++) {
            //Drive into the salt bag
            driveForward(FAST_DRIVE, 5, 0);
            Sleep(SLEEP_TIME);
            //Drive back from the salt bag
            driveForward(-STD_DRIVE, 5, 0);
            Sleep(SLEEP_TIME);
        }
        //Turn towards the ramp wall
        turnRight(STD_DRIVE, 80, 0);
        checkHeading(140);
        Sleep(SLEEP_TIME);
        //Drive towards the ramp wall
        driveForward(-STD_DRIVE, 10, 20);
        checkXMinus(POINT_F);
        Sleep(SLEEP_TIME);
        //Turn towards the ramp
        turnLeft(STD_DRIVE, 30, 0);
        checkHeading(180);
        Sleep(SLEEP_TIME);
        //Drive up the ramp
        driveForward(-FAST_DRIVE, 26, 0);
        driveForward(-STD_DRIVE, 2, 0);
        checkYMinus(POINT_G);
        Sleep(SLEEP_TIME);
        //Drive forward from the ramp
        driveForward(-STD_DRIVE, 5, 0);
        checkYMinus(POINT_H);
        Sleep(SLEEP_TIME);
        //Turn towards the snow pile
        turnRight(STD_DRIVE, 100, 0);
        checkHeading(80.7);
        Sleep(SLEEP_TIME);
        //Plow the snow forward
        driveForward(STD_DRIVE, 20, 0);
        checkXMinus(POINT_I);
        Sleep(SLEEP_TIME);
        //Pull back from snow
        driveForward(-STD_DRIVE, 2, 20);
        checkXMinus(POINT_J);
        Sleep(SLEEP_TIME);
        //Turn towards the garage
        turnRight(STD_DRIVE, 34, 0);
        checkHeading(46);
        Sleep(SLEEP_TIME);
        //Drive into the garage
        driveForward(FAST_DRIVE, 8, 20);
        //checkXMinus(POINT_K);
        Sleep(SLEEP_TIME);
        //Drive back from the garage
        driveForward(-STD_DRIVE, 6, 0);
        Sleep(SLEEP_TIME);
        //Turn back towards the wall
        turnLeft(STD_DRIVE, 30, 0);
        checkHeading(90);
        Sleep(SLEEP_TIME);
        //Drive towards the wall
        driveForward(-STD_DRIVE, 20, 0);
        checkXMinus(POINT_F);
        Sleep(SLEEP_TIME);
        //Turn toward the ramp
        turnLeft(STD_DRIVE, 90, 0);
        checkHeading(180);
        Sleep(SLEEP_TIME);
        //Drive down the ramp
        driveForward(STD_DRIVE, 20, 0);
        checkYMinus(21.8);
        Sleep(SLEEP_TIME);
        //Turn towards the lever
        turnRight(STD_DRIVE, 55, 0);
        checkHeading(126);
        Sleep(SLEEP_TIME);
        driveForward(STD_DRIVE, 10, 0);
        checkXMinus(21);
        turnRight(STD_DRIVE, 180, 0);
        //checkHeading(270);
        //Drive into the lever
        driveForward(-STD_DRIVE, 13, 0);
        Sleep(SLEEP_TIME);*/
        }
    //Use this to get RPS readings and CdS Readings
   /* while(true) {
        LCD.Clear();
        LCD.Write("X: ");
        LCD.WriteLine(RPS.X());
        LCD.Write("Y: ");
        LCD.WriteLine(RPS.Y());
        LCD.Write("Heading: ");
        LCD.WriteLine(RPS.Heading());
        LCD.Write("CdS: ");
        LCD.WriteLine(CdS.Value());
        Sleep(SLEEP_TIME);
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
    /*if (percent > 0) {
        left_motor.SetPercent(percent + 2);
    } else if (percent < 0) {
        left_motor.SetPercent(percent - 2);
    }*/
    while ((left_encoder.Counts() + right_encoder.Counts())/2.<counts);
    right_motor.Stop();
    left_motor.Stop();
}
void checkXPlus(float x_coordinate) //using RPS while robot is in the +x direction
{
    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - .5 || RPS.X() > x_coordinate + .5)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            driveForward(-SLOW_DRIVE, 0, 2);

        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            driveForward(SLOW_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
    }
}

void checkXMinus(float x_coordinate) //using RPS while robot is in the -x direction
{
    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - .5|| RPS.X() > x_coordinate + .5)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            driveForward(SLOW_DRIVE, 0, 2);

        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
            driveForward(-SLOW_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
    }
}

void checkYMinus(float y_coordinate) //using RPS while robot is in the -y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - .5 || RPS.Y() > y_coordinate + .5)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            driveForward(SLOW_DRIVE, 0, 2);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            driveForward(-SLOW_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
    }
}

void checkYPlus(float y_coordinate) //using RPS while robot is in the +y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - .5 || RPS.Y() > y_coordinate + .5)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            driveForward(-SLOW_DRIVE, 0, 2);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            driveForward(SLOW_DRIVE, 0, 2);
        }
        Sleep(SHORT_SLEEP);
    }
}

void checkHeading(float heading) //using RPS
{
    if (-1 < heading && heading < 1) {
        heading += 180;
        while((RPS.Heading() + 180) < heading - .5 || (RPS.Heading() +  180) > heading + .5)
        {
            if((RPS.Heading() + 180) > heading)
            {
                //pulse the motors for a short duration in the correct direction

                turnRight(STD_DRIVE, 0, 1);
            }
            else if((RPS.Heading() + 180) < heading)
            {
                //pulse the motors for a short duration in the correct direction

                turnLeft(STD_DRIVE, 0, 1);
            }
            Sleep(SHORT_SLEEP);
        }
    }
    //Check if the heading is in acceptable range
    while(RPS.Heading() < heading - .5 || RPS.Heading() > heading + .5)
    {
        if(RPS.Heading() > heading)
        {
            //pulse the motors for a short duration in the correct direction

            turnRight(STD_DRIVE, 0, 1);
        }
        else if(RPS.Heading() < heading)
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
int crankDirection() {
    //Crank needs turned Right (CW)
    if (CdS.Value() < 1.35) {
        return 1;
    } else {     //Crank needs turned Left (CCW)

        return 2;
    } /*else {
        return 0;
    }*/
}
