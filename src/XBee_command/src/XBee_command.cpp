#include <ros/ros.h> 
#include <serial/serial.h>  //ROS已经内置了的串口包 
#include <std_msgs/String.h> 
#include <std_msgs/Empty.h> 
#include "asctec.h"

uint8_t waypointNumber = 1;
serial::Serial ser; //声明串口对象 
enum Commands{
      get_imu_data,
      get_gps_data,
      waypoint,
      goto_waypoint,
      launch_waypoint,
      waypoint_auto,
      motor_test,
      command_invalid
};

Commands resolveCommands(std::string input){
      if( input == "get_imu_data" ) return get_imu_data;
      if( input == "get_gps_data" ) return get_gps_data;
      if( input == "waypoint" ) return waypoint;
      if( input == "goto_waypoint" ) return goto_waypoint;
      if( input == "launch_waypoint" ) return launch_waypoint;
      if( input == "waypoint_auto" ) return waypoint_auto;
      if( input == "motor_test" ) return motor_test;
      return command_invalid;
}
//回调函数 
void write_callback(const std_msgs::String::ConstPtr& msg) {
      if (msg->data.length()>0){
            std::stringstream write_buf(msg->data);
            std::string tmp;
            std::string command_list[50];
            int i = 0;

            while (std::getline(write_buf, tmp, ',')){
                  command_list[i] = tmp;
                  i++;
            }
            Commands check_command = resolveCommands(command_list[0]) ;

            switch(check_command){
                  case get_imu_data:{
                        ROS_INFO("imu\n");
                        POLL_REQUEST req = { { '>', '*', '>', 'p' }, IMU_CalcData };
                        // ROS_INFO_STREAM("Writing to serial port" <<req); 
                        ser.write((const uint8_t *)&req, sizeof(POLL_REQUEST));   //发送串口数据   
                        break;
                  }
                  case get_gps_data:{
                        ROS_INFO("gps\n");
                        POLL_REQUEST req = { { '>', '*', '>', 'p' }, GPS_Data };
                        // ROS_INFO_STREAM("Writing to serial port" <<req); 
                        ser.write((const uint8_t *)&req, sizeof(POLL_REQUEST));   //发送串口数据
                        break;
                        
                  }
                  case waypoint:{
                        ROS_INFO("wp\n");
                        WAYPOINT_CMD wp = { { '>', '*', '>', 'w', 's' } };
                        wp.cmd.wp_number = 1;
                        wp.cmd.properties = WPPROP_ABSCOORDS;
                        wp.cmd.max_speed = 100;
                        wp.cmd.time = 0.01;
                        wp.cmd.pos_acc = 2500;
                        std::stringstream tmp_ss(command_list[1]);
                        uint8_t wp_x;
                        tmp_ss>>wp_x;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.X = wp_x;
                        tmp_ss.str(command_list[2]);
                        uint8_t wp_y;
                        tmp_ss>>wp_y;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.Y = wp_y;
                        wp.cmd.yaw = 0;
                        tmp_ss.str(command_list[3]);
                        uint8_t wp_h;
                        tmp_ss>>wp_h;
                        wp.cmd.height = wp_h;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.chksum = 0xAAAA + wp.cmd.yaw + wp.cmd.height + wp.cmd.time + wp.cmd.X + wp.cmd.Y + wp.cmd.max_speed + wp.cmd.pos_acc + wp.cmd.properties + wp.cmd.wp_number;
                        ser.write((const uint8_t *)&wp, sizeof(WAYPOINT_CMD));
                        break;
                        
                  }
                  case goto_waypoint:{
                        ROS_INFO("wp_goto\n");
                        std::string cmd = ">*>wg";
                        ser.write(cmd);
                        break;
                  }
                  case launch_waypoint:{
                        ROS_INFO("launch waypoint\n");
                        std::string cmd = ">*>wl";
                        ser.write(cmd);
                        break;
                  }
                  
                  case waypoint_auto:{
                        ROS_INFO("wpa\n");
                        WAYPOINT_CMD wp = { { '>', '*', '>', 'w', 's' } };
                        wp.cmd.wp_number = 1;
                        wp.cmd.properties = WPPROP_AUTOMATICGOTO;
                        wp.cmd.max_speed = 100;
                        wp.cmd.time = 0.01;
                        wp.cmd.pos_acc = 2500;
                        std::stringstream tmp_ss(command_list[1]);
                        uint8_t wp_x;
                        tmp_ss>>wp_x;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.X = wp_x;
                        tmp_ss.str(command_list[2]);
                        uint8_t wp_y;
                        tmp_ss>>wp_y;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.Y = wp_y;
                        wp.cmd.yaw = 0;
                        tmp_ss.str(command_list[3]);
                        uint8_t wp_h;
                        tmp_ss>>wp_h;
                        wp.cmd.height = wp_h;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.chksum = 0xAAAA + wp.cmd.yaw + wp.cmd.height + wp.cmd.time + wp.cmd.X + wp.cmd.Y + wp.cmd.max_speed + wp.cmd.pos_acc + wp.cmd.properties + wp.cmd.wp_number;
                        ser.write((const uint8_t *)&wp, sizeof(WAYPOINT_CMD));
                        break;
                              
                  }
                  case motor_test:{
                        ROS_INFO("motor_test\n");
                        CTRL_INPUT_CMD ctrl = { { '>', '*', '>', 'd', 'i' } };
                        ctrl.cmd.ctrl = 0x0F; // enable pitch/roll/yaw/thrust commands
                        ctrl.cmd.pitch = 0;
                        ctrl.cmd.roll = 0;
                        ctrl.cmd.yaw = 0;
                        std::stringstream tmp_ss(command_list[1]);
                        int16_t thrust;
                        tmp_ss>>thrust;
                        ctrl.cmd.thrust = thrust;
                        ser.write((const uint8_t *)&ctrl, sizeof(CTRL_INPUT_CMD));
                        break;
                              
                  }
                  case command_invalid:{
                        ROS_INFO("command_invalid\n");
                        break;
                              
                  }
                  default:
                        break;
            }

            // write_buf.erase(0,1);   //reduce '\'
            // uint16_t packets;
            // write_buf >> packets;
            // POLL_REQUEST req = { { '>', '*', '>', 'p' }, packets };
            // // ROS_INFO_STREAM("Writing to serial port" <<msg->data); 
            // ROS_INFO_STREAM("Writing to serial port" <<write_buf); 
            // ser.write(msg->data);   //发送串口数据 
            // ser.write((const uint8_t *)&req, sizeof(POLL_REQUEST));   //发送串口数据   
      }
      
} 
int main (int argc, char** argv) 
{ 
      //初始化节点 
      ros::init(argc, argv, "XBee_example_node"); 
      //声明节点句柄 
      ros::NodeHandle nh; 
      //订阅主题，并配置回调函数 
      ros::Subscriber write_sub = nh.subscribe("write", 1, write_callback); 
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
                              case PD_IMUCALCDATA:  // PD_IMUCALCDATA
                              {
                                IMU_CALCDATA* pCalcData = (IMU_CALCDATA*)&buf[i];
                                ROS_INFO_STREAM("angle_nick: " << pCalcData->angle_nick);
                                ROS_INFO_STREAM("angle_roll: " << pCalcData->angle_roll);
                                ROS_INFO_STREAM("angle_yaw: " << pCalcData->angle_yaw);
                                ROS_INFO_STREAM("height: " << pCalcData->height);
                                break;
                              }
                              case PD_GPSDATA:  // PD_IMUCALCDATA
                              {
                                GPS_DATA* pCalcData = (GPS_DATA*)&buf[i];
                                ROS_INFO_STREAM("latitude: " << pCalcData->latitude);
                                ROS_INFO_STREAM("longitude: " << pCalcData->longitude);
                                ROS_INFO_STREAM("height: " << pCalcData->height);
                                break;
                              }

                              default:
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
