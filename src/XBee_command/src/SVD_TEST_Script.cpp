#include <ros/ros.h> 
#include <std_msgs/String.h> 
#include <std_msgs/Empty.h> 
#include "asctec.h"

char cmd_flag = 0;
std::string cmd_to_publish;

enum Commands{
      waypoint1,
      waypoint2,
      waypoint3,
      waypoint4,
      waypoint5,
      go_home,
      land,
      command_invalid
};

Commands resolveCommands(std::string input){
      if( input == "waypoint1" ) return waypoint1;
      if( input == "waypoint2" ) return waypoint2;
      if( input == "waypoint3" ) return waypoint3;
      if( input == "waypoint4" ) return waypoint4;
      if( input == "waypoint5" ) return waypoint5;
      if( input == "go_home" ) return go_home;
      if( input == "land" ) return land;
      return command_invalid;
}


void write_callback(const std_msgs::String::ConstPtr& msg) {
      if (msg->data.length()>0){

            std::string tmp = msg->data;

            Commands check_command = resolveCommands(tmp) ;

            switch(check_command){
                  case waypoint1:{
                        ROS_INFO("going to waypoint 1\n");
                        cmd_to_publish = "waypoint_auto,0,-5000,-1000";      //GPS LAT & LON & HEIGHT OF WP1
                        cmd_flag = 1;
                        break;
                  }
                  case waypoint2:{
                        ROS_INFO("going to waypoint 2\n");
                        cmd_to_publish = "waypoint_auto,404433303,-799428605,2000";      //GPS LAT & LON & HEIGHT OF WP1
                        cmd_flag = 1;
                        break;
                  }
                  case waypoint3:{
                        ROS_INFO("going to waypoint 3\n");
                        cmd_to_publish = "waypoint_auto,404434300,-799429400,2000";      //GPS LAT & LON & HEIGHT OF WP1
                        cmd_flag = 1;
                        break;
                  }
                  case waypoint4:{
                        ROS_INFO("going to waypoint 4\n");
                        cmd_to_publish = "waypoint_auto,404433520,-799429860,1000";      //GPS LAT & LON & HEIGHT OF WP1
                        cmd_flag = 1;
                        break;
                  }
                  case waypoint5:{
                        ROS_INFO("going to waypoint 5\n");
                        cmd_to_publish = "waypoint_auto,404433520,-799429860,1000";      //GPS LAT & LON & HEIGHT OF WP1
                        cmd_flag = 1;
                        break;
                  }
                  case go_home:{
                        ROS_INFO("go to start point\n");
                        cmd_to_publish = "home_waypoint";
                        cmd_flag = 2;
                        break;
                  }
                  case land:{
                        ROS_INFO("Landing\n");
                        cmd_to_publish = "land_waypoint";
                        cmd_flag = 2;
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

      ros::init(argc, argv, "WP_TEST_NODE"); 

      ros::NodeHandle nh; 

      ros::Subscriber write_sub = nh.subscribe("wp_cmd", 1, write_callback); 

      ros::Publisher read_pub = nh.advertise<std_msgs::String>("write", 1000); 
      
      ros::Rate loop_rate(20);


      while(ros::ok()) 
      { 
            ros::spinOnce(); 
            loop_rate.sleep(); 
            if(cmd_flag == 1){
                  std_msgs::String command;
                  command.data = "launch_waypoint";
                  read_pub.publish(command);
                  ros::Duration(5).sleep();     //Delay 2 Sec.
                  command.data = cmd_to_publish;
                  read_pub.publish(command);
                  cmd_flag = 0;
            }
            else if(cmd_flag == 2){
                  std_msgs::String command;
                  command.data = cmd_to_publish;
                  read_pub.publish(command);
                  cmd_flag = 0; 
            }
      } 
} 
