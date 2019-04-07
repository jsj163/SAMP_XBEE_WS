#include <ros/ros.h> 
#include <serial/serial.h>  //ROS已经内置了的串口包 
#include <std_msgs/String.h> 
#include <std_msgs/Empty.h> 
#include "asctec.h"
serial::Serial ser; //声明串口对象 
//回调函数 
void write_callback(const std_msgs::String::ConstPtr& msg) 
{ 
      std::stringstream write_buf(msg->data);
      std::string 
      // write_buf.erase(0,1);   //reduce '\'
      uint16_t packets;
      write_buf >> packets;
      POLL_REQUEST req = { { '>', '*', '>', 'p' }, packets };
      // ROS_INFO_STREAM("Writing to serial port" <<msg->data); 
      ROS_INFO_STREAM("Writing to serial port" <<write_buf); 
      // ser.write(msg->data);   //发送串口数据 
      ser.write((const uint8_t *)&req, sizeof(POLL_REQUEST));   //发送串口数据 
} 
int main (int argc, char** argv) 
{ 
      //初始化节点 
      ros::init(argc, argv, "XBee_example_node"); 
      //声明节点句柄 
      ros::NodeHandle nh; 
      //订阅主题，并配置回调函数 
      ros::Subscriber write_sub = nh.subscribe("write", 1000, write_callback); 
      //发布主题 
      ros::Publisher read_pub = nh.advertise<std_msgs::String>("read", 1000); 
      try 
      { 
            //设置串口属性，并打开串口 
            ser.setPort("/dev/ttyUSB0"); 
            ser.setBaudrate(57600); 
            serial::Timeout to = serial::Timeout::simpleTimeout(1000); 
            ser.setTimeout(to); 
            ser.open(); 
      } 
      catch (serial::IOException& e) 
      { 
            ROS_ERROR_STREAM("Unable to open port "); 
            return -1; 
      } 
      //检测串口是否已经打开，并给出提示信息 
      if(ser.isOpen()) 
      { 
            ROS_INFO_STREAM("Serial Port initialized"); 
      } 
      else 
      { 
            return -1; 
      } 
      //指定循环的频率 
      ros::Rate loop_rate(10);

      POLL_HEADER* pHead;
      POLL_FOOTER* pFoot;

      while(ros::ok()) 
      { 
            if(ser.available()){ 
                  ROS_INFO_STREAM("Reading from serial port");
                  ROS_INFO_STREAM("full length: " << ser.available());

                  size_t size = ser.available(); 
                  std::string buf = ser.read(size);; 
                  std_msgs::String result;
                  result.data = buf;
                  // result.data = ser.read(ser.available()); 
                  for(size_t i = 0; i < size;)
                  {
                        pHead = (POLL_HEADER*)&buf[i];
                        // ROS_INFO_STREAM("Start: " << pHead->startstring);
                        // ROS_INFO_STREAM("length: " << pHead->length);
                        // ROS_INFO_STREAM("packet_desc: " << (int)pHead->packet_desc);
                        // printf("Start: %.3s\n", pHead->startstring);
                        // printf("packet desc: %hhu / 0x%02hhX\n", pHead->packet_desc, pHead->packet_desc);
                        // printf("length: %hu\n", pHead->length);

                        i += sizeof(POLL_HEADER);
                        // printf("header_size: %hu\n", sizeof(POLL_HEADER));

                        switch(pHead->packet_desc)
                        {
                              case 0x03:  // PD_IMUCALCDATA
                              {
                                IMU_CALCDATA* pCalcData = (IMU_CALCDATA*)&buf[i];
                                ROS_INFO_STREAM("angle_nick: " << pCalcData->angle_nick);
                                ROS_INFO_STREAM("angle_roll: " << pCalcData->angle_roll);
                                ROS_INFO_STREAM("angle_yaw: " << pCalcData->angle_yaw);
                              }
                                break;
                        }

                        i += pHead->length;

                        pFoot = (POLL_FOOTER*)&buf[i];
                        ROS_INFO_STREAM("crc: " << pFoot->crc16);
                        ROS_INFO_STREAM("stop string: " << pFoot->stopstring);
                        // printf("crc: %04hX\n", pFoot->crc16);
                        // printf("stop string: %.3s\n\n", pFoot->stopstring);

                        i += sizeof(POLL_FOOTER);
                        // printf("footer_size: %hu\n", sizeof(POLL_FOOTER));
                  }
                  // ROS_INFO_STREAM("Read: " << result.data); 
                  ROS_INFO_STREAM("\n");
                  read_pub.publish(result); 
            } 

            //处理ROS的信息，比如订阅消息,并调用回调函数 
            ros::spinOnce(); 
            loop_rate.sleep(); 
      } 
} 
