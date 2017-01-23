/*
 * mBlockInterface.cpp
 *
 *  Created on: Jan 15, 2017
 *      Author: Pete
 */

#include "mBlockInterface.h"

mBlockInterface::mBlockInterface(mCoreBoard &_board) :
		board(_board), isAvailable(false), len(52), index(0), dataLen(0), modulesLen(0), isStart(false),
		serialRead(0), buttonPressed(false), currentTime(0.0), lastTime(0.0), mVersion("1.2.103"), prevc(0)
{
}

mBlockInterface::~mBlockInterface() {
	// TODO Auto-generated destructor stub
}

unsigned char mBlockInterface::readBuffer(int index) {
	return buffer[index];
}

void mBlockInterface::writeBuffer(int index, unsigned char c) {
	buffer[index] = c;
}

void mBlockInterface::writeHead() {
	writeSerial(0xff);
	writeSerial(0x55);
}
void mBlockInterface::writeEnd() {
	Serial.println();
}
void mBlockInterface::writeSerial(unsigned char c) {
	Serial.write(c);
}
void mBlockInterface::readSerial() {
	isAvailable = false;
	if (Serial.available() > 0) {
		isAvailable = true;
		serialRead = Serial.read();
	}
}
void mBlockInterface::serialHandle() {
	readSerial();
	if (isAvailable) {
		unsigned char c = serialRead & 0xff;
		if (c == 0x55 && isStart == false) {
			if (prevc == 0xff) {
				index = 1;
				isStart = true;
			}
		} else {
			prevc = c;
			if (isStart) {
				if (index == 2) {
					dataLen = c;
				} else if (index > 2) {
					dataLen--;
				}
				writeBuffer(index, c);
			}
		}
		index++;
		if (index > 51) {
			index = 0;
			isStart = false;
		}
		if (isStart && dataLen == 0 && index > 3) {
			isStart = false;
			parseData();
			index = 0;
		}
	}
}

// Data Format
// HEADING		LEN		IDX		ACTION	DEVICE
// 0	1		2		3		4		5
// 0xFF 0x55	0x??	0x??	0x??	0x??
void mBlockInterface::parseData() {
	isStart = false;
	int idx = readBuffer(3);
	int action = readBuffer(4);
	int device = readBuffer(5);
	switch (action) {
	case GET: {
		writeHead();
		writeSerial(idx);
		readSensor(device);
		writeEnd();
	}
		break;
	case RUN: {
		runModule(device);
		callOK();
	}
		break;
	case RESET: {
		//reset
		callOK();
	}
		break;
	case START: {
		//start
		callOK();
	}
		break;
	}
}
void mBlockInterface::callOK() {
	writeSerial(0xff);
	writeSerial(0x55);
	writeEnd();
}
void mBlockInterface::sendByte(char c) {
	writeSerial(1);
	writeSerial(c);
}
void mBlockInterface::sendString(String s) {
	int l = s.length();
	writeSerial(4);
	writeSerial(l);
	for (int i = 0; i < l; i++) {
		writeSerial(s.charAt(i));
	}
}
//1 byte 2 float 3 short 4 len+string 5 double
void mBlockInterface::sendFloat(float value) {
	writeSerial(2);
	val.floatVal = value;
	writeSerial(val.byteVal[0]);
	writeSerial(val.byteVal[1]);
	writeSerial(val.byteVal[2]);
	writeSerial(val.byteVal[3]);
}
void mBlockInterface::sendShort(double value) {
	writeSerial(3);
	valShort.shortVal = value;
	writeSerial(valShort.byteVal[0]);
	writeSerial(valShort.byteVal[1]);
	writeSerial(valShort.byteVal[2]);
	writeSerial(valShort.byteVal[3]);
}
void mBlockInterface::sendDouble(double value) {
	writeSerial(5);
	valDouble.doubleVal = value;
	writeSerial(valDouble.byteVal[0]);
	writeSerial(valDouble.byteVal[1]);
	writeSerial(valDouble.byteVal[2]);
	writeSerial(valDouble.byteVal[3]);
	writeSerial(valDouble.byteVal[4]);
	writeSerial(valDouble.byteVal[5]);
	writeSerial(valDouble.byteVal[6]);
	writeSerial(valDouble.byteVal[7]);
}
short mBlockInterface::readShort(int idx) {
	valShort.byteVal[0] = readBuffer(idx);
	valShort.byteVal[1] = readBuffer(idx + 1);
	return valShort.shortVal;
}
float mBlockInterface::readFloat(int idx) {
	val.byteVal[0] = readBuffer(idx);
	val.byteVal[1] = readBuffer(idx + 1);
	val.byteVal[2] = readBuffer(idx + 2);
	val.byteVal[3] = readBuffer(idx + 3);
	return val.floatVal;
}

char* mBlockInterface::readString(int idx, int len) {
	for (int i = 0; i < len; i++) {
		_receiveStr[i] = readBuffer(idx + i);
	}
	_receiveStr[len] = '\0';
	return _receiveStr;
}

uint8_t* mBlockInterface::readUint8(int idx, int len) {
	for (int i = 0; i < len; i++) {
		if (i > 15) {
			break;
		}
		_receiveUint8[i] = readBuffer(idx + i);
	}
	return _receiveUint8;
}

void mBlockInterface::runModule(int device) {
	//0xff 0x55 0x6 0x0 0x2 0x22 0x9 0x0 0x0 0xa
	int port = readBuffer(6);
	int pin = port;
	switch (device) {
	case MOTOR: {
		int speed = readShort(7);
		port == M1 ? board.MotorL.run(speed) : board.MotorR.run(speed);
	}
		break;
	case JOYSTICK: {
		int leftSpeed = readShort(6);
		board.MotorL.run(leftSpeed);
		int rightSpeed = readShort(8);
		board.MotorR.run(rightSpeed);
	}
		break;
	case RGBLED: {
		int idx = readBuffer(7);
		int r = readBuffer(8);
		int g = readBuffer(9);
		int b = readBuffer(10);
		board.rgb.setColor(idx, r, g, b);
		board.rgb.show();
	}
		break;
	case SERVO: {
		int slot = readBuffer(7);
		pin = slot == 1 ? mePort[port].s1 : mePort[port].s2;
		int v = readBuffer(8);
		if (v >= 0 && v <= 180) {
			board.servo.attach(pin);
			board.servo.write(v);
		}
	}
		break;
	case SEVSEG: {
		if (board.seg.getPort() != port) {
			board.seg.reset(port);
		}
		float v = readFloat(7);
		board.seg.display(v);
	}
		break;
	case LEDMATRIX: {
		if (board.ledMx.getPort() != port) {
			board.ledMx.reset(port);
		}
		int action = readBuffer(7);
		if (action == 1) {
			int brightness = readBuffer(8);
			int color = readBuffer(9);
			int px = readShort(10);
			int py = readShort(12);
			int len = readBuffer(14);
			char *s = readString(15, len);
			board.ledMx.clearScreen();
			board.ledMx.setColorIndex(color);
			board.ledMx.setBrightness(brightness);
			board.ledMx.drawStr(px, py, s);
		} else if (action == 2) {
			int brightness = readBuffer(8);
			int dw = readBuffer(9);
			int px = readShort(10);
			int py = readShort(12);
			int len = readBuffer(14);
			uint8_t *ss = readUint8(15, len);
			board.ledMx.clearScreen();
			board.ledMx.setColorIndex(1);
			board.ledMx.setBrightness(brightness);
			board.ledMx.drawBitmap(px, py, dw, ss);
		} else if (action == 3) {
			int brightness = readBuffer(8);
			int point = readBuffer(9);
			int hours = readShort(10);
			int minutes = readShort(12);
			board.ledMx.clearScreen();
			board.ledMx.setColorIndex(1);
			board.ledMx.setBrightness(brightness);
			board.ledMx.showClock(hours, minutes, point);
		}
	}
		break;
	case LIGHT_SENSOR: {
		if (board.generalDevice.getPort() != port) {
			board.generalDevice.reset(port);
		}
		int v = readBuffer(7);
		board.generalDevice.dWrite1(v);
	}
		break;
	case IR: {
		int len = readBuffer(2) - 3;
		String s = "";
		for (int i = 0; i < len; i++) {
			s += (char) readBuffer(6 + i);
		}
		board.ir.sendString(s);
	}
		break;
	case SHUTTER: {
		if (board.generalDevice.getPort() != port) {
			board.generalDevice.reset(port);
		}
		int v = readBuffer(7);
		if (v < 2) {
			board.generalDevice.dWrite1(v);
		} else {
			board.generalDevice.dWrite2(v - 2);
		}
	}
		break;
	case DIGITAL: {
		pinMode(pin, OUTPUT);
		int v = readBuffer(7);
		digitalWrite(pin, v);
	}
		break;
	case PWM: {
		pinMode(pin, OUTPUT);
		int v = readBuffer(7);
		analogWrite(pin, v);
	}
		break;
	case TONE: {
		pinMode(pin, OUTPUT);
		int hz = readShort(6);
		if (hz > 0) {
			board.buzzer.tone(hz);
		} else {
			board.buzzer.noTone();
		}
	}
		break;
	case SERVO_PIN: {
		int v = readBuffer(7);
		if (v >= 0 && v <= 180) {
			board.servo.attach(pin);
			board.servo.write(v);
		}
	}
		break;
	case TIMER: {
		lastTime = millis() / 1000.0;
	}
		break;
	}
}
void mBlockInterface::readSensor(int device) {
	/**************************************************
	 ff    55      len idx action device port slot data a
	 0     1       2   3   4      5      6    7    8
	 0xff  0x55   0x4 0x3 0x1    0x1    0x1  0xa
	 ***************************************************/
	float value = 0.0;
	int port, slot, pin;
	port = readBuffer(6);
	pin = port;
	switch (device) {
	case ULTRASONIC_SENSOR: {
		if (board.ultr.getPort() != port) {
			board.ultr.reset(port);
		}
		value = (float) board.ultr.distanceCm(50000);
		sendFloat(value);
	}
		break;
	case TEMPERATURE_SENSOR: {
		slot = readBuffer(7);
		if (board.ts.getPort() != port || board.ts.getSlot() != slot) {
			board.ts.reset(port, slot);
		}
		value = board.ts.temperature();
		sendFloat(value);
	}
		break;
	case LIGHT_SENSOR:
	case SOUND_SENSOR:
	case POTENTIONMETER: {
		if (board.generalDevice.getPort() != port) {
			board.generalDevice.reset(port);
			pinMode(board.generalDevice.pin2(), INPUT);
		}
		value = board.generalDevice.aRead2();
		sendFloat(value);
	}
		break;
	case JOYSTICK: {
		slot = readBuffer(7);
		if (board.generalDevice.getPort() != port) {
			board.generalDevice.reset(port);
			pinMode(board.generalDevice.pin1(), INPUT);
			pinMode(board.generalDevice.pin2(), INPUT);
		}
		if (slot == 1) {
			value = board.generalDevice.aRead1();
			sendFloat(value);
		} else if (slot == 2) {
			value = board.generalDevice.aRead2();
			sendFloat(value);
		}
	}
		break;
	case IR: {
		//     if(board.ir.getPort()!=port){
		//       board.ir.reset(port);
		//     }
		//      if(board.irReady){
		//         sendString(irBuffer);
		//         irReady = false;
		//         irBuffer = "";
		//      }
	}
		break;
	case IRREMOTE: {
		//     unsigned char r = readBuffer(7);
		//     if(millis()/1000.0-lastIRTime>0.2){
		//       sendByte(0);
		//     }else{
		//       sendByte(irRead==r);
		//     }
		//     //irRead = 0;
		//     irIndex = 0;
	}
		break;
	case IRREMOTECODE: {
		//     sendByte(irRead);
		//     irRead = 0;
		//     irIndex = 0;
	}
		break;
	case PIRMOTION: {
		if (board.generalDevice.getPort() != port) {
			board.generalDevice.reset(port);
			pinMode(board.generalDevice.pin2(), INPUT);
		}
		value = board.generalDevice.dRead2();
		sendFloat(value);
	}
		break;
	case LINEFOLLOWER: {
		if (board.generalDevice.getPort() != port) {
			board.generalDevice.reset(port);
			pinMode(board.generalDevice.pin1(), INPUT);
			pinMode(board.generalDevice.pin2(), INPUT);
		}
		value = board.generalDevice.dRead1() * 2 + board.generalDevice.dRead2();
		sendFloat(value);
	}
		break;
	case LIMITSWITCH: {
		slot = readBuffer(7);
		if (board.generalDevice.getPort() != port
				|| board.generalDevice.getSlot() != slot) {
			board.generalDevice.reset(port, slot);
		}
		if (slot == 1) {
			pinMode(board.generalDevice.pin1(), INPUT_PULLUP);
			value = board.generalDevice.dRead1();
		} else {
			pinMode(board.generalDevice.pin2(), INPUT_PULLUP);
			value = board.generalDevice.dRead2();
		}
		sendFloat(value);
	}
		break;
	case BUTTON_INNER: {
		pin = board.analogs[pin];
		char s = readBuffer(7);
		pinMode(pin, INPUT);
		boolean currentPressed = !(analogRead(pin) > 10);
		sendByte(s ^ (currentPressed ? 1 : 0));
		buttonPressed = currentPressed;
	}
		break;
	case GYRO: {
		//       int axis = readBuffer(7);
		//       gyro.update();
		//       if(axis==1){
		//         value = gyro.getAngleX();
		//         sendFloat(value);
		//       }else if(axis==2){
		//         value = gyro.getAngleY();
		//         sendFloat(value);
		//       }else if(axis==3){
		//         value = gyro.getAngleZ();
		//         sendFloat(value);
		//       }
	}
		break;
	case VERSION: {
		sendString(mVersion);
	}
		break;
	case DIGITAL: {
		pinMode(pin, INPUT);
		sendFloat(digitalRead(pin));
	}
		break;
	case ANALOG: {
		pin = board.analogs[pin];
		pinMode(pin, INPUT);
		sendFloat(analogRead(pin));
	}
		break;
	case TIMER: {
		sendFloat(currentTime);
	}
		break;
	}
}

