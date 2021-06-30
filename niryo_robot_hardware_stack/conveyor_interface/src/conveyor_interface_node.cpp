/*
    conveyor_interface_node.cpp
    Copyright (C) 2020 Niryo
    All rights reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http:// www.gnu.org/licenses/>.
*/

// ros
#include <ros/ros.h>

// niryo
#include "conveyor_interface/conveyor_interface_core.hpp"
#include "can_driver/can_driver_core.hpp"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "conveyor_interface_node");

    ROS_DEBUG("Launching conveyor_interface_node");

    ros::AsyncSpinner spinner(4);
    spinner.start();

    ros::NodeHandle nh;

        auto can_driver = std::make_shared<can_driver::CanDriverCore>();
        ros::Duration(0.25).sleep();

        auto conveyor_interface = std::make_shared<conveyor_interface::ConveyorInterfaceCore>(can_driver);
        ros::Duration(0.25).sleep();

    ros::spin();

    return 0;
}
