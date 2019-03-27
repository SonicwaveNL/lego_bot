#include "BrickPi3.h" // for BrickPi3
#include <iostream>      // for cout
#include <string>       // Include strings
#include <unistd.h>     // for sleep
#include <signal.h>     // for catching exit signals
#include <iomanip>		// for setw and setprecision

using namespace std;

BrickPi3 BP;

void exit_signal_handler(int signo);

//Function to move robot (left, right)
void moveBot(const int measurement, const int valueLeft, const int valueRight) {
	BP.set_motor_power(PORT_C, valueLeft); //Left motor
    BP.set_motor_power(PORT_B, valueRight); // Right motor

    cout << "\033[2J\033[1;1H"; //Clear screen
    cout << "CLINIBOT ============" << endl;

    cout << endl << "-INFORMATION:" << endl;
    cout << " Battery voltage: " << BP.get_voltage_battery() << endl;
    cout << " 9v voltage: " << BP.get_voltage_9v() << endl;
    cout << " 5v voltage: " << BP.get_voltage_5v() << endl;
    cout << " 3.3v voltage: " << BP.get_voltage_3v3() << endl;

    cout << endl << "-MOTOR VALUES: " << endl;
    cout << " Left: " << valueLeft << "v" << endl;
    cout << " Right: " << valueRight << "v" << endl;

    cout << endl << "-DETECTION:" << endl ;
    cout << " LIGHT REFLECTION: " << measurement << endl;

    cout << endl << "=====================" << endl;

}

void askDirection(){
    sting direction;
    cout << endl << "Which direction do you want to go?" << endl;
    cout >> "-GO DIRECTION: ";
    cin >> direction;

    while(true){
        if(direction == "L" || direction == "left"){
            moveBot(measurement, 10, 50);
            break
        } else if(direction == "L" || direction == "left"){
            moveBot(measurement, 50, 10);
            break
        } else {
            cout << endl << "-INVALID DIRECION, TRY AGAIN!" << endl;
        }
    }
    
}

// check if other sensor is black
bool isCrossing(){

    //sensor_ultrasonic_t Ultrasonic2;

    int measurement = 0;
    bool s1 = false;
    bool s2 = false;

    if (BP.get_sensor(PORT_1, Color1) == 0) {
        measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
        if(measurement >=250 && measurement < 400){
            s1 = true;
        }
    }


    if (BP.get_sensor(PORT_3, Light3) == 0) {
        measurement = Light3.reflected;
        if(measurement >= 2400){
            s2 = true;
        }
    }

    if(s1 && s2){
        return true;
    }else{
        return false;
    }
}

int main() {
	
	signal(SIGINT, exit_signal_handler); // register the exit function for Ctrl+C

	BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

	sensor_ultrasonic_t Ultrasonic2;
	sensor_light_t      Light3;

	int average = 0;
	int measurement = 0;

	while (true) {

		if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
			if (Ultrasonic2.cm > 10) {

                if(!isCrossing()){

                    if (BP.get_sensor(PORT_3, Light3) == 0) {
                        measurement = Light3.reflected;
                        if (measurement >= 1900 && measurement <= 2300) {
                            moveBot(measurement, 50, 50); //Forward
                        }
                        if (measurement > 1800 && measurement < 1900) {
                            moveBot(measurement, 10, 50); //Left
                        }
                        else if (measurement > 2300) {
                            moveBot(measurement, 50, 10); //Right
                        }
                    }
                } else {
                    askDirection();
                }
			}
			else
			{
				moveBot(measurement, 0, 0);
			}

            usleep(50000);//slaap een kwart seconde (1 usleep = 1 miljoenste van een seconde)

		}
		else
		{
			cout << "can't find the ultrasonic sensor" << endl;
		}
	}	
	
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo) {
	if (signo == SIGINT) {
		BP.reset_all();    // Reset everything so there are no run-away motors
		exit(-2);
	}
}