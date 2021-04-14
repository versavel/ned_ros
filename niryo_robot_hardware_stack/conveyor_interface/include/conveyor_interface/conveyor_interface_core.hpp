/*
    conveyor_interface_interface_core.hpp
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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONVEYOR_INTERFACE_CORE_HPP
#define CONVEYOR_INTERFACE_CORE_HPP

#include <memory>

#include <ros/ros.h>
#include <vector>

#include "conveyor_interface/SetConveyor.h"
#include "conveyor_interface/ControlConveyor.h"
#include "conveyor_interface/ConveyorFeedbackArray.h"
#include "stepper_driver/stepper_driver_core.hpp"
#include "niryo_robot_msgs/CommandStatus.h"

namespace ConveyorInterface {
    class ConveyorInterfaceCore
    {
        public:

            ConveyorInterfaceCore(std::shared_ptr<StepperDriver::StepperDriverCore> stepper);
            virtual ~ConveyorInterfaceCore();

            void initServices();
            void initParams();

        private:

            ros::NodeHandle _nh;
            std::shared_ptr<StepperDriver::StepperDriverCore> _stepper;

            ros::ServiceServer _ping_and_set_stepper_server;
            ros::ServiceServer _control_conveyor_server;
            ros::Publisher _conveyors_feedback_publisher;

            ros::Publisher _conveyor_status_publisher;
            std::thread _publish_conveyors_feedback_thread;

            std::vector<uint8_t> _list_conveyor_id;
            std::vector<uint8_t> _list_available_id;
            std::vector<int> _list_possible_conveyor_id;
            int _conveyor_id;
            int _conveyor_max_effort;
            double _publish_feedback_frequency;


            bool _callbackPingAndSetConveyor(conveyor_interface::SetConveyor::Request &req, conveyor_interface::SetConveyor::Response &res);
            bool _callbackControlConveyor(conveyor_interface::ControlConveyor::Request &req, conveyor_interface::ControlConveyor::Response &res);

            void _publishConveyorsFeedback();
    };
} // ConveyorInterface
#endif
