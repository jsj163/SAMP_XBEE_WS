#include <ros/ros.h> 
#include <std_msgs/String.h> 
#include <std_msgs/Empty.h> 
#include "asctec.h"
#include "sensor_msgs/NavSatFix.h"
#include "XBee_command/Temperature.h"
#include <math.h>

int waypointNumber = 1;
int test_wp_num = 4;
bool tmp_converg_flag = true;      //flag to detect the converge of temperature sensor
bool gps_converg_flag = true;      //flag to detect the converge of GPS sensor
bool home_flag = true;
std::vector<double> lat_buff;
std::vector<double> long_buff;
std::vector<double> tmp_buff;



void gps_callback(const sensor_msgs::NavSatFix::ConstPtr& msg) {
      if(!gps_converg_flag){
            double newlatitude = msg->latitude*pow(10,7);
            double newlongitude = msg->longitude*pow(10,7);
            // ROS_INFO("I am in GPS gps_callback");
            lat_buff.push_back(newlatitude);
            long_buff.push_back(newlongitude);
            
            if(lat_buff.size()>5){
                  lat_buff.erase(lat_buff.begin());
                  long_buff.erase(long_buff.begin());
                  bool dist_flag = true;       //flag to calculate the distance threshold
                  double distance = 0;
                  for(int i = 0; i < lat_buff.size()-1;++i){
                        distance = sqrt(pow(lat_buff[i+1]-lat_buff[i],2)+pow(long_buff[i+1]-long_buff[i],2));
                        if(distance>1500)       //Threshold = 1.5m
                        {
                              dist_flag = false;
                              break;
                        }
                  }

                  if(dist_flag){                //GPS converge, start temperature sampling
                        ROS_INFO("GPS converge.");
                        ROS_INFO_STREAM("Converged latitude: " <<std::setprecision(9)<< msg->latitude <<" Converged longitude: "<<msg->longitude<<"\n");
                        gps_converg_flag = true;
                  }
            }
      }
} 


void tmp_callback(const XBee_command::Temperature::ConstPtr& msg) {
      if(gps_converg_flag && !tmp_converg_flag){
            double new_tmp = msg->data;

            tmp_buff.push_back(new_tmp);

            if(tmp_buff.size()>5){
                  tmp_buff.erase(tmp_buff.begin());

                  bool tmp_err_flag = true;       //flag to calculate the temperature error threshold
                  double tmp_err = 0;
                  for(int i = 0; i < tmp_buff.size()-1;++i){
                        tmp_err = tmp_buff[i+1]-tmp_buff[i];
                        if(tmp_err>0.1)       //Threshold = 0.2 degree
                        {
                              tmp_err_flag = false;
                              break;
                        }
                  }

                  if(tmp_err_flag){                //Temperature converge, start next WP
                        ROS_INFO("Temperature converge, start next task.");
                        ROS_INFO_STREAM("Converged Temperature: "<<msg->data<<"\n");

                        tmp_converg_flag = true;
                  }
            }
      }
} 


int main (int argc, char** argv) 
{ 

      ros::init(argc, argv, "SVD_NAVI_NODE"); 

      ros::NodeHandle nh; 

      ros::Subscriber tmp_sub = nh.subscribe("uav/temp", 1, tmp_callback); 
      ros::Subscriber gps_sub = nh.subscribe("fcu/gps", 1, gps_callback); 


      // ros::Publisher cmd_pub = nh.advertise<std_msgs::String>("wp_cmd", 1); 
      ros::Publisher cmd_pub = nh.advertise<std_msgs::String>("write", 1); 


      ros::Rate loop_rate(10);
      std::string wp_command;
      std_msgs::String ros_wp_cmd;


      while(ros::ok()) 
      {     
            if(gps_converg_flag && tmp_converg_flag && waypointNumber<=test_wp_num && home_flag){
                  ros::Duration(5).sleep();   
                  if(waypointNumber == 1)
                        wp_command = "launch_waypoint";
                  // wp_command = "waypoint1";
                  else
                  {
                        wp_command = "waypoint_height_auto,0,-5000,3500";
                        gps_converg_flag = false;
                        tmp_converg_flag = false;
                        lat_buff.clear();
                        long_buff.clear();
                        tmp_buff.clear();
                  }

                  // waypointNumber++; 
                  // wp_command.push_back('0'+(waypointNumber++));
                  ros_wp_cmd.data = wp_command;
                  cmd_pub.publish(ros_wp_cmd);
                  // printf("%s\n",wp_command );
                  std::cout<<wp_command<<std::endl;

                  // ROS_INFO("Going to %s \n",wp_command);
                  
                  ros::Duration(10).sleep();
                  waypointNumber++;
                  home_flag = false;

            }

            else if(gps_converg_flag && tmp_converg_flag && waypointNumber <= test_wp_num && !home_flag){
                  ros::Duration(5).sleep();   

                  wp_command = "waypoint_height_auto,0,0,4000";
                  ros_wp_cmd.data = wp_command;
                  cmd_pub.publish(ros_wp_cmd);
                  std::cout<<wp_command<<std::endl;
                  ros::Duration(5).sleep();

                  wp_command = "home_waypoint";
                  ros_wp_cmd.data = wp_command;
                  cmd_pub.publish(ros_wp_cmd);
                  std::cout<<wp_command<<std::endl;
                  ros::Duration(12).sleep();
                  home_flag = true;
                  // waypointNumber++;
            }

            else if(gps_converg_flag && tmp_converg_flag && waypointNumber == test_wp_num+1){

                  // wp_command = "waypoint_height_auto,0,0,2500";
                  // ros_wp_cmd.data = wp_command;
                  // cmd_pub.publish(ros_wp_cmd);
                  // std::cout<<wp_command<<std::endl;
                  // ros::Duration(5).sleep();


                  wp_command = "land_waypoint";
                  ros_wp_cmd.data = wp_command;
                  cmd_pub.publish(ros_wp_cmd);
                  std::cout<<wp_command<<std::endl;
                  tmp_buff.clear();
                  tmp_converg_flag = false;
                  ros::Duration(10).sleep();

                  waypointNumber++; 
                  ROS_INFO("Finished Task!");
            }
            ros::spinOnce(); 
            loop_rate.sleep(); 
      } 
} 
