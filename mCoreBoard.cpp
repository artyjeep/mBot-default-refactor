/*
 * mCore.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: Pete
 */
#include "mCoreBoard.h"

MePort_Sig mePort[11]={{(uint8_t)NC, (uint8_t)NC}, {11, 12}, {9, 10}, {A2, A3}, {A0, A1},
	    {6, 7}, {5, 4}, {A7, 13}, {8, A6}, {6, 7}, {5, 4}};

mCoreBoard::mCoreBoard():
rgb(),
ultr(PORT_3),
line(PORT_2),
ledMx(),
ir(),
buzzer(),
ts(),
seg(),
MotorL(M1),
MotorR(M2),
generalDevice(),
servo(),
analogs{ A0, A1, A2, A3, A4, A5, A6, A7 }
{

}



