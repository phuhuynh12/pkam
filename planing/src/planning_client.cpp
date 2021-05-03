#include "planning_client.h"
#include "std_msgs/Int8.h"

#include <ros/ros.h>
#include <mavros_msgs/State.h>
#include <mavros_msgs/GlobalPositionTarget.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/TransformStamped.h>
#include <sensor_msgs/NavSatFix.h>
#include <std_msgs/String.h>
#include <visualization_msgs/Marker.h>


PlanningClient::PlanningClient(int &argc, char **argv)
{
	this->nh_ = new ros::NodeHandle();

	avoidCollision_ = false;
}


void PlanningClient::init(DPlanning *const drone_planing){

	//For Mavros Subcriber Data.
	state_sub = nh_->subscribe<mavros_msgs::State>("/mavros/state", 10, &DPlanning::state_callback, drone_planing);
	local_pos_sub = nh_->subscribe<geometry_msgs::PoseStamped>("/mavros/local_position/pose", 10, &DPlanning::local_position_callback, drone_planing);
	global_pos_sub = nh_->subscribe<sensor_msgs::NavSatFix>("/mavros/global_position/global", 10, &DPlanning::global_position_callback, drone_planing);
	local_octomap_sub = nh_->subscribe<sensor_msgs::PointCloud2>("/octomap_point_cloud_centers", 10, &DPlanning::octomap_callback, drone_planing);

	//For Planning process
	/**
	* Trajectory get endpoint_position and create Movement Array
	* from current_position from mavros/local_position/pose and endpoint_position
	* with velocity and acceleration and start_time.
	* calculate current_target_position with dt (current_time - start_time).
	* (x = xo + vo*t + (a* t^2)/2).
	* quaternion (TODO).
	* with this propose we can change velocity and acceleration run time and add pid controller.
	*/
	getpoint_target_sub = nh_->subscribe<geometry_msgs::PoseStamped>("/planning/endpoint_position", 10, &DPlanning::get_target_position_callback, drone_planing);


	//Set point for drone Movement.
	setpoint_pos_pub = nh_->advertise<geometry_msgs::PoseStamped>("/planning/setpoint_position", 10);

	traj_marker_pub = nh_->advertise<visualization_msgs::Marker>("/planning_trajectory", 0);

}

//Not need spinOnce() in here, in planning_node already had.
void PlanningClient::publish_position_to_controller(const geometry_msgs::PoseStamped& setpoint_pos_ENU){
	setpoint_pos_pub.publish(setpoint_pos_ENU);
}