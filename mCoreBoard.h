///@file Makeblock.h head file of Makeblock Library V2.1.0625
///Define the interface of Makeblock Library

#ifndef mCore_h
#define mCore_h

#include "MePort.h"
#include "MeDCMotor.h"
#include "MeBuzzer.h"
#include "MeTemperature.h"
#include "Me7SegmentDisplay.h"
#include "MeRGBLed.h"
#include "MeUltrasonic.h"
#include "MeInfraredReceiver.h"
#include "MeIR.h"
#include "MeLineFollower.h"
#include "MeLEDMatrix.h"
#include <Servo.h>

//Board_type MakeblockBoard = mCore;
//
extern MePort_Sig mePort[11];//mePort[0] is nonsense

// Buzzer Tones
#define NTD1 294
#define NTD2 330
#define NTD3 350
#define NTD4 393
#define NTD5 441
#define NTD6 495
#define NTD7 556
#define NTDL1 147
#define NTDL2 165
#define NTDL3 175
#define NTDL4 196
#define NTDL5 221
#define NTDL6 248
#define NTDL7 278
#define NTDH1 589
#define NTDH2 661
#define NTDH3 700
#define NTDH4 786
#define NTDH5 882
#define NTDH6 990
#define NTDH7 112

///@brief Class for mCore
 class mCoreBoard
 {
 public:
     mCoreBoard();

     MeRGBLed rgb;
     MeUltrasonic ultr;
     MeLineFollower line;
     MeLEDMatrix ledMx;
     MeIR ir;
     MeBuzzer buzzer;
     MeTemperature ts;
     Me7SegmentDisplay seg;

     MeDCMotor MotorL;
     MeDCMotor MotorR;
     MePort generalDevice;
     Servo servo;
     int analogs[8];
 };



#endif
