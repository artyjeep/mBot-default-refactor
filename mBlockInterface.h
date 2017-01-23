/*
 * mBlockInterface.h
 *
 *  Created on: Jan 15, 2017
 *      Author: Pete
 */

#ifndef MBLOCKINTERFACE_H_
#define MBLOCKINTERFACE_H_
#include <Servo.h>
#include "mCoreBoard.h"

#define VERSION 0
#define ULTRASONIC_SENSOR 1
#define TEMPERATURE_SENSOR 2
#define LIGHT_SENSOR 3
#define POTENTIONMETER 4
#define JOYSTICK 5
#define GYRO 6
#define SOUND_SENSOR 7
#define RGBLED 8
#define SEVSEG 9
#define MOTOR 10
#define SERVO 11
#define ENCODER 12
#define IR 13
#define IRREMOTE 14
#define PIRMOTION 15
#define INFRARED 16
#define LINEFOLLOWER 17
#define IRREMOTECODE 18
#define SHUTTER 20
#define LIMITSWITCH 21
#define BUTTON 22
#define DIGITAL 30
#define ANALOG 31
#define PWM 32
#define SERVO_PIN 33
#define TONE 34
#define BUTTON_INNER 35
#define LEDMATRIX 41
#define TIMER 50

#define GET 1
#define RUN 2
#define RESET 4
#define START 5

class mBlockInterface {
	union VAL{
		byte byteVal[4];
		float floatVal;
		long longVal;
	};

	union VALDOUBLE{
		byte byteVal[8];
		double doubleVal;
	};

	union VALSHORT{
		byte byteVal[2];
		short shortVal;
	};

public:
	mBlockInterface(mCoreBoard &_board);
	virtual ~mBlockInterface();
	void serialHandle();
private:
	unsigned char readBuffer(int index);
	void writeBuffer(int index, unsigned char c);
	void writeHead();
	void writeEnd();
	void writeSerial(unsigned char c);
	void readSerial();
	void parseData();
	void callOK();
	void sendByte(char c);
	void sendString(String s);
	void sendFloat(float value);
	void sendShort(double value);
	void sendDouble(double value);
	short readShort(int idx);
	float readFloat(int idx);
	char* readString(int idx, int len);
	uint8_t* readUint8(int idx, int len);
	void runModule(int device);
	void readSensor(int device);


	mCoreBoard &board;

	boolean isAvailable;
	int len;
	char buffer[52];
	char bufferBt[52];
	byte index;
	byte dataLen;
	byte modulesLen;
	boolean isStart;
	char serialRead;
	char _receiveStr[20];
	uint8_t _receiveUint8[16];
	boolean buttonPressed;
	double currentTime;
	double lastTime;
	String mVersion;
	unsigned char prevc;
	VAL val;
	VALDOUBLE valDouble;
	VALSHORT valShort;
};

#endif /* MBLOCKINTERFACE_H_ */
