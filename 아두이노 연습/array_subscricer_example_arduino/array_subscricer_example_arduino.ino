


#include <ros.h>

#include <std_msgs/Int32MultiArray.h>


int data[19];


ros::NodeHandle  nh;


void callback( const std_msgs::Int32MultiArray& cmd_msg)
{
  for(int i=0;i<19;i++)
  {
    data[i]= cmd_msg.data[i];
    
  }
  
}


ros::Subscriber<std_msgs::Int32MultiArray> sub("teleop_msg", callback);



void setup()
{
  
  nh.initNode();

  nh.subscribe(sub);

}



void loop()
{
 
  nh.spinOnce();
  
 
}
