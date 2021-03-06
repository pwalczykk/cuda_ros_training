#include "ros_wrapper.hpp"

ROSWrapper::ROSWrapper(ros::NodeHandle *nh, image_transport::ImageTransport *it) : GPUPathPlanner(){
    this-> nh = nh;
    h_odom = (double*) malloc(3*sizeof(double));
    h_goal = (double*) malloc(3*sizeof(double));
    h_hmap_image = cv::Mat(256, 256, CV_8UC1);
    h_cost = (double*) malloc(1*sizeof(double));

    this->h_odom[0] = 0.0;
    this->h_odom[1] = 0.0;
    this->h_odom[2] = 0.0;
    this->h_goal[0] = 0.0;
    this->h_goal[1] = 0.0;
    this->h_goal[2] = 0.0;


    GPUPathPlanner::gpuSetup(256,256);
    ROS_INFO("MemoryAlocated");

    sub_odom = nh->subscribe("/cuda/odom", 100, &ROSWrapper::callbackOdom, this);
    sub_goal = nh->subscribe("/cuda/goal", 100, &ROSWrapper::callbackGoal, this);
    sub_hmap = it->subscribe("/cuda/hmap", 100, &ROSWrapper::callbackHmap, this);
    pub_cost = nh->advertise<std_msgs::Float64>("/cuda/cost", 100);


    cv::namedWindow("win1");
    cv::namedWindow("win2");
}


void ROSWrapper::callbackOdom(const nav_msgs::Odometry msg){
    this->h_odom[0] = msg.pose.pose.position.x;
    this->h_odom[1] = msg.pose.pose.position.y;
    this->h_odom[2] = msg.pose.pose.position.z;
    // ROS_WARN("Odom: %f %f %f", h_odom[0], h_odom[1], h_odom[2]);
}

void ROSWrapper::callbackGoal(const geometry_msgs::PoseStamped msg){
    this->h_goal[0] = msg.pose.position.x;
    this->h_goal[1] = msg.pose.position.y;
    this->h_goal[2] = msg.pose.position.z;
    // ROS_WARN("Goal: %f %f %f", h_goal[0], h_goal[1], h_goal[2]);

}

void ROSWrapper::callbackHmap(const sensor_msgs::ImageConstPtr& msg){
    this->h_hmap_image = cv_bridge::toCvShare(msg, "mono8")->image;

    // ROS_INFO("Odom: %f %f %f", h_odom[0], h_odom[1], h_odom[2]);
    // ROS_INFO("Goal: %f %f %f", h_goal[0], h_goal[1], h_goal[2]);

    cv::imshow("win2", this->h_hmap_image);
    cv::waitKey(10);


    ROS_INFO("Pixels: [0]: %d | [1]: %d | [2]: %d", h_hmap_image.data[0], h_hmap_image.data[1], h_hmap_image.data[2]);

    GPUPathPlanner::gpuCopyInputToDevice(h_odom, h_goal, h_hmap_image.data);
    GPUPathPlanner::gpuExecuteKernel();
    GPUPathPlanner::gpuCopyOutputToHost(h_cost, h_hmap_image.data);

    ROS_WARN("Pixels: [0]: %d | [1]: %d | [2]: %d", h_hmap_image.data[0], h_hmap_image.data[1], h_hmap_image.data[2]);

    cv::imshow("win1", this->h_hmap_image);
    cv::waitKey(10);
    ROS_INFO("cost: %f", *h_cost);
    msg_cost.data = *h_cost;
    pub_cost.publish(msg_cost);
}
