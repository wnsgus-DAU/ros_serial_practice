#include <AltSoftSerial.h>
#include <ros.h>
#include <std_msgs/Int32MultiArray.h>
#include <stdlib.h>
#include <std_msgs/MultiArrayLayout.h>
#include <std_msgs/MultiArrayDimension.h>
// AltSoftSerial always uses these pins:
//
// Board          Transmit  Receive   PWM Unusable
// -----          --------  -------   ------------
// Teensy 3.0 & 3.1  21        20         22
// Teensy 2.0         9        10       (none)
// Teensy++ 2.0      25         4       26, 27
// Arduino Uno        9         8         10
// Arduino Leonardo   5        13       (none)
// Arduino Mega      46        48       44, 45
// Wiring-S           5         6          4
// Sanguino          13        14         12

AltSoftSerial altSerial;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 1;           // interval at which to blink (milliseconds)


#define NOT_STARTED 0
#define WAIT_FOR_SIGNAL 1
#define MAX_WAIT 1000



#define TRIGGER1_PIN A0
#define TRIGGER2_PIN A1
#define TRIGGER3_PIN A2
#define TRIGGER4_PIN A3
#define TRIGGER5_PIN A4
#define TRIGGER6_PIN A5

#define DISTANCE_0_5M 0x11
#define DISTANCE_1_0M 0x12
#define DISTANCE_1_5M 0x13
#define DISTANCE_2_0M 0x14
#define DISTANCE_2_5M 0x15
#define DISTANCE_3_0M 0x16
#define DISTANCE_4_0M 0x17
#define DISTANCE_5_0M 0x18

#define DISTANCE DISTANCE_5_0M

//0x11 :0.5
//0x12 :1.0
//0x13 :1.5
//0x14 :2.0
//0x15 :2.5
//0x16 :3.0
//0x17 :4.0
//0x18 :5.0
ros::NodeHandle nh;
std_msgs::Int32MultiArray sonic_msg;
ros::Publisher ultrasonic("ultrasonic", &sonic_msg);
int real_data_storage[6];
unsigned char sensor_index = 0;
unsigned char n_sensor ;
unsigned char  state = NOT_STARTED;
unsigned char index = 0; // buffer index
char strBuffer[8];      //receive index
unsigned int wait_count = 0;

//unsigned char trigger_pins[] = { TRIGGER1_PIN,TRIGGER2_PIN,TRIGGER3_PIN,TRIGGER4_PIN,TRIGGER5_PIN,TRIGGER6_PIN   };
unsigned char trigger_pins[] = { TRIGGER1_PIN, TRIGGER2_PIN, TRIGGER3_PIN, TRIGGER4_PIN, TRIGGER5_PIN, TRIGGER6_PIN   };
bool trigger_pins_enable[] = { true, true, true, true, true, true    };


void next_sensor()
{

  int new_sensor_index = sensor_index;
  for (unsigned char i = 0; i < n_sensor; i++)
  {
    new_sensor_index = (new_sensor_index + 1) % n_sensor;
    if (trigger_pins_enable[new_sensor_index])
    {
      sensor_index =  new_sensor_index;
      break;
    }


  }

}

void first_sensor()
{
  for ( unsigned char i = 0 ; i < n_sensor ; i++)
  {
    if (trigger_pins_enable[ i  ])
    {
      sensor_index = i;
    }

  }

}


void send_signal( int pin , char cmd)
{
  const int BIT_RATE_DELAY = 104;
  //start bit
  digitalWrite(pin, 0);
  delayMicroseconds(BIT_RATE_DELAY);
  char mask  = 0x01;
  for (unsigned char i = 0; i < 8; i++ , mask <<= 1)
  {
    digitalWrite(pin, mask &  cmd);
    delayMicroseconds(BIT_RATE_DELAY);
  }
  //stop bit
  digitalWrite(pin, 1);
  delayMicroseconds(BIT_RATE_DELAY);


}

void send_trigger( int pin)
{
  send_signal(pin, 0);
}



void setup() {
  n_sensor = sizeof(trigger_pins ) / sizeof(trigger_pins[0] );

  delay(100); // wait for sensor
  Serial.begin(9600);

  while (!Serial) ; // wait for Arduino Serial Monitor to open
  Serial.println("STMA-506ND TEST");

  // SOFT SERIAL
  altSerial.begin(9600);
  pinMode(8, INPUT_PULLUP);


  // SET DISTANCE MODE FOR ALL SENSOR
  for (unsigned char i = 0; i < n_sensor; i++)
  {
    pinMode(trigger_pins[i], OUTPUT);
    delayMicroseconds(10);
    send_signal( trigger_pins[i ],   (char)0x32  );

    delayMicroseconds(10);

    // set distance
    send_signal( trigger_pins[i ],   DISTANCE  );

    

  }
  
  nh.initNode();
  sonic_msg.data_length = 6;
  sonic_msg.data = (int32_t *)malloc(sizeof(int32_t)*6);
  nh.advertise(ultrasonic);        
  first_sensor();

}




void loop() {
  char c;
 
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    switch (state)
    {


      case NOT_STARTED:
        //Serial.print((int)sensor_index);
        //Serial.print(':');

        send_trigger( trigger_pins[sensor_index   ]  );
        index = 0;
        state = WAIT_FOR_SIGNAL;
        wait_count = 0;
        break;

      case WAIT_FOR_SIGNAL:


        while (altSerial.available()) {
          strBuffer[index++] = altSerial.read();
        }

        if (index == 3)
        {
          strBuffer[index] = '\0';
          sonic_msg.data[sensor_index]=atoi(strBuffer);
          next_sensor();
          state = NOT_STARTED;
        }


        if (++ wait_count >= MAX_WAIT)
        {
          Serial.println("NO_SIGNAL!!");
          next_sensor();

          state = NOT_STARTED;
        }

        break;

    }

  }
  ultrasonic.publish(&sonic_msg);
  nh.spinOnce();
  
}
