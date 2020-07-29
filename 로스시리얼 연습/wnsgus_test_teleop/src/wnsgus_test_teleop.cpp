#include "ros/ros.h"
#include "std_msgs/String.h"
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
std_msgs::Int32MultiArray pub_msg;
void arrayCallback(const std_msgs::Int32MultiArray::ConstPtr& array);
int Arr[4];
int getch(void);
int main(int argc, char **argv)
{ 
  printf("start\n");
  ros::init(argc, argv, "wnsgus_test_teleop");
  ros::NodeHandle nh;
  ros::Publisher teleop_pub = nh.advertise<std_msgs::String>("teleop_msg", 1000);
  ros::Subscriber ultrasonic = nh.subscribe("ultrasonic_data", 1000,arrayCallback);
  while (ros::ok())
  {  
    ros::spinOnce();
    char a;
    a=getch();
    std_msgs::String teleop_msg;
    teleop_msg.data =a;
    printf("%c send \n",a);
    printf("sonic %d %d %d %d \n",Arr[0],Arr[1],Arr[2],Arr[3]);
    teleop_pub.publish(teleop_msg);
  }

  return 0;
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
