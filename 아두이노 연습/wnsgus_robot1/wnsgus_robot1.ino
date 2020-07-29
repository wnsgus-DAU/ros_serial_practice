/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#include <DynamixelShield.h>
#include <stdio.h>
#include <SoftwareSerial.h>
SoftwareSerial sserial(7, 8);
SoftwareSerial serial(3, 4);



const float DXL_PROTOCOL_VERSION = 2.0;

DynamixelShield dxl;

void setup() {
  dxl.begin(1000000);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  init(2);
  serial.begin(9600);
}

void loop() {

  teleop(serial.read());
  delay(100);
  
}
void teleop(char a)
{
  if(a=='w')
    forward(300);
  else if(a=='x')
    back(300);
  else if(a=='a')
    left(300);
  else if(a=='d')
    right(300);
  else if(a=='s')
    stop_();
}
void forward(int vel)
{
  dxl.setGoalPWM(2,-vel);
  dxl.setGoalPWM(1,-vel);
}
void init(int a)
{
  for(int i=1;i<=a;i++)
  {
    dxl.ping(i);
    dxl.torqueOff(i);
    dxl.setOperatingMode(i, OP_PWM);
    dxl.torqueOn(i);
  }
}
void back(int vel)
{
  dxl.setGoalPWM(2,vel);
  dxl.setGoalPWM(1,vel);
}
void right(int vel)
{
  dxl.setGoalPWM(2,-vel);
  dxl.setGoalPWM(1,vel);
}void left(int vel)
{
  dxl.setGoalPWM(2,vel);
  dxl.setGoalPWM(1,-vel);
}
void stop_()
{
  dxl.setGoalPWM(2,0);
  dxl.setGoalPWM(1,0);
}
