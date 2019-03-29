// Includes
#include <iostream>      // Basic IO
#include <unistd.h>      // for usleep and sleep(for linux)
#include <signal.h>      // for catching exit signals
#include <curses.h>      // Library to be able to rea
#include "BrickPi3.h"    // BrickPi3 Header file - used to control the ROBO-MAN
#include <string>        // String variables
#include "BluetoothSocket.h" //Bluetooth

// Using statements
using std::string;
using std::cin;
using std::cout;
using std::endl;

BrickPi3 BP; // Define an instance of BrickPi3, called 'BP'

//Sensor definitions
sensor_light_t      Light3; //RGB Light sensor
sensor_color_t      Color1; //Infrared sensor
sensor_ultrasonic_t Ultrasonic2; //Ultrasonic sensor

void eHandler(int s){
            cout << "Caught ctrl c" << endl;
            BP.reset_all();
            exit(0);

}


//-- Movement functions
// Stop the robot by setting the motor power to '0'
void moveStop(){
    BP.set_motor_power(PORT_B, 0);
    BP.set_motor_power(PORT_C, 0);
    // Zet stroom van poort B en C op 0, waardoor de robot stopt.

    cout << " Stopped - ";
}




void moveLeft() {

		BP.set_motor_dps(PORT_B, 80);
		BP.set_motor_dps(PORT_C, -80);
		/*BP.set_motor_position_relative(PORT_B, 45);
		BP.set_motor_position_relative(PORT_C, -45);*/

	cout << " Left - ";

}

void moveRight() {
		BP.set_motor_dps(PORT_B, -80);
		BP.set_motor_dps(PORT_C, 80);
		/*BP.set_motor_position_relative(PORT_B, -45);
		BP.set_motor_position_relative(PORT_C, 45);*/

	cout << " Right - ";

}
void moveFwd() {
	//BP.set_motor_dps(PORT_B, 360);
	//BP.set_motor_dps(PORT_C, 360);
	// Draai de motor op port B en C 360 graden
    BP.set_motor_position_relative(PORT_B, 420);
    BP.set_motor_position_relative(PORT_C, 420);
	cout << " Forward - ";

}
void moveBack() {
	//BP.set_motor_dps(PORT_B, -360);
	//BP.set_motor_dps(PORT_C, -360);
	// Draai de motor op port B en C -360 graden
    BP.set_motor_position_relative(PORT_B, -420);
    BP.set_motor_position_relative(PORT_C, -420);

	cout << " Back - ";

}
void turnLeft(){
    BP.set_motor_position_relative(PORT_B, 420);
    BP.set_motor_position_relative(PORT_C, -420);
    cout << "L-turn" << endl;
    //should be 90 degrees
}
void turnRight(){
    BP.set_motor_position_relative(PORT_B, -420);
    BP.set_motor_position_relative(PORT_C, 420);
    cout << "R-turn" << endl;
    //should be 90 degrees
}

// Turn the eyes left
void lookLeft(){
    BP.set_motor_position_relative(PORT_D, 95);
}

// Turn the eyes right
void lookRight(){
    BP.set_motor_position_relative(PORT_D, -95);
}


//- Control functions
// Check if there is a regular crossing(both sensors would be black)
bool isCrossing(){
    // check if other sensor is black
    //sensor_ultrasonic_t Ultrasonic2;

    int measurement = 0;
    bool s1 = false;
    bool s2 = false;

    if (BP.get_sensor(PORT_1, Color1) == 0) {
        measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
        cout << "rgb val: " << measurement << endl;

        if(measurement >=300 && measurement < 500){
            s1 = true;
        }
    }


    if (BP.get_sensor(PORT_3, Light3) == 0) {
        measurement = Light3.reflected;
        cout << "ir val: " << measurement << endl;
        if(measurement >= 2200){
            s2 = true;
        }
    }

    if(s1 && s2){
        return true;
    }else{
        return false;
    }



}
void testValues(){
    int measurement = 0;
    while(true){

            if (BP.get_sensor(PORT_1, Color1) == 0) {
                measurement = (Color1.reflected_red + Color1.reflected_green + Color1.reflected_blue) / 3;
                cout << "Value for RGB: " << measurement;
            }


            if (BP.get_sensor(PORT_3, Light3) == 0) {
                measurement = Light3.reflected;
                cout << " - Value for IR: " << measurement;

            }
            if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
                    cout << " - Value for ultrasonic: " << Ultrasonic2.cm << endl;

            }
            sleep(1);
    }
}
// Check if there is an obstacle in FRONT of the robot
bool obstacleDetected(){
    int obstacleDetectionDistance = 25;
    int timeout = 0;
    while(true){
        timeout++;
        if(timeout > 2000){
            break;
        }
        if (BP.get_sensor(PORT_2, Ultrasonic2) == 0) {
                if(Ultrasonic2.cm <= obstacleDetectionDistance){
                    return true;

                }

        }
    }
    //cout << endl << "cm: " << Ultrasonic2.cm << endl;
    return false;

}
bool lineDetected(){
    int measurement = 0;
    cout << endl << endl << "checking line";

    if (BP.get_sensor(PORT_3, Light3) == 0) {
        measurement = Light3.reflected;
        cout << endl << "m: " << measurement << " borders: 2000 <= x < 2200" << endl;
        if(measurement >=2000 && measurement < 2200){
            return true;
        }else{
            return false;

        }
    }


}
void checkGrid(){
    unsigned int routesToCheck = 3; // MIN 3
    bool driveRequired = false;
    for(unsigned int i = 0; i < routesToCheck; i++){
        switch(i){
            case 0: cout << endl << "Forward: ";
            cout << "Checking route: " << i;
            if(!obstacleDetected()){
                cout << "...clear!" << endl;
                cout << "checking if path...";
                moveFwd();
                sleep(3);
                moveStop();
                if(!lineDetected()){
                    cout << "no path" << endl;
                }else{
                    cout << "path found" << endl;
                }
                moveBack();
                sleep(3);

            }else{
                cout << "...blocked!" << endl;
            }
            break;
            case 1: cout << endl << "Left: ";
            cout << "Checking route: " << i;
            lookLeft();
            sleep(2);
            if(!obstacleDetected()){
                cout << "...clear!" << endl;
                lookRight();
                cout << "checking if path...";
                turnLeft();
                sleep(3);
                moveFwd();
                sleep(3);
                moveStop();

                if(!lineDetected()){
                    cout << "no path" << endl;
                }else{
                    cout << "path found" << endl;
                }
                moveBack();
                sleep(3);

                turnRight();
                sleep(3);

            }else{
                cout << "...blocked!" << endl;
                lookRight();

            }
            break;
            case 2: cout << endl << "Right: ";
            cout << "Checking route: " << i << endl;
            lookRight();
            if(!obstacleDetected()){
                cout << "...clear!" << endl;
                lookLeft();
                cout << "checking if path...";
                turnRight();
                sleep(3);
                moveFwd();
                sleep(3);
                moveStop();

                if(!lineDetected()){
                    cout << "no path" << endl;
                }else{
                    cout << "path found" << endl;
                }

                moveBack();
                sleep(3);

                turnLeft();
                sleep(3);

            }else{
                cout << "...blocked!" << endl;
                lookLeft();
            }
            break;
            default: cout << endl << "Unknown: ";
            break;
        }


    }
}

void vvDance(){
    cout << "Het dak moet er af...!" << endl;
    BP.set_motor_dps(PORT_B, -360);
    BP.set_motor_dps(PORT_C, -360);
    usleep(500000);
    BP.set_motor_dps(PORT_B, 360);
    BP.set_motor_dps(PORT_C, 360);
    usleep(500000);
    BP.set_motor_dps(PORT_B, 360);
    BP.set_motor_dps(PORT_C, -360);
    sleep(10);
    BP.set_motor_dps(PORT_B, -360);
    BP.set_motor_dps(PORT_C, -360);
    usleep(500000);
    BP.set_motor_dps(PORT_B, 360);
    BP.set_motor_dps(PORT_C, 360);
    usleep(500000);
}


// Show the movement controls on-screen
void showControls(){
    cout << "Controls: " << endl << "W - Forwards" << endl << "A - Left" << endl << "S - Backwards" << endl << "D - Right" << endl << endl << "Press enter to continue...";
    string cinput;
    cin >> cinput;

}
void controlBluetooth(){
    BluetoothServerSocket serversock(2, 1);  //2 is het channel-number
	cout << "Waiting for a bluetooth device..." << endl;
	while(true) {
		BluetoothSocket* clientsock = serversock.accept();
		cout << "Connected with [" << clientsock->getForeignAddress().getAddress() << "]" << endl;
		MessageBox& mb = clientsock->getMessageBox();

		string input;
		while(mb.isRunning()) {
			input = mb.readMessage();  //blokkeert niet
            cout << "Executing Action: " << input << endl;
			if(input != ""){
                // input
                if(input.find("UP") != std::string::npos){
                    moveFwd();
                }else if(input.find("LEFT") != std::string::npos){
                    moveLeft();
                }else if(input.find("RIGHT") != std::string::npos){
                    moveRight();
                }else if(input.find("DOWN") != std::string::npos){
                    moveBack();
                }else if(input.find("A") != std::string::npos){
                    vvDance();
                }else if(input.find("FIRE") != std::string::npos){
                    moveStop();
                }
                // cout << endl << input << endl;
            }

			//doe andere dingen.
			cout.flush();
			sleep(1); // wacht 500 ms
		}

		clientsock->close();

	}
}

void controlTerminal(){
    // Show movement controls
    showControls();

    // Curses settings
    initscr();      // Init curses !!! Anything that gets printed after this will be printed weirdly
    cbreak();       // Sets that the code buffers per-key and not per newline
    noecho();       // Don't print the characters entered
    timeout(1750);  // Check once every 1,750ms
    //signal(SIGINT, exit_signal_handler);    // register the exit function for Ctrl+C

    while (true){
        int userIn = getch();   // Request a single character from the user
        refresh();              // Refresh the screen

        // Check the input of the user
        if (userIn == 'w'){
            moveFwd();
       }else if(userIn == 'a'){
           moveLeft();
       }else if(userIn == 'd'){
           moveRight();
       }else if(userIn == 's'){
            moveBack();
       }else if(userIn == 'e'){
           break;
       }else if(userIn == 'p'){
            moveStop();
        }
     }
}


// debug function
void debug(){
    cout << "VV DEBUG" << endl;
    cout << "a - turn left 90 degrees" << endl << "t - check if there is a crossing"<< endl << "o - check if there is an obstacle" << endl << "oa - check all obstacles" << endl << "tv - test values" << endl;
    cout << "cr - look left and right" << endl;

    while(true){
        cout << endl << "> ";
        string uin;
        cin >> uin;
        if(uin == "a"){
            turnLeft();
        }else if(uin == "t"){
            cout << "Crossing > ";
            if(isCrossing()){
                cout << "Yes." << endl;
            }else{
                cout << "No." << endl;
            }
        }else if(uin == "o"){
            cout << "Route > ";
            if(obstacleDetected()){
                cout << "Blocked." << endl;
            }else{
                cout << "Clear." << endl;
            }
        }else if(uin == "oa"){
            checkGrid();
        }else if(uin == "tv"){
            testValues();
        }else if(uin == "cr"){
            lookLeft();
            cout << "Looking left" << endl;
            sleep(2);
            lookRight();
            cout << "Looking forward" << endl;
            sleep (2);
            lookRight();
            cout << "Looking right" << endl;
            sleep(2);
            lookLeft();
            cout << "Looking forward again" << endl;
        }else{
            return;
        }
    }

}
// Main execution
int main()
{
    cout << "Setting up ctrl c check..." << endl;
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = eHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    cout << "Checking voltage..." << endl;
    int cvoltage = BP.get_voltage_battery();
    if (cvoltage < 10) {
        cout << "[ERROR] BATTERY CRITICAL!" << endl;
        cout << "Voltage Level: " << cvoltage << endl;
        BP.reset_all();
        exit(0);
    }else{
        cout << "[INFO] Battery levels sufficient" << endl;
        cout << "Voltage Level: " << cvoltage << endl;
    }


    cout << "Setting up sensors..." << endl;
    BP.detect(); // Make sure that the BrickPi3 is communicating and that the firmware is compatible with the drivers.
	BP.set_sensor_type(PORT_1, SENSOR_TYPE_NXT_COLOR_FULL);
	BP.set_sensor_type(PORT_2, SENSOR_TYPE_NXT_ULTRASONIC);
	BP.set_sensor_type(PORT_3, SENSOR_TYPE_NXT_LIGHT_ON);

    BP.set_motor_limits(PORT_B, 30, 0);
    BP.set_motor_limits(PORT_C, 30, 0);
    BP.set_motor_limits(PORT_D, 90, 0);


    cout << "Enter 'move' to control the robot via this terminal, enter 'bt' to control the robot via bluetooth or enter 'debug' to enter debug menu." << endl;
    string userChoice;
    cin >> userChoice;
    if(userChoice == "bt"){
        controlBluetooth();
    }else if(userChoice == "debug"){
        debug();
    }else if(userChoice == "move"){
        controlTerminal();
    }

    BP.reset_all();
    cout << "Program Terminated." << endl;
    return 0;

}
