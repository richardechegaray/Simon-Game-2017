/* 
 * Option 2 - Simon Game
 * Name: Richard Echegaray
 * Student #: 73815326
 * Lab Section: L1K
 * Email: richardechegaray@alumni.ubc.ca  
 * Date: Nov. 25, 2017
 * Purpose: My implementation of the  'Simon' game
 * Notes: One bug is that sometimes, when the program is ran for the first time after opening visual studio it does
 * not run. I am not sure if this is because of my computer, but otherwise, it always works just fine.
 * Also, when the game initally starts up, all four lights flash to indicate that a game is starting. This also happens
 * afterwards, ie) whenever a game is won or lost
 */

#define _CRT_SECURE_NO_WARNINGS

//header files
#include <stdio.h>     
#include <stdlib.h>
#include <DAQlib.h>     //used for standard DAQ framework functions
#include <Windows.h>    //used for sleep function
#include <time.h>    //used for seeding the random number generator

//defining true and false for DAQ library functions
#define TRUE 1        
#define FALSE 0

//defining ON and OFF for LED channels
#define ON 1
#define OFF 0

//defining channel numbers for each LED
#define GREEN_LIGHT 0
#define	RED_LIGHT 1
#define YELLOW_LIGHT 2
#define BLUE_LIGHT 3

//defining channel numbers for each button
#define GREEN_BUTTON 0
#define RED_BUTTON 1
#define YELLOW_BUTTON 2
#define BLUE_BUTTON 3

//defining seconds for sleep function, max length which is the maximum sequence length
#define SECONDS 1000.0
#define MAX_LENGTH 5

void runSimon(int data[]);    //function that implements the simon game
int randomInt(int upper);      //function that generates a random integer from 0 to the upper bound passed
void generateSequence(int length, int data[]);   //function that generates a random sequence of given length into an array
int readButton(void);     //function to read from the four push buttons
void flashOnce(int light);    //function to flash an LED light once (used throughout the entire game)
void flashThreeTimes(int light);    //function to flash an LED light three times (green to indicate win, red for loss)
void resetGame(int data[]);   //function to flash all lights and reset random sequence generator

int main(void) {
	int setupNum = 6;      
	int data[MAX_LENGTH]; //the array only needs to be as large as the max sequence length
	
	printf("Welcome to Richard Echegaray's Simon Game!\n\nThe simulation will begin in three seconds. . .\n");
	Sleep(3 * SECONDS);

	printf("\n\nSimulator now commencing. . .\nExit the simulator when you would like to quit.\n\n");

	if (setupDAQ(setupNum) == TRUE) {   //testing if the simulator opens correctly
		runSimon(data);

		printf("Thank you for playing! :D\n\n");
	}
	else
		printf("Error: Could not initialize simulation. . . \n\n");
	
	system("PAUSE");
 	return 0;
}

void runSimon(int data[]) {
	int level = 1;
	int index;
	int input;

	srand((unsigned)time(NULL));   //seeding the random number generator
	
	resetGame(data); //start new game, generate new random sequence

	while (continueSuperLoop() == TRUE) {  //while the user is still using the simulator, the game will run
		
		Sleep(1.5 * SECONDS);

		for (index = 0; index < level ; index++) {   //this loop flashes the randomly generated lights, according to level
			flashOnce(data[index]); 
		}

		if (continueSuperLoop() == FALSE)  //checking to see if user quit the loop
			break;

		for (index = 0; index < level; index++) {
			input = readButton(); //function to read from push buttons
			
			if (continueSuperLoop() == FALSE)  //checking to see if user quit the loop
				break;

			if (input != data[index]) {   //checking to see if the correct buttons have been pressed
				flashThreeTimes(RED_LIGHT);
				
				Sleep(1 * SECONDS);

				resetGame(data); //start new game, generate new random sequence
				level = 0; //reseting level to zero, as it increments at the end of loop
			}
		}
		
		if (continueSuperLoop() == FALSE)     //checking to see if user quit the loop
			break;

		if (level < MAX_LENGTH) //increments level until level reaches 5
			level++;

		else if (level == MAX_LENGTH) {  //once level equals 5, and you do not lose, you win
			flashThreeTimes(GREEN_LIGHT);

			if (continueSuperLoop() == FALSE)     //checking to see if user quit the loop
				break;

			Sleep(1 * SECONDS);

			resetGame(data); //start new game, generate new random sequence
			level = 1;
		}
	}
}

/* 
 * Purpose: This function generates a random integer, between zero and the upper parameter
 * Parameters: This function only takes an upper bound number, and returns a 
 * randomly-generated numbetween 0 and the given parameter
 */
int randomInt(int upper) {
	int randInt;
	randInt = rand() % (upper + 1);
	return randInt;
}

/* 
 * Purpose: This function generates an array (sequence) of random numbers. It makes use of the randomInt function above
 * Parameters: This function takes in a length of which the array will become, and the array that is being passed through
 */
void generateSequence(int length, int data[]) {
	int index;

	for (index = 0; index < length ; index++) 
		data[index] = randomInt(3);
}

/*
 * Purpose: This function reads from the push buttons on the simulator
 * Parameters: It takes in no parameters, and returns the channel number of the button that has been pushed
 */
int readButton(void) {
	int channelNumber = -1; //defines the value -1 to keep the loop captive, until a button is read

	while (channelNumber == -1) { //as the values being read are either 0,1,2 or 3, the loop will run until something is read

		if (digitalRead(GREEN_BUTTON) == ON) { //checking to see if user has pressed a button
			do {    //holding the user captive in a loop until they release the button (turn from off to on)
				channelNumber = GREEN_BUTTON;
			} while (digitalRead(GREEN_BUTTON) == ON);
		}
			
		if (digitalRead(RED_BUTTON) == ON) { //same as above, repeated for each button
			do {
				channelNumber = RED_BUTTON;
			} while (digitalRead(RED_BUTTON) == ON);
		}
				
		if (digitalRead(YELLOW_BUTTON) == ON) {
			do {
				channelNumber = YELLOW_BUTTON;
			} while (digitalRead(YELLOW_BUTTON) == ON);	
		}

		if (digitalRead(BLUE_BUTTON) == ON) {
			do {
				channelNumber = BLUE_BUTTON;
			} while (digitalRead(BLUE_BUTTON) == ON);
		}
		
		if (continueSuperLoop() == FALSE)  //checking to see if user quit the loop
			break;
		
	}
		return (channelNumber);
}

/*
 * Purpose: This simple function flashes a light once, it is used frequently in the simon game
 * Parameters: This function only takes in the channel number for the LED light to be flashed
 */
void flashOnce(int light) {
	Sleep(0.2 * SECONDS); //the sleep is placed at the beginning so that the readButton function is more consistent
	digitalWrite(light, ON); //turning on
	Sleep(0.5 * SECONDS);
	digitalWrite(light, OFF); //turning off
}

/*
* Purpose: This simple function flashes a light three times, it is used for when a user either loses or wins the game
* Parameters: This function only takes in the channel number for the LED light to be flashed (usually green or red)
*/
void flashThreeTimes(int light) {
	digitalWrite(light, ON);
	Sleep(0.4 * SECONDS);
	digitalWrite(light, OFF);
	Sleep(0.2 * SECONDS);
	digitalWrite(light, ON);
	Sleep(0.4 * SECONDS);
	digitalWrite(light, OFF);
	Sleep(0.2 * SECONDS);
	digitalWrite(light, ON);
	Sleep(0.4 * SECONDS);
	digitalWrite(light, OFF);
}

/*
 * Purpose: This simple function flashes all of the LED lights a few times, and also re-randomizes the array,
 * also known as the random sequence of integers. This function is to indicate whenever a new game is starting
 * Parameters: The only parameter is the data array, which is passed by address
 */
void resetGame(int data[]) { 
	generateSequence(MAX_LENGTH, data);

	digitalWrite(GREEN_BUTTON, ON);
	digitalWrite(RED_BUTTON, ON);
	digitalWrite(YELLOW_BUTTON, ON);
	digitalWrite(BLUE_BUTTON, ON);
	Sleep(0.3 * SECONDS);
	digitalWrite(BLUE_BUTTON, OFF);
	digitalWrite(YELLOW_BUTTON, OFF);
	digitalWrite(RED_BUTTON, OFF);
	digitalWrite(GREEN_BUTTON, OFF);
	Sleep(0.2 * SECONDS);
	digitalWrite(BLUE_BUTTON, ON);
	digitalWrite(YELLOW_BUTTON, ON);
	digitalWrite(RED_BUTTON, ON);
	digitalWrite(GREEN_BUTTON, ON);
	Sleep(0.3 * SECONDS);
	digitalWrite(GREEN_BUTTON, OFF);
	digitalWrite(RED_BUTTON, OFF);
	digitalWrite(YELLOW_BUTTON, OFF);
	digitalWrite(BLUE_BUTTON, OFF);
	Sleep(0.2 * SECONDS);
	digitalWrite(GREEN_BUTTON, ON);
	digitalWrite(RED_BUTTON, ON);
	digitalWrite(YELLOW_BUTTON, ON);
	digitalWrite(BLUE_BUTTON, ON);
	Sleep(0.3 * SECONDS);
	digitalWrite(BLUE_BUTTON, OFF);
	digitalWrite(YELLOW_BUTTON, OFF);
	digitalWrite(RED_BUTTON, OFF);
	digitalWrite(GREEN_BUTTON, OFF);
	Sleep(0.2 * SECONDS);
	digitalWrite(BLUE_BUTTON, ON);
	digitalWrite(YELLOW_BUTTON, ON);
	digitalWrite(RED_BUTTON, ON);
	digitalWrite(GREEN_BUTTON, ON);
	Sleep(0.3 * SECONDS);
	digitalWrite(GREEN_BUTTON, OFF);
	digitalWrite(RED_BUTTON, OFF);
	digitalWrite(YELLOW_BUTTON, OFF);
	digitalWrite(BLUE_BUTTON, OFF);
}