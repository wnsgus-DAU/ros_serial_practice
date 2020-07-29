#include "ros/ros.h"
#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <unistd.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Int32MultiArray.h"
#define _speed_ 10
#define _max_speed_ 100
std_msgs::Int32MultiArray pub_msg;
int m1_speed=0,m2_speed=0;
int m1_derection,m2_derection;
void push(int a,int b,int c,int d,int e);
int getch(void);
void moving_control(char a);
void arrayCallback(const std_msgs::Int32MultiArray::ConstPtr& array);
void interface(int a,int b,int c,int d,int e);
int kbhit(void);
int Arr[6]={0,0,0,0,0,0};
int main(int argc, char **argv)
{
  ros::init(argc, argv, "wnsgus_wheel_teleop");
  ros::NodeHandle nh;
  ros::Publisher pub = nh.advertise<std_msgs::Int32MultiArray>("teleop_msg", 100);
  ros::Subscriber ultrasonic = nh.subscribe("ultrasonic_data", 1000,arrayCallback);
  while (ros::ok())
  { 
    ros::spinOnce(); 
    char a;
    a=getch();
    moving_control(a);
    pub.publish(pub_msg);
    ros::spinOnce();
  }
  return 0;
}
void moving_control(char a)
{
	
	int enable=1;
	int real_direction1=1,real_direction2=1;
	if(a=='w' || a=='W'){
           m1_derection=1;m2_derection=1;}
	else if(a=='x' || a=='X'){
	   m1_derection=-1;m2_derection=-1;}
        else if(a=='a'|| a=='A'){
	   m1_derection=-1;m2_derection=1;}
        else if(a=='d' || a=='D'){
	   m1_derection=1;m2_derection=-1;}
        else if(a=='s' || a=='S'){
	   enable=0;}
	if((enable==1)){
	   m1_speed+=m1_derection*_speed_;
	   m2_speed+=m2_derection*_speed_;}
        else if((enable==0)){	
	   m1_speed=0;
	   m2_speed=0;} 
	if((abs(m1_speed)>_max_speed_)||(abs(m2_speed)>_max_speed_))
	{
	    m1_speed=0;
	    m2_speed=0;
	}
        if((m1_speed==0)&&(m2_speed==0)){
	  enable=0;real_direction1=0;real_direction2=1;
	  real_direction1=0;real_direction2=1;}
	if(m1_speed>=0)
	   real_direction1=1;
	else if(m1_speed<0)
	   real_direction1=0;
	if(m2_speed>=0)
	   real_direction2=1;
	else if(m2_speed<0)
	   real_direction2=0;
        push(abs(m1_speed),real_direction1,abs(m2_speed),real_direction2,enable);		
}
void push(int a,int b,int c,int d,int e)
{
   pub_msg.data.clear();
   pub_msg.data.push_back(3);
   pub_msg.data.push_back(a);
   pub_msg.data.push_back(b);
   pub_msg.data.push_back(c);
   pub_msg.data.push_back(d);
   pub_msg.data.push_back(e);
   for(int i=0;i<13;i++)
	 pub_msg.data.push_back(0);
  interface(a,b,c,d,e);
}
void interface(int a,int b,int c,int d,int e)
{
     system("clear");
     printf("[wheel chair ros information]\n");
     if(e==1)
         printf("moter state : ON\n");
     else
         printf("moter state : OFF\n");
     printf("Max velocity : %d\n",_max_speed_);
     printf("velocity interval : %d\n",_speed_);
     printf("velocity : %d (m1) , %d (m2) \n",a,c);
     if(e==0)
         printf("direction : stop\n");
     else if((b==1)&&(d==1))
     {
	if(a==c)
	   printf("direction : forward\n");
	else if(a>c)
	   printf("direction : forward,right\n");
        else if(a<c)
	   printf("direction : forward,left\n");
     }
     else if((b==0)&&(d==0))
     {
         if(a==c)
	   printf("direction : back\n");
	else if(a>c)
	   printf("direction : back,right\n");
        else if(a<c)
	   printf("direction : back,left\n");
     }
     else if(b>d)
         printf("direction : right\n");
     else if(b<d)
         printf("direction : left\n");
    
     printf("signal : $3,%d,%d,%d,%d,%d,0,0,0,0,0,0,0,0,0,0,0,0,0\n",a,b,c,d,e);
     printf("sonic sensor : ");
     for(int i=0;i<6;i++)
     	printf("%dcm  ",Arr[i]);
     puts("");

}
int getch(void)
{
    int ch;
    struct termios buf, save;
    tcgetattr(0,&save);
    buf = save;
    buf.c_lflag &= ~(ICANON|ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buf);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);
    return ch;
}
void arrayCallback(const std_msgs::Int32MultiArray::ConstPtr& array)
{

	int i = 0;
	for(std::vector<int>::const_iterator it = array->data.begin(); it != array->data.end(); ++it)
	{
		Arr[i] = *it;
		i++;
	}
	return;
}
int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
 
  return 0;
}









































/*
int getch(void);
int main(int argc, char **argv)
{
  ros::init(argc, argv, "wnsgus_wheel_teleop");
  ros::NodeHandle nh;
  std_msgs::String msg;
  ros::Publisher chatter_pub = nh.advertise<std_msgs::String>("teleop_msg", 1000);
  while (ros::ok())
  {
    char a;
    a=getch();
    if(a=='w' || a=='W')
	msg.data ="$3,100,1,100,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    else if(a=='x' || a=='X')
	msg.data ="$3,100,0,100,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    else if(a=='a' || a=='A')
	msg.data ="$3,100,0,100,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    else if(a=='d' || a=='D')
 	msg.data ="$3,100,1,100,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    else if(a=='s' || a=='S')
	msg.data ="$3,100,0,100,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    chatter_pub.publish(msg);
    ros::spinOnce();
  }
  return 0;
}

int getch(void){
    int ch;
    struct termios buf, save;
    tcgetattr(0,&save);
    buf = save;
    buf.c_lflag &= ~(ICANON|ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buf);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);
    return ch;
}
/*
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <stdio.h>
#include <termio.h>
int getch(void);
int main(int argc, char **argv)
{
  ros::init(argc, argv, "wnsgus_wheel_teleop");
  ros::NodeHandle nh;
  std_msgs::String msg;
  ros::Publisher chatter_pub = nh.advertise<std_msgs::String>("teleop_msg", 1000);
  while (ros::ok())
  {
    char a;
    a=getch();
    if(a=='w' || a=='W')
	msg.data ="$3,100,1,100,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    else if(a=='x' || a=='X')
	msg.data ="$3,100,0,100,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    else if(a=='a' || a=='A')
	msg.data ="$3,100,0,100,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    else if(a=='d' || a=='D')
 	msg.data ="$3,100,1,100,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    else if(a=='s' || a=='S')
	msg.data ="$3,100,0,100,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0\n";
    chatter_pub.publish(msg);
    ros::spinOnce();
  }
  return 0;
}

int getch(void){
    int ch;
    struct termios buf, save;
    tcgetattr(0,&save);
    buf = save;
    buf.c_lflag &= ~(ICANON|ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buf);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);
    return ch;
}
*/
