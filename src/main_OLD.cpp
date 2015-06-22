/******************************************************************************
* Copyright (c) 2011
* Locomotec
*
* Author:
* Sebastian Blumenthal
* Modified: May 2015 by Drew Waller
*
*
* This software is published under a dual-license: GNU Lesser General Public
* License LGPL 2.1 and BSD license. The dual-license implies that users of this
* code may choose which terms they prefer.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of Locomotec nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License LGPL as
* published by the Free Software Foundation, either version 2.1 of the
* License, or (at your option) any later version or the BSD license.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License LGPL and the BSD license for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License LGPL and BSD license along with this program.
*
******************************************************************************/

#include <iostream>
#include <vector>
#include <signal.h>
#include <curses.h>
#include <stdio.h>

#include "youbot/YouBotBase.hpp"
#include "youbot/YouBotManipulator.hpp"
#include "generic-joint/JointData.hpp"

using namespace std;
using namespace youbot;

bool running = true;

void sigintHandler(int signal) {
  running = false;
  printf("End!\n\r");
}

int main() {
	int nJnts = 5;
  signal(SIGINT, sigintHandler);
  YouBotBase myYouBotBase("youbot-base", YOUBOT_CONFIGURATIONS_DIR);
  myYouBotBase.doJointCommutation();
  YouBotManipulator myYouBotManipulator("youbot-manipulator", YOUBOT_CONFIGURATIONS_DIR);
  myYouBotManipulator.doJointCommutation();
  myYouBotManipulator.calibrateManipulator();
  myYouBotManipulator.calibrateGripper();

	std::vector<JointSensedAngle> armSensedAngles;
	std::vector<JointTorqueSetpoint> armSetTorques;
	armSetTorques.resize(nJnts);
	std::cout << "vectors initialized" << endl;
	for( int i = 0; i < 5; i++) armSetTorques[i].torque = 0 * newton_meters;
	std::cout << "torques zeroed" << endl;

  try {
		ConfigFile configfile("applications.cfg", std::string(CONFIG_DIR) );
		
		int ch = 0; int cnt = 0;
    double linearVel = 0.05; //meter_per_second
    double angularVel = 0.2; //radian_per_second
    double q2, q3, q4, t2, t3, t4;	// radians, newton_meters

    configfile.readInto(linearVel,  "KeyboardRemoteContol", "TanslationalVelocity_[meter_per_second]");
    configfile.readInto(angularVel, "KeyboardRemoteContol", "RotationalVelocity_[radian_per_second]");

		JointVelocitySetpoint setVel;
    quantity<si::velocity> longitudinalVelocity = 0 * meter_per_second;
    quantity<si::velocity> transversalVelocity = 0 * meter_per_second;
    quantity<si::angular_velocity> angularVelocity = 0 * radian_per_second;



    scrollok(stdscr, TRUE);
    (void) initscr(); /* initialize the curses library */
    keypad(stdscr, TRUE); /* enable keyboard mapping */
    (void) nonl(); /* tell curses not to do NL->CR/NL on output */
    (void) cbreak(); /* take input chars one at a time, no wait for \n */
    // (void) echo(); /* echo input - in color */
    noecho();
    nodelay(stdscr, TRUE);

    def_prog_mode();

    refresh();
    printf("up = drive forward\n\r"
            "down = drive backward\n\r"
            "left = drive left\n\r"
            "right = drive right\n\r"
            "y = turn right\n\r"
            "x = turn left\n\r"
            "any other key = stop\n\r\n");
    refresh();


    while (running) {

      myYouBotManipulator.getJointData( armSensedAngles );
      q2 = armSensedAngles[2-1].angle.value() -  65  *3.141592654/180;
      q3 = armSensedAngles[3-1].angle.value() + 146  *3.141592654/180;
      q4 = armSensedAngles[4-1].angle.value() - 102.5*3.141592654/180;

      t4 =  0 - ( 77970861*sin(q2 + q3 + q4))/ 50000000;
      t3 = t4 - (692273061*sin(q2 + q3)			)/250000000;
      t2 = t3 - (493626447*sin(q2)					)/100000000;

      armSetTorques[2-1].torque = t2 * newton_meters;
      armSetTorques[3-1].torque = t3 * newton_meters;
      armSetTorques[4-1].torque = t4 * newton_meters;
      
			switch (getch()) {
				case KEY_DOWN:
					longitudinalVelocity = -linearVel * meter_per_second;
					transversalVelocity = 0 * meter_per_second;
					angularVelocity = 0 * radian_per_second;
					LOG(info) << "drive backward";
					printf("\r");
					break;
				case KEY_UP:
					longitudinalVelocity = linearVel * meter_per_second;
					transversalVelocity = 0 * meter_per_second;
					angularVelocity = 0 * radian_per_second;
					LOG(info) << "drive forward";
					printf("\r");
					break;
				case KEY_LEFT:
					transversalVelocity = linearVel * meter_per_second;
					longitudinalVelocity = 0 * meter_per_second;
					angularVelocity = 0 * radian_per_second;
					LOG(info) << "drive left";
					printf("\r");
					break;
				case KEY_RIGHT:
					transversalVelocity = -linearVel * meter_per_second;
					longitudinalVelocity = 0 * meter_per_second;
					angularVelocity = 0 * radian_per_second;
					LOG(info) << "drive right";
					printf("\r");
					break;
				case ERR:
					break;
				case 'y':
					angularVelocity = angularVel * radian_per_second;
					transversalVelocity = 0 * meter_per_second;
					longitudinalVelocity = 0 * meter_per_second;
					LOG(info) << "turn right";
					printf("\r");
					break;
				case 'x':
					angularVelocity = -angularVel * radian_per_second;
					transversalVelocity = 0 * meter_per_second;
					longitudinalVelocity = 0 * meter_per_second;
					LOG(info) << "turn left";
					printf("\r");
					break;
				case 'q':
					longitudinalVelocity = 0 * meter_per_second;
					transversalVelocity = 0 * meter_per_second;
					angularVelocity = 0 * radian_per_second;
					LOG(info) << "joint 2: " << q2;
					printf("\r");
					LOG(info) << "joint 3: " << q3;
					printf("\r");
					LOG(info) << "joint 4: " << q4;
					printf("\r");
					break;

				default:
					longitudinalVelocity = 0 * meter_per_second;
					transversalVelocity = 0 * meter_per_second;
					angularVelocity = 0 * radian_per_second;
					LOG(info) << "stop";
					printf("\r");
					break;
			}
      
      myYouBotBase.setBaseVelocity(longitudinalVelocity, transversalVelocity, angularVelocity);
      myYouBotManipulator.setJointData(armSetTorques);

      refresh();
      SLEEP_MILLISEC(4);
    }

    setVel.angularVelocity = 0 * radian_per_second;
    myYouBotBase.getBaseJoint(1).setData(setVel);
    myYouBotBase.getBaseJoint(2).setData(setVel);
    myYouBotBase.getBaseJoint(3).setData(setVel);
    myYouBotBase.getBaseJoint(4).setData(setVel);

    endwin();
    SLEEP_MILLISEC(500);
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "unhandled exception" << std::endl;
  }

  return 0;
}
