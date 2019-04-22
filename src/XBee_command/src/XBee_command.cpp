#include <ros/ros.h> 
#include <serial/serial.h>  
#include <std_msgs/String.h> 
#include <std_msgs/Empty.h> 
#include "asctec.h"

uint8_t waypointNumber = 1;
serial::Serial ser; 
enum Commands{
      get_imu_data,
      get_gps_data,
      get_adv_gps_data,
      get_wp_status,
      get_ll_status,
      waypoint,
      goto_waypoint,
      launch_waypoint,
      land_waypoint,
      home_waypoint,
      waypoint_auto,
      waypoint_height,
      waypoint_height_auto,
      motor_test,
      command_invalid
};

Commands resolveCommands(std::string input){
      if( input == "get_imu_data" ) return get_imu_data;
      if( input == "get_gps_data" ) return get_gps_data;
      if( input == "get_adv_gps_data" ) return get_adv_gps_data;
      if( input == "get_wp_status" ) return get_wp_status;
      if( input == "get_ll_status" ) return get_ll_status;
      if( input == "waypoint" ) return waypoint;
      if( input == "goto_waypoint" ) return goto_waypoint;
      if( input == "launch_waypoint" ) return launch_waypoint;
      if( input == "land_waypoint" ) return land_waypoint;
      if( input == "home_waypoint" ) return home_waypoint;
      if( input == "waypoint_auto" ) return waypoint_auto;
      if( input == "waypoint_height") return waypoint_height;
      if( input == "waypoint_height_auto" ) return waypoint_height_auto;
      if( input == "motor_test" ) return motor_test;
      return command_invalid;
}

//Write commands callback
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
                        ser.write((const uint8_t *)&req, sizeof(POLL_REQUEST));   //sending serial command   
                        break;
                  }
                  case get_gps_data:{
                        ROS_INFO("gps\n");
                        POLL_REQUEST req = { { '>', '*', '>', 'p' }, GPS_Data };
                        // ROS_INFO_STREAM("Writing to serial port" <<req); 
                        ser.write((const uint8_t *)&req, sizeof(POLL_REQUEST));   //sending serial command
                        break;
                  }
                  case get_adv_gps_data:{
                        ROS_INFO("gps\n");
                        POLL_REQUEST req = { { '>', '*', '>', 'p' }, GPS_Data_Advanced };
                        // ROS_INFO_STREAM("Writing to serial port" <<req); 
                        ser.write((const uint8_t *)&req, sizeof(POLL_REQUEST));   //sending serial command
                        break;
                  }
                  case get_wp_status:{
                        ROS_INFO("wp_status\n");
                        POLL_REQUEST req = { { '>', '*', '>', 'p' }, current_way };
                        // ROS_INFO_STREAM("Writing to serial port" <<req); 
                        ser.write((const uint8_t *)&req, sizeof(POLL_REQUEST));   //sending serial command
                        break;
                  }
                  case get_ll_status:{
                        ROS_INFO("ll_status\n");
                        POLL_REQUEST req = { { '>', '*', '>', 'p' }, LL_Status };
                        // ROS_INFO_STREAM("Writing to serial port" <<req); 
                        ser.write((const uint8_t *)&req, sizeof(POLL_REQUEST));   //sending serial command
                        break;
                  }
                  case waypoint:{
                        ROS_INFO("wp\n");
                        WAYPOINT_CMD wp = { { '>', '*', '>', 'w', 's' } };
                        wp.cmd.wp_number = 1;
                        wp.cmd.properties = WPPROP_ABSCOORDS;
                        wp.cmd.max_speed = 100;
                        wp.cmd.time = 1000;
                        wp.cmd.pos_acc = 1500;
                        std::stringstream tmp_ss(command_list[1]);
                        int32_t wp_x;
                        tmp_ss>>wp_x;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.X = wp_x;
                        tmp_ss.str(command_list[2]);
                        int32_t wp_y;
                        tmp_ss>>wp_y;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.Y = wp_y;
                        wp.cmd.yaw = 0;
                        tmp_ss.str(command_list[3]);
                        int32_t wp_h;
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
                  case land_waypoint:{
                        ROS_INFO("land at current waypoint\n");
                        std::string cmd = ">*>we";
                        ser.write(cmd);
                        break;
                  }
                  case home_waypoint:{
                        ROS_INFO("back to start waypoint\n");
                        std::string cmd = ">*>wh";
                        ser.write(cmd);
                        break;
                  }
                  case waypoint_auto:{
                        ROS_INFO("wpa\n");
                        WAYPOINT_CMD wp = { { '>', '*', '>', 'w', 's' } };
                        wp.cmd.wp_number = 1;
                        wp.cmd.properties = WPPROP_AUTOMATICGOTO;
                        wp.cmd.max_speed = 100;
                        wp.cmd.time = 1000;
                        wp.cmd.pos_acc = 1000;

                        std::stringstream tmp_ss(command_list[1]);
                        int32_t wp_x;
                        tmp_ss>>wp_x;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.X = wp_x;

                        tmp_ss.str(command_list[2]);
                        int32_t wp_y;
                        tmp_ss>>wp_y;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.Y = wp_y;

                        wp.cmd.yaw = 0;

                        tmp_ss.str(command_list[3]);
                        int32_t wp_h;
                        tmp_ss>>wp_h;
                        wp.cmd.height = wp_h;
                        tmp_ss.str("");
                        tmp_ss.clear();

                        wp.cmd.chksum = 0xAAAA + wp.cmd.yaw + wp.cmd.height + wp.cmd.time + wp.cmd.X + wp.cmd.Y + wp.cmd.max_speed + wp.cmd.pos_acc + wp.cmd.properties + wp.cmd.wp_number;
                        ser.write((const uint8_t *)&wp, sizeof(WAYPOINT_CMD));
                        ROS_INFO_STREAM("latitude: " << wp_x);
                        ROS_INFO_STREAM("longitude: " << wp_y);
                        ROS_INFO_STREAM("height: " << wp_h);

                        break;
                              
                  }
                  case waypoint_height:{
                        ROS_INFO("wp_height\n");
                        WAYPOINT_CMD wp = { { '>', '*', '>', 'w', 's' } };
                        wp.cmd.wp_number = 1;
                        wp.cmd.properties = WPPROP_HEIGHTENABLED;
                        wp.cmd.max_speed = 100;
                        wp.cmd.time = 1000;
                        wp.cmd.pos_acc = 1000;

                        std::stringstream tmp_ss(command_list[1]);
                        int32_t wp_x;
                        tmp_ss>>wp_x;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.X = wp_x;

                        tmp_ss.str(command_list[2]);
                        int32_t wp_y;
                        tmp_ss>>wp_y;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.Y = wp_y;

                        wp.cmd.yaw = 0;

                        tmp_ss.str(command_list[3]);
                        int32_t wp_h;
                        tmp_ss>>wp_h;
                        wp.cmd.height = wp_h;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        
                        wp.cmd.chksum = 0xAAAA + wp.cmd.yaw + wp.cmd.height + wp.cmd.time + wp.cmd.X + wp.cmd.Y + wp.cmd.max_speed + wp.cmd.pos_acc + wp.cmd.properties + wp.cmd.wp_number;
                        ser.write((const uint8_t *)&wp, sizeof(WAYPOINT_CMD));
                        ROS_INFO_STREAM("latitude: " << wp_x);
                        ROS_INFO_STREAM("longitude: " << wp_y);
                        ROS_INFO_STREAM("height: " << wp_h);

                        break;
                              
                  }
                  case waypoint_height_auto:{
                        ROS_INFO("wp_height_auto\n");
                        WAYPOINT_CMD wp = { { '>', '*', '>', 'w', 's' } };
                        wp.cmd.wp_number = 1;
                        wp.cmd.properties = WPPROP_HEIGHTENABLED+WPPROP_AUTOMATICGOTO;
                        wp.cmd.max_speed = 100;
                        wp.cmd.time = 1000;
                        wp.cmd.pos_acc = 1000;

                        std::stringstream tmp_ss(command_list[1]);
                        int32_t wp_x;
                        tmp_ss>>wp_x;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.X = wp_x;

                        tmp_ss.str(command_list[2]);
                        int32_t wp_y;
                        tmp_ss>>wp_y;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        wp.cmd.Y = wp_y;

                        wp.cmd.yaw = 0;

                        tmp_ss.str(command_list[3]);
                        int32_t wp_h;
                        tmp_ss>>wp_h;
                        wp.cmd.height = wp_h;
                        tmp_ss.str("");
                        tmp_ss.clear();
                        
                        wp.cmd.chksum = 0xAAAA + wp.cmd.yaw + wp.cmd.height + wp.cmd.time + wp.cmd.X + wp.cmd.Y + wp.cmd.max_speed + wp.cmd.pos_acc + wp.cmd.properties + wp.cmd.wp_number;
                        ser.write((const uint8_t *)&wp, sizeof(WAYPOINT_CMD));
                        ROS_INFO_STREAM("latitude: " << wp_x);
                        ROS_INFO_STREAM("longitude: " << wp_y);
                        ROS_INFO_STREAM("height: " << wp_h);

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
                        ctrl.cmd.chksum = ctrl.cmd.pitch + ctrl.cmd.roll + ctrl.cmd.yaw + ctrl.cmd.thrust + ctrl.cmd.ctrl + 0xAAAA;
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
      }
      
} 


int main (int argc, char** argv) 
{ 

      ros::init(argc, argv, "XBee_example_node"); 

      ros::NodeHandle nh; 

      ros::Subscriber write_sub = nh.subscribe("write", 1, write_callback); 

      ros::Publisher read_pub = nh.advertise<std_msgs::String>("read", 1000); 
      try 
      { 
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

      if(ser.isOpen()) 
      { 
            ROS_INFO_STREAM("Serial Port initialized"); 
      } 
      else 
      { 
            return -1; 

      }
      ros::Rate loop_rate(10);      //Loop Rate: 10Hz

      POLL_HEADER* pHead;
      POLL_FOOTER* pFoot;

      while(ros::ok()) 
      { 
            if(ser.available()){ 
                  ROS_INFO_STREAM("Reading from serial port");
                  ROS_INFO_STREAM("full length: " << ser.available());

                  size_t size = ser.available(); 
                  std::string buf = ser.read(size);
                  std_msgs::String result;
                  result.data = buf;
                  if(size<10){
                        // ROS_INFO_STREAM("Read: " << result.data); 
                        printf("Read: %x\n", result.data[2]);
                        continue;
                  }
                  // result.data = ser.read(ser.available()); 
                  for(size_t i = 0; i < size;)
                  {
                        pHead = (POLL_HEADER*)&buf[i];
                        ROS_INFO_STREAM("Start: " << pHead->startstring);
                        ROS_INFO_STREAM("length: " << pHead->length);
                        // ROS_INFO_STREAM("packet_desc: " << (int)pHead->packet_desc);
                        printf("packet_desc: %x\n", pHead->packet_desc);
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
                                ROS_INFO_STREAM("height_reference: " << pCalcData->height_reference);

                                
                                break;
                              }

                              case PD_GPSDATA:  // PD_GPSDATA
                              {
                                GPS_DATA* pCalcData = (GPS_DATA*)&buf[i];
                                ROS_INFO_STREAM("latitude: " << pCalcData->latitude);
                                ROS_INFO_STREAM("longitude: " << pCalcData->longitude);
                                ROS_INFO_STREAM("height: " << pCalcData->height);
                                break;
                              }

                              case PD_GPSDATAADVANCED:  // PD_GPSDATA_advanced
                              {
                                GPS_DATA_ADVANCED* pCalcData = (GPS_DATA_ADVANCED*)&buf[i];
                                ROS_INFO_STREAM("latitude: " << pCalcData->latitude);
                                ROS_INFO_STREAM("longitude: " << pCalcData->longitude);
                                ROS_INFO_STREAM("height: " << pCalcData->height);
                                ROS_INFO_STREAM("latitude_best_estimate: " << pCalcData->latitude_best_estimate);
                                ROS_INFO_STREAM("longitude_best_estimate: " << pCalcData->longitude_best_estimate);
                                break;
                              }

                              case PD_CURRENTWAY:  // PD_CURRENTWAY
                              {
                                CURRENT_WAY* pCalcData = (CURRENT_WAY*)&buf[i];
                                ROS_INFO_STREAM("navigation_status: " << pCalcData->navigation_status);
                                // ROS_INFO_STREAM("status: " << pCalcData->status);
                                // ROS_INFO_STREAM("properties: " << pCalcData->properties);
                                printf("status: %d\n", pCalcData->status);
                                printf("properties: %d\n", pCalcData->properties);
                                ROS_INFO_STREAM("distance_to_wp: " << pCalcData->distance_to_wp);
                                ROS_INFO_STREAM("number_of_wp: " << pCalcData->nr_of_wp);
                                // printf("distance_to_wp: %d\n", pCalcData->distance_to_wp);
                                // printf("number_of_wp: %d\n", pCalcData->nr_of_wp);
                                printf("current_wp: %d\n", pCalcData->current_wp);
                                printf("current_wp_memlocation: %d\n", pCalcData->current_wp_memlocation);
                                //ROS_INFO_STREAM("current_wp: " << pCalcData->current_wp);
                                //ROS_INFO_STREAM("current_wp_memlocation: " << pCalcData->current_wp_memlocation);
                                break;
                              }

                              case PD_LLSTATUS:  // PD_LLSTATUS
                              {
                                LL_STATUS* pCalcData = (LL_STATUS*)&buf[i];
                                ROS_INFO_STREAM("battery_voltage_1: " << pCalcData->battery_voltage_1);
                                ROS_INFO_STREAM("battery_voltage_2: " << pCalcData->battery_voltage_2);
                                ROS_INFO_STREAM("up_time: " << pCalcData->up_time);
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

            ros::spinOnce(); 
            loop_rate.sleep(); 
      } 
} 
