#include "../../include/WallDetectingRunner.h"

void WallDetectingRunnger::turn(int angle)
{
    if (angle > 0)
    { // Turn right
        motorLeft.run(-MOTOR_SPEED);
        motorRight.run(-MOTOR_SPEED);
    }
    else
    { // Turn left
        motorLeft.run(MOTOR_SPEED);
        motorRight.run(MOTOR_SPEED);
    }
    const float turnDurationMultiplier = 6.75;
    delay(abs(angle) * turnDurationMultiplier);
    motorLeft.run(0);
    motorRight.run(0);
}

void WallDetectingRunnger::moveForward(float feet)
{
    float initialDistance = ultrasonicSensor.distanceInch();
    float targetDistance = initialDistance - (12.0 * feet);

    while (ultrasonicSensor.distanceInch() > targetDistance)
    {
        motorLeft.run(-MOTOR_SPEED);
        motorRight.run(MOTOR_SPEED);
        delay(50);
    }

    motorLeft.run(0);
    motorRight.run(0);
}

void WallDetectingRunnger::stop()
{
    motorLeft.run(0);
    motorRight.run(0);
}

void WallDetectingRunnger::checkCenter()
{
    float bodylength = 6.5;
    turn(90);
    if (ultrasonicSensor.distanceInch() > 12)
    {
        turn(-90);
        return;
    }
    while (ultrasonicSensor.distanceInch() < (12 - bodylength) / 2)
    {
        motorLeft.run(MOTOR_SPEED);
        motorRight.run(-MOTOR_SPEED);
        delay(50);
    }
    while (ultrasonicSensor.distanceInch() > (12 - bodylength) / 2)
    {
        motorLeft.run(-MOTOR_SPEED);
        motorRight.run(MOTOR_SPEED);
        delay(50);
    }
    turn(-90);
    stop();
}

void WallDetectingRunnger::runMaze()
{
    checkCenter();
    moveForward(1.0);
    checkCenter();
    turn(90);
    moveForward(2.0);
    checkCenter();
    turn(-90);
    moveForward(0.6);
    checkCenter();
    stop();
    buzzer.tone(500, 1000);
}