#include "ros/ros.h"
#include "std_msgs/Int32MultiArray.h"
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#define NUM 4
using namespace std;
std_msgs::Int32MultiArray msg;
void arrayCallback(const std_msgs::Int32MultiArray::ConstPtr& array);
void sonic_num(int *a,int len);
int Arr[NUM];
int main(int argc, char **argv)
{
  ROS_INFO("start\n");
  ros::init(argc, argv, "wnsgus_sonic");
  ros::NodeHandle nh;
  ros::Publisher pub = nh.advertise<std_msgs::Int32MultiArray>("ultrasonic_data", 100);
  ros::Subscriber ultrasonic = nh.subscribe("ultrasonic", 1000,arrayCallback);
  
  while(ros::ok())
  {      
	  msg.data.clear();
	  ros::spinOnce();
	  sonic_num(Arr,6); 
          pub.publish(msg);
  }
  return 0;
}
void arrayCallback(const std_msgs::Int32MultiArray::ConstPtr& array)
{
	int i = 0;
	for(std::vector<int>::const_iterator it = array->data.begin(); it != array->data.end(); ++it)
	{
		Arr[i] = *it;
		i++;
	}
	for(int j = 0; j <NUM ; j++)
	  {
		printf("%d번 센서 %dcm\n",j+1, Arr[j]);
	  }
	  puts("");

	return;
}
void sonic_num(int *a,int len)
{
    for(int i=0;i<len;i++)
    	msg.data.push_back(a[i]);   
}
