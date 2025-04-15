#ifndef MAZE_RUNNER_H
#define MAZE_RUNNER_H

#include <MeMCore.h>

class MazeRunner {
protected:
    MeDCMotor& motorLeft;
    MeDCMotor& motorRight;
    MeUltrasonicSensor& ultrasonicSensor;
    MeLineFollower& lineFollower;
    MeBuzzer& buzzer;
    MeRGBLed& led;

public:
    MazeRunner(MeDCMotor& left, MeDCMotor& right, MeUltrasonicSensor& sensor, 
               MeLineFollower& lineFollower, MeBuzzer& buz, MeRGBLed& rgbled)
        : motorLeft(left), motorRight(right), ultrasonicSensor(sensor), 
          lineFollower(lineFollower), buzzer(buz), led(rgbled) {}
    
    virtual ~MazeRunner() = default;
    virtual void runMaze() = 0;
    
protected:
    void stop() {
        motorLeft.run(0);
        motorRight.run(0);
    }  

    void setLed(int r, int g, int b) {
        led.setColor(0, r, g, b); // Set left LED to the specified color
        led.setColor(1, r, g, b); // Set right LED to the specified color
        led.show();
    }

    void setLed(int index, int r, int g, int b) {
        led.setColor(index, r, g, b);
        led.show();
    }
    
};
#endif