#ifndef ROS_BUFFOR_HPP_
#define ROS_BUFFOR_HPP_

#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseStamped.h>
#include <sensor_msgs/LaserScan.h>

class _ROSBuffor
{
public:
    geometry_msgs::PoseStamped goal;

    nav_msgs::Odometry odom;

    std_msgs::Float64 lidar_pose;

    sensor_msgs::LaserScan laser_scan;

public:
    _ROSBuffor(){

    }

    void debugInfo()
    {
        ROS_INFO("GOAL: {%f %f %f}", goal.pose.position.x, goal.pose.position.y, goal.pose.position.z);

        ROS_INFO("ODOM: {%f %f %f | %f %f %f %f}", odom.pose.pose.position.x, odom.pose.pose.position.y, odom.pose.pose.position.z,
            odom.pose.pose.orientation.x, odom.pose.pose.orientation.y, odom.pose.pose.orientation.z, odom.pose.pose.orientation.w);

        ROS_INFO("LIDAR POSE: {%f}", lidar_pose.data);

        ROS_INFO("LASER SCAN:");
        printf("{");
        for( int i = 0; i < laser_scan.ranges.size(); i++)
        {
            printf(" %f", laser_scan.ranges[i]);
        }
        printf(" }\n");
    }
};


#endif
