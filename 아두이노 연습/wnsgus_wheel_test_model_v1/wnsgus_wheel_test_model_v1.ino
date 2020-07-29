#include <DynamixelWorkbench.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <stdio.h>
#define  SERIAL_CLASS USBSerial
#define VEL 30
char b[100];
int current_vel1=0;
int current_vel2=0;
DynamixelWorkbench x;
ros::NodeHandle  nh;
void msg_sender( const std_msgs::String& cmd_msg)
{ 
  sprintf(b,"%s",cmd_msg.data);
}
ros::Subscriber<std_msgs::String> sub("teleop_msg",msg_sender);
void setup() 
{
  nh.initNode();
  nh.subscribe(sub);
  //Serial.begin(57600);
  moter_count(2);
  //Serial.println("start");
}

void loop() 
{
  nh.spinOnce();
  teleop(b[0]);
}

void moter_count(int a)
{
  x.init("",1000000); //고정 
  for(int i=1;i<=a;i++)
  {
     x.ping(i);
     x.wheelMode(i, 0);
  }
}
int current_vel_sel(char c)
{
  if(c=='w')
  {
    current_vel1+=VEL;
    current_vel2+=VEL;
  }
  else if(c=='x')
  {
    current_vel1-=VEL;
    current_vel2-=VEL;
  }
  else if(c=='d')
  {
    current_vel1-=VEL;
    current_vel2+=VEL;
  }
  else if(c=='a')
  {
    current_vel1+=VEL;
    current_vel2-=VEL;
  }  
  else if(c=='s')
    {
    current_vel1=0;
    current_vel2=0;
  }
  
}
void teleop(char a)
{
  current_vel_sel(a);
  if((a=='w')||(a=='x'))
    forward(current_vel1,current_vel2);
  else if((a=='a')||(a=='d'))
    right(current_vel1,current_vel2);
  else if(a=='s')
    stop_();
}
void forward(int vel1,int vel2)
{
  x.goalVelocity(1, (int32_t)-vel1);
  x.goalVelocity(2, (int32_t)+vel2);
  
}
void right(int vel1,int vel2)
{
    x.goalVelocity(1, (int32_t)-vel1);
    x.goalVelocity(2, (int32_t)+vel2);
}

void stop_()
{
  x.goalVelocity(1, (int32_t)0);
  x.goalVelocity(2, (int32_t)0);

}
