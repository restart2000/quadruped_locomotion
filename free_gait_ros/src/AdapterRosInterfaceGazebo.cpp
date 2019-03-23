/*
 *  AdapterRosInterfaceGazebo.cpp
 *  Descriotion: Adapter Ros interface with Gazebo states, update state via ros message from
 *               the state in the locomotion controller, which is a real time controller
 *
 *
 *  Created on: Dec 26, 2018
 *  Author: Shunyao Wang
 *  Institute: Harbin Institute of Technology, Shenzhen
 */

#include "free_gait_ros/AdapterRosInterfaceGazebo.hpp"

namespace free_gait {

AdapterRosInterfaceGazebo::AdapterRosInterfaceGazebo()
{
  leg_modes_.resize(4);
}

AdapterRosInterfaceGazebo::~AdapterRosInterfaceGazebo()
{

}

bool AdapterRosInterfaceGazebo::subscribeToRobotState(const std::string& robotStateTopic)
{
  joint_states_sub_ = nodeHandle_->subscribe(robotStateTopic, 1, &AdapterRosInterfaceGazebo::updateRobotState,this);
}
void AdapterRosInterfaceGazebo::unsubscribeFromRobotState()
{
  joint_states_sub_.shutdown();
//  throw std::runtime_error("AdapterRosInterfaceGazebo::unsubscribeFromRobotState() is not implemented.");

}
const std::string AdapterRosInterfaceGazebo::getRobotStateMessageType()
{
  return ros::message_traits::datatype<free_gait_msgs::RobotState>();
  throw std::runtime_error("AdapterRosInterfaceGazebo::getRobotStateMessageType() is not implemented.");

}
bool AdapterRosInterfaceGazebo::isReady() const
{
  throw std::runtime_error("AdapterRosInterfaceGazebo::isReady() is not implemented.");

}


//! Update adapter.
bool AdapterRosInterfaceGazebo::initializeAdapter(AdapterBase& adapter) const
{
  std::cout<<"Initial Adapter"<<std::endl;
//  throw std::runtime_error("AdapterRosInterfaceGazebo::initializeAdapter() is not implemented.");

}
bool AdapterRosInterfaceGazebo::updateAdapterWithRobotState(AdapterBase& adapter) const
{
/****************
* TODO(Shunyao) : update footcontacts info from gazebo,
* subscribe to sim_assiants/FootContacts msg, and update the support foot, surface normal
* in the State.
****************/
    // TODO(Shunyao): How to  update state?
    State state_last = adapter.getState();
    Pose pose_base_to_world(Position(base_pose_in_world_.pose.pose.position.x,
                                     base_pose_in_world_.pose.pose.position.y,
                                     base_pose_in_world_.pose.pose.position.z),
                            RotationQuaternion(base_pose_in_world_.pose.pose.orientation.w,
                                               base_pose_in_world_.pose.pose.orientation.x,
                                               base_pose_in_world_.pose.pose.orientation.y,
                                               base_pose_in_world_.pose.pose.orientation.z));
    LinearVelocity linear_velocity(LinearVelocity(base_pose_in_world_.twist.twist.linear.x,
                                                  base_pose_in_world_.twist.twist.linear.y,
                                                  base_pose_in_world_.twist.twist.linear.z));
    LocalAngularVelocity angular_velocity(LocalAngularVelocity(base_pose_in_world_.twist.twist.angular.x,
                                                               base_pose_in_world_.twist.twist.angular.y,
                                                               base_pose_in_world_.twist.twist.angular.z));

    state_last.setBaseStateFromFeedback(linear_velocity, angular_velocity);
    state_last.setPoseBaseToWorld(pose_base_to_world);
    state_last.setCurrentLimbJoints(all_joint_positions_);
    for(auto leg_mode : leg_modes_)
      {
        if(leg_mode.support_leg)
          {
            state_last.setSupportLeg(adapter.getLimbEnumFromLimbString(leg_mode.name),
                                     leg_mode.support_leg);
            state_last.setSurfaceNormal(adapter.getLimbEnumFromLimbString(leg_mode.name),
                                        Vector(leg_mode.surface_normal.vector.x,
                                               leg_mode.surface_normal.vector.y,
                                               leg_mode.surface_normal.vector.z));
//            state_last.setLimbConfigure()
          }
      }
//    std::cout<<"AdapterRosInterfaceGazebo update base position : "<<pose_base_to_world.getPosition()<<std::endl;
//    std::cout<<"AdapterRosInterfaceGazebo updatejoint position: "<<all_joint_positions_<<std::endl;
    adapter.setInternalDataFromState(state_last);

    return true;
    throw std::runtime_error("AdapterRosInterfaceGazebo::updateAdapterWithRobotState() is not implemented.");

}

void AdapterRosInterfaceGazebo::updateRobotState(const free_gait_msgs::RobotStateConstPtr& robotState)
{
//  std::cout<<"AdapterRosInterfaceGazebo::updateRobotState Once"<<std::endl;
  for(int i = 0;i<3;i++)
  {
    all_joint_positions_(i) = robotState->lf_leg_joints.position[i];
  }
  for(int i = 0;i<3;i++)
  {
    all_joint_positions_(i+3) = robotState->rf_leg_joints.position[i];
  }
  for(int i = 0;i<3;i++)
  {
    all_joint_positions_(i+6) = robotState->rh_leg_joints.position[i];
  }
  for(int i = 0;i<3;i++)
  {
    all_joint_positions_(i+9) = robotState->lh_leg_joints.position[i];
  }

  base_pose_in_world_.header = robotState->base_pose.header;
  base_pose_in_world_.child_frame_id = robotState->base_pose.child_frame_id;
  base_pose_in_world_.pose = robotState->base_pose.pose;
  base_pose_in_world_.twist = robotState->base_pose.twist;
//  std::cout<<"AdapterRosInterfaceGazebo update base position : "<<base_pose_in_world_.pose.pose.position.x<<std::endl;
  leg_modes_[0] = robotState->lf_leg_mode;
  leg_modes_[1] = robotState->rf_leg_mode;
  leg_modes_[2] = robotState->rh_leg_mode;
  leg_modes_[3] = robotState->lh_leg_mode;

}

}//namespace


#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(free_gait::AdapterRosInterfaceGazebo, free_gait::AdapterRosInterfaceBase)