#include <DynamixelWorkbench.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int32MultiArray.h>
#include <stdio.h>
#define  SERIAL_CLASS USBSerial
#define VEL 30
#define NUM 4
char b[100];
char derec[2];
int sobic_data[NUM];
int current_vel1=0;
int current_vel2=0;
DynamixelWorkbench x;
ros::NodeHandle  nh;
void callback( const std_msgs::Int32MultiArray& cmd_msg)
{
  for(int i=0;i<NUM;i++)
    sobic_data[i]= cmd_msg.data[i];
    
  
}
void msg_sender( const std_msgs::String& cmd_msg)
{ 
  sprintf(b,"%s",cmd_msg.data);
}
ros::Subscriber<std_msgs::String> sub("teleop_msg",msg_sender);
ros::Subscriber<std_msgs::Int32MultiArray> sub2("ultrasonic", callback);
void setup() 
{
  nh.initNode();
  nh.subscribe(sub);
  moter_count(2);
  nh.subscribe(sub2);
}

void loop() 
{
  nh.spinOnce();
  sonic_detection(b[0]);
  nh.spinOnce();
  teleop(b[0]);
  b[0]='k';
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
  if(c=='w'&&(sonic_key_check(c)))
  {
    current_vel1+=VEL;
    current_vel2+=VEL;
    derec[0]='f';
    
  }
  else if(c=='x'&&(sonic_key_check(c)))
  {
    current_vel1-=VEL;
    current_vel2-=VEL;
    derec[0]='b';
  }
  else if(c=='d'&&(sonic_key_check(c)))
  {
    current_vel1-=VEL;
    current_vel2+=VEL;
    derec[1]='r';
  }
  else if(c=='a'&&(sonic_key_check(c)))
  {
    current_vel1+=VEL;
    current_vel2-=VEL;
    derec[1]='l';
  }  
  else if(c=='s')
    {
    current_vel1=0;
    current_vel2=0;
    derec[0]='q';
    derec[1]='q';
  }
  else if(c=='q')
    {
    current_vel1=0;
    current_vel2=0;
    } 
  
}
void teleop(char a)
{
  current_vel_sel(a);
  if((sonic_key_check(a))&&((a=='w')||(a=='x')))
    forward(current_vel1,current_vel2);
  else if((sonic_key_check(a))&&((a=='a')||(a=='d')))
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
bool sonic_key_check(char a)
{
  if((sobic_data[0]<15)&&a=='w')
    return false;
  if((sobic_data[2]<15)&&a=='x')
    return false;
  if((sobic_data[4]<15)&&a=='d')
    return false;
  if((sobic_data[1]<15)&&a=='a')
    return false;  
  return true;
}
void sonic_detection(char a)
{
  nh.spinOnce();
  if((sobic_data[0]<15)||(sobic_data[1]<15)||(sobic_data[2]<15)||(sobic_data[3]<15))
  {
          stop_();
     while((sobic_data[0]<15)||(sobic_data[1]<15)||(sobic_data[2]<15)||(sobic_data[3]<15))
     {
        nh.spinOnce();
        teleop(b[0]);
        b[0]='q';
     }
  }
}
