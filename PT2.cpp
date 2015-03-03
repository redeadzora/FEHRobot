#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>

#define COUNTS_PER_INCH 40
#define COUNTS_PER_DEGREE 0

ButtonBoard buttons(FEHIO::Bank3);
FEHMotor right_motor( FEHMotor::Motor0);
FEHMotor left_motor( FEHMotor::Motor1);
DigitalEncoder right_encoder( FEHIO::P0_0);
DigitalEncoder left_encoder( FEHIO::P0_2);
AnalogInputPin CDS( FEHIO::P1_0);
DigitalInputPin right_bump( FEHIO::P1_2);

void turn_right(int percent, int counts) //using encoders
{
    //Make input degrees and convert degrees to counts
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

void turn_left(int percent, int counts) //using encoders
{
    //Make input degrees and convert degrees to counts
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
void driveForward(int percent, int inches) {
    int counts = inches*COUNTS_PER_INCH;
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();
    right_motor.SetPercent(-percent-.5);
    left_motor.SetPercent(percent);
    while ((left_encoder.Counts() + right_encoder.Counts())/2.<counts);
    right_motor.Stop();
    left_motor.Stop();
}
FEHMotor right( FEHMotor::Motor0);
FEHMotor left( FEHMotor::Motor1);
//The left wheel is correct, but the right wheel is backwards
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
        driveForward(25,13);
        Sleep(.5);
        turn_left(25, 233);
        Sleep(.5);
        driveForward(25, 11);
        Sleep(.5);
        turn_left(25, 210);
        Sleep(.5);
        driveForward(50, 45);
    }
    while(true) {
        LCD.WriteLine(CDS.Value());
        Sleep(.2);
    }
    LCD.WriteLine(right_bump.Value());
}
