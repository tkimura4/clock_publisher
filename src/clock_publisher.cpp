// Copyright 2020 Tier IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <clock_publisher/clock_publisher.hpp>

#include <rclcpp/create_timer.hpp>

void ClockPublisher::onTimer()
{
  const rclcpp::Duration dif_time = (this->now() - base_time_) * speed_rate_;
  rclcpp::Time current_time;
  if (time_start_from_zero_) {
    current_time = rclcpp::Time(0) + dif_time;
  } else {
    current_time = base_time_ + dif_time;
  }
  rosgraph_msgs::msg::Clock clock;
  clock.set__clock(current_time);
  clock_pub_->publish(clock);
}

ClockPublisher::ClockPublisher(const rclcpp::NodeOptions & node_options)
: Node("clock_publisher", node_options)
{
  base_time_ = this->now();
  clock_pub_ = this->create_publisher<rosgraph_msgs::msg::Clock>("/clock", 1);
  const int rate = this->declare_parameter("rate", 200);
  speed_rate_ = this->declare_parameter<double>("speed_rate", 1.0);
  time_start_from_zero_ = this->declare_parameter<bool>("time_start_from_zero", false);

  // Timer
  auto timer_callback = std::bind(&ClockPublisher::onTimer, this);
  auto period = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<double>(1.0 / static_cast<double>(rate)));

  timer_ = std::make_shared<rclcpp::GenericTimer<decltype(timer_callback)>>(
    this->get_clock(), period, std::move(timer_callback),
    this->get_node_base_interface()->get_context());
  this->get_node_timers_interface()->add_timer(timer_, nullptr);
}

#include <rclcpp_components/register_node_macro.hpp>

RCLCPP_COMPONENTS_REGISTER_NODE(ClockPublisher)
