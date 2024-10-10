// Copyright 2023, FZI Forschungszentrum Informatik, Created on behalf of Universal Robots A/S
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the {copyright_holder} nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

//----------------------------------------------------------------------
/*!\file
 *
 * \author  Felix Exner exner@fzi.de
 * \date    2023-06-29
 */
//----------------------------------------------------------------------
#ifndef UR_CONTROLLERS__FREEDRIVE_MODE_CONTROLLER_HPP_
#define UR_CONTROLLERS__FREEDRIVE_MODE_CONTROLLER_HPP_

#pragma once
#include <memory>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>

#include <controller_interface/controller_interface.hpp>
#include <rclcpp/rclcpp.hpp>

namespace ur_controllers
{
enum CommandInterfaces
{
  FREEDRIVE_MODE_ASYNC_SUCCESS = 25,
  FREEDRIVE_MODE_CMD = 26,
};
enum StateInterfaces
{
  INITIALIZED_FLAG = 0u,
};

class FreedriveModeController : public controller_interface::ControllerInterface
{
public:
  controller_interface::InterfaceConfiguration command_interface_configuration() const override;

  controller_interface::InterfaceConfiguration state_interface_configuration() const override;

  // Change the input for the update function
  controller_interface::return_type update(const rclcpp::Time& time, const rclcpp::Duration& period) override;

  CallbackReturn on_configure(const rclcpp_lifecycle::State& previous_state) override;

  CallbackReturn on_activate(const rclcpp_lifecycle::State& previous_state) override;

  CallbackReturn on_deactivate(const rclcpp_lifecycle::State& previous_state) override;

  CallbackReturn on_init() override;

private:
  //void readFreedriveModeCmd(const std_msgs::msg::Bool::SharedPtr msg);
  //bool enableFreedriveMode();
  //bool disableFreedriveMode();

  // Everything related to the RT action server
  using FollowJTrajAction = control_msgs::action::FollowJointTrajectory;
  using RealtimeGoalHandle = realtime_tools::RealtimeServerGoalHandle<FollowJTrajAction>;
  using RealtimeGoalHandlePtr = std::shared_ptr<RealtimeGoalHandle>;
  using RealtimeGoalHandleBuffer = realtime_tools::RealtimeBuffer<RealtimeGoalHandlePtr>;

  RealtimeGoalHandleBuffer rt_active_goal_;         ///< Currently active action goal, if any.
  rclcpp::TimerBase::SharedPtr goal_handle_timer_;  ///< Timer to frequently check on the running goal
  realtime_tools::RealtimeBuffer<std::unordered_map<std::string, size_t>> joint_trajectory_mapping_;

  rclcpp::Duration action_monitor_period_ = rclcpp::Duration(50ms);

  // Not sure this is needed anymore, for tf_prefix there are other ways to handle
  // std::unique_ptr<tf2_ros::Buffer> tf_buffer_;
  // std::unique_ptr<tf2_ros::TransformListener> tf_listener_;

  std::shared_ptr<freedrive_mode_controller::ParamListener> param_listener_;
  freedrive_mode_controller::Params params_;

  /* Start an action server with an action called: /freedrive_mode_controller/start_freedrive_mode. */
  void start_action_server(void);

  void end_goal();

  std::atomic<bool> freedrive_mode_enable_;
  std::shared_ptr<rclcpp::Subscription<std_msgs::msg::Bool> freedrive_mode_sub_;

  static constexpr double ASYNC_WAITING = 2.0;
  /**
   * @brief wait until a command interface isn't in state ASYNC_WAITING anymore or until the parameter maximum_retries
   * have been reached
   */
  bool waitForAsyncCommand(std::function<double(void)> get_value);
};
} // namespace ur_controllers
#endif // UR_CONTROLLERS__PASSTHROUGH_TRAJECTORY_CONTROLLER_HPP_
