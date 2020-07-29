#include <ros.h>
#include <std_msgs/Int32MultiArray.h>
#include <stdlib.h>
#include <std_msgs/MultiArrayLayout.h>
#include <std_msgs/MultiArrayDimension.h>
#define NUM 4
int trigPin[NUM]={2,3,4,5};    //Trig 핀 할당
int echoPin[NUM]={6,7,8,9};    //Echo 핀 할당
ros::NodeHandle nh;
std_msgs::Int32MultiArray sonic_msg;
ros::Publisher ultrasonic("ultrasonic", &sonic_msg);
void setup()
{
    Serial.begin(115200);    //시리얼 초기화
    Serial.println("Hello Arduino!!");
    nh.initNode();
    sonic_msg.data_length = NUM;
    sonic_msg.data = (int32_t *)malloc(sizeof(int32_t)*NUM);
    nh.advertise(ultrasonic); 
    for(int i=0;i<NUM;i++)
    {
      pinMode(trigPin[i], OUTPUT);    //Trig 핀 output으로 세팅
      pinMode(echoPin[i], INPUT);    //Echo 핀 input으로 세팅
    }
}
 
void loop()
{
  for(int i=0;i<NUM;i++)
  {
    long duration[NUM], distance[NUM];    //기본 변수 선언
 
    //Trig 핀으로 10us의 pulse 발생
    
   
      digitalWrite(trigPin[i], LOW);        //Trig 핀 Low
      delayMicroseconds(2);            //2us 유지
      digitalWrite(trigPin[i], HIGH);    //Trig 핀 High
      delayMicroseconds(10);            //10us 유지
      digitalWrite(trigPin[i], LOW);        //Trig 핀 Low

 
    //Echo 핀으로 들어오는 펄스의 시간 측정
    
       duration[i] = pulseIn(echoPin[i], HIGH);        //pulseIn함수가 호출되고 펄스가 입력될 때까지의 시간. us단위로 값을 리턴.
 
    //음파가 반사된 시간을 거리로 환산
    //음파의 속도는 340m/s 이므로 1cm를 이동하는데 약 29us.
    //따라서, 음파의 이동거리 = 왕복시간 / 1cm 이동 시간 / 2 이다.
   
        distance[i] = duration[i] / 29 / 2;        //센치미터로 환산
 
      sonic_msg.data[i]=distance[i];
  }
      ultrasonic.publish(&sonic_msg);
      nh.spinOnce();

}
