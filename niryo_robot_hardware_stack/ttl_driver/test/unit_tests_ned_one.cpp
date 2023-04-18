/*
    ttl_driver_unit_tests.cpp
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

// Bring in my package's API, which is what I'm testing
#include "common/model/abstract_motor_state.hpp"
#include "common/model/bus_protocol_enum.hpp"
#include "common/model/dxl_command_type_enum.hpp"
#include "common/model/dxl_motor_state.hpp"
#include "common/model/hardware_type_enum.hpp"
#include "common/model/joint_state.hpp"
#include "common/model/single_motor_cmd.hpp"
#include "common/model/stepper_command_type_enum.hpp"
#include "common/model/stepper_motor_state.hpp"
#include "common/model/synchronize_motor_cmd.hpp"
#include "dynamixel_sdk/dynamixel_sdk.h"
#include "ros/node_handle.h"
#include "ttl_driver/ttl_interface_core.hpp"
#include "ttl_driver/ttl_manager.hpp"

// Bring in gtest
#include <cassert>
#include <gtest/gtest.h>
#include <memory>
#include <ros/console.h>
#include <string>
#include <utility>

using ::common::model::BusProtocolEnum;
using ::common::model::DxlMotorState;
using ::common::model::EBusProtocol;
using ::common::model::EHardwareType;
using ::common::model::HardwareTypeEnum;
using ::common::model::StepperMotorState;
using ::std::string;
using ::std::to_string;

/**
 * @brief addJointToTtlInterface
 * @param ttl_interface
 */
void addJointToTtlInterface(const std::shared_ptr<ttl_driver::TtlInterfaceCore> &ttl_interface)
{
    size_t nb_joints = 0;

    ros::NodeHandle robot_hwnh("joints_interface");
    // retrieve nb joints with checking that the config param exists for both name and id
    while (robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/id") && robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/name") &&
           robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/type") && robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/bus"))
        nb_joints++;

    // connect and register joint state interface

    int currentIdDxl = 1;
    int currentIdStepper = 1;

    for (size_t j = 0; j < nb_joints; j++)
    {
        int joint_id_config = 0;
        string joint_name;
        string joint_type;
        string joint_bus;

        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/id", joint_id_config);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/name", joint_name);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/type", joint_type);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/bus", joint_bus);
        HardwareTypeEnum eType = HardwareTypeEnum(joint_type.c_str());
        BusProtocolEnum eBusProto = BusProtocolEnum(joint_bus.c_str());

        if (eType == EHardwareType::STEPPER || eType == EHardwareType::FAKE_STEPPER_MOTOR)
        {  // stepper
            std::string currentStepperNamespace = "steppers/stepper_" + to_string(currentIdStepper);

            auto stepperState = std::make_shared<StepperMotorState>(joint_name, eType, common::model::EComponentType::JOINT, eBusProto, static_cast<uint8_t>(joint_id_config));
            if (stepperState)
            {
                double offsetPos = 0.0;
                double gear_ratio = 1.0;
                int direction = 1;
                double max_effort = 0.0;
                double home_position = 0.0;
                double limit_position_min = 0.0;
                double limit_position_max = 0.0;
                double motor_ratio = 0.0;

                robot_hwnh.getParam(currentStepperNamespace + "/offset_position", offsetPos);
                robot_hwnh.getParam(currentStepperNamespace + "/gear_ratio", gear_ratio);
                robot_hwnh.getParam(currentStepperNamespace + "/direction", direction);
                robot_hwnh.getParam(currentStepperNamespace + "/max_effort", max_effort);
                robot_hwnh.getParam(currentStepperNamespace + "/home_position", home_position);
                robot_hwnh.getParam(currentStepperNamespace + "/limit_position_min", limit_position_min);
                robot_hwnh.getParam(currentStepperNamespace + "/limit_position_max", limit_position_max);
                robot_hwnh.getParam(currentStepperNamespace + "/motor_ratio", motor_ratio);

                // acceleration and velocity profiles
                common::model::VelocityProfile profile{};
                int data{};
                if (robot_hwnh.hasParam(currentStepperNamespace + "/v_start"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/v_start", data);
                    profile.v_start = static_cast<uint32_t>(data);
                }

                if (robot_hwnh.hasParam(currentStepperNamespace + "/a_1"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/a_1", data);
                    profile.a_1 = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/v_1"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/v_1", data);
                    profile.v_1 = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/a_max"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/a_max", data);
                    profile.a_max = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/v_max"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/v_max", data);
                    profile.v_max = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/d_max"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/d_max", data);
                    profile.d_max = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/d_1"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/d_1", data);
                    profile.d_1 = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/v_stop"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/v_stop", data);
                    profile.v_stop = static_cast<uint32_t>(data);
                }

                // add parameters
                stepperState->setOffsetPosition(offsetPos);
                stepperState->setGearRatio(gear_ratio);
                stepperState->setDirection(static_cast<int8_t>(direction));
                stepperState->setMaxEffort(max_effort);
                stepperState->setHomePosition(home_position);
                stepperState->setLimitPositionMax(limit_position_max);
                stepperState->setLimitPositionMin(limit_position_min);
                stepperState->setMotorRatio(motor_ratio);
                stepperState->setVelocityProfile(profile);

                if (eBusProto == EBusProtocol::TTL)
                    ttl_interface->addJoint(stepperState);

                currentIdStepper++;
            }
        }
        else if (eType != EHardwareType::UNKNOWN)
        {  // dynamixel
            auto dxlState = std::make_shared<DxlMotorState>(joint_name, eType, common::model::EComponentType::JOINT, static_cast<uint8_t>(joint_id_config));
            if (dxlState)
            {
                double offsetPos = 0.0;
                int direction = 1;
                int positionPGain = 0;
                int positionIGain = 0;
                int positionDGain = 0;
                int velocityPGain = 0;
                int velocityIGain = 0;
                int FF1Gain = 0;
                int FF2Gain = 0;
                int velocityProfile = 0;
                int accelerationProfile = 0;
                double limit_position_min = 0.0;
                double limit_position_max = 0.0;
                double home_position = 0.0;

                std::string currentDxlNamespace = "dynamixels/dxl_" + to_string(currentIdDxl);

                robot_hwnh.getParam(currentDxlNamespace + "/offset_position", offsetPos);
                robot_hwnh.getParam(currentDxlNamespace + "/direction", direction);

                robot_hwnh.getParam(currentDxlNamespace + "/position_P_gain", positionPGain);
                robot_hwnh.getParam(currentDxlNamespace + "/position_I_gain", positionIGain);
                robot_hwnh.getParam(currentDxlNamespace + "/position_D_gain", positionDGain);

                robot_hwnh.getParam(currentDxlNamespace + "/velocity_P_gain", velocityPGain);
                robot_hwnh.getParam(currentDxlNamespace + "/velocity_I_gain", velocityIGain);

                robot_hwnh.getParam(currentDxlNamespace + "/FF1_gain", FF1Gain);
                robot_hwnh.getParam(currentDxlNamespace + "/FF2_gain", FF2Gain);

                robot_hwnh.getParam(currentDxlNamespace + "/velocity_profile", velocityProfile);
                robot_hwnh.getParam(currentDxlNamespace + "/acceleration_profile", accelerationProfile);

                robot_hwnh.getParam(currentDxlNamespace + "/home_position", home_position);
                robot_hwnh.getParam(currentDxlNamespace + "/limit_position_min", limit_position_min);
                robot_hwnh.getParam(currentDxlNamespace + "/limit_position_max", limit_position_max);

                dxlState->setOffsetPosition(offsetPos);
                dxlState->setDirection(static_cast<int8_t>(direction));

                dxlState->setPositionPGain(static_cast<uint32_t>(positionPGain));
                dxlState->setPositionIGain(static_cast<uint32_t>(positionIGain));
                dxlState->setPositionDGain(static_cast<uint32_t>(positionDGain));

                dxlState->setVelocityPGain(static_cast<uint32_t>(velocityPGain));
                dxlState->setVelocityIGain(static_cast<uint32_t>(velocityIGain));

                dxlState->setFF1Gain(static_cast<uint32_t>(FF1Gain));
                dxlState->setFF2Gain(static_cast<uint32_t>(FF2Gain));

                dxlState->setVelProfile(static_cast<uint32_t>(velocityProfile));
                dxlState->setAccProfile(static_cast<uint32_t>(accelerationProfile));

                dxlState->setLimitPositionMin(limit_position_min);
                dxlState->setLimitPositionMax(limit_position_max);

                dxlState->setHomePosition(home_position);

                if (eBusProto == EBusProtocol::TTL)
                    ttl_interface->addJoint(dxlState);

                currentIdDxl++;
            }
        }
    }  // end for (size_t j = 0; j < nb_joints; j++)
}

/**
 * @brief addJointToTtlManager
 * @param ttl_drv
 */
void addJointToTtlManager(const std::shared_ptr<ttl_driver::TtlManager> &ttl_drv)
{
    size_t nb_joints = 0;

    ros::NodeHandle robot_hwnh("joints_interface");
    // retrieve nb joints with checking that the config param exists for both name and id
    while (robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/id") && robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/name") &&
           robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/type") && robot_hwnh.hasParam("joint_" + to_string(nb_joints + 1) + "/bus"))
        nb_joints++;

    // connect and register joint state interface
    int currentIdStepper = 1;
    int currentIdDxl = 1;

    for (size_t j = 0; j < nb_joints; j++)
    {
        int joint_id_config = 0;
        string joint_name;
        string joint_type;
        string joint_bus;

        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/id", joint_id_config);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/name", joint_name);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/type", joint_type);
        robot_hwnh.getParam("joint_" + to_string(j + 1) + "/bus", joint_bus);

        HardwareTypeEnum eType = HardwareTypeEnum(joint_type.c_str());
        BusProtocolEnum eBusProto = BusProtocolEnum(joint_bus.c_str());

        if (eType == EHardwareType::STEPPER || eType == EHardwareType::FAKE_STEPPER_MOTOR)
        {  // stepper
            std::string currentStepperNamespace = "steppers/stepper_" + to_string(currentIdStepper);

            auto stepperState = std::make_shared<StepperMotorState>(joint_name, eType, common::model::EComponentType::JOINT, eBusProto, static_cast<uint8_t>(joint_id_config));
            if (stepperState)
            {
                double offsetPos = 0.0;
                double gear_ratio = 1.0;
                int direction = 1;
                double max_effort = 0.0;
                double home_position = 0.0;
                double limit_position_min = 0.0;
                double limit_position_max = 0.0;
                double motor_ratio = 0.0;

                robot_hwnh.getParam(currentStepperNamespace + "/offset_position", offsetPos);
                robot_hwnh.getParam(currentStepperNamespace + "/gear_ratio", gear_ratio);
                robot_hwnh.getParam(currentStepperNamespace + "/direction", direction);
                robot_hwnh.getParam(currentStepperNamespace + "/max_effort", max_effort);
                robot_hwnh.getParam(currentStepperNamespace + "/home_position", home_position);
                robot_hwnh.getParam(currentStepperNamespace + "/limit_position_min", limit_position_min);
                robot_hwnh.getParam(currentStepperNamespace + "/limit_position_max", limit_position_max);
                robot_hwnh.getParam(currentStepperNamespace + "/motor_ratio", motor_ratio);

                // acceleration and velocity profiles
                common::model::VelocityProfile profile{};
                int data{};
                if (robot_hwnh.hasParam(currentStepperNamespace + "/v_start"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/v_start", data);
                    profile.v_start = static_cast<uint32_t>(data);
                }

                if (robot_hwnh.hasParam(currentStepperNamespace + "/a_1"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/a_1", data);
                    profile.a_1 = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/v_1"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/v_1", data);
                    profile.v_1 = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/a_max"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/a_max", data);
                    profile.a_max = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/v_max"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/v_max", data);
                    profile.v_max = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/d_max"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/d_max", data);
                    profile.d_max = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/d_1"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/d_1", data);
                    profile.d_1 = static_cast<uint32_t>(data);
                }
                if (robot_hwnh.hasParam(currentStepperNamespace + "/v_stop"))
                {
                    robot_hwnh.getParam(currentStepperNamespace + "/v_stop", data);
                    profile.v_stop = static_cast<uint32_t>(data);
                }

                // add parameters
                stepperState->setOffsetPosition(offsetPos);
                stepperState->setGearRatio(gear_ratio);
                stepperState->setDirection(static_cast<int8_t>(direction));
                stepperState->setMaxEffort(max_effort);
                stepperState->setHomePosition(home_position);
                stepperState->setLimitPositionMax(limit_position_max);
                stepperState->setLimitPositionMin(limit_position_min);
                stepperState->setMotorRatio(motor_ratio);
                stepperState->setVelocityProfile(profile);

                if (eBusProto == EBusProtocol::TTL)
                {
                    ttl_drv->addHardwareComponent(stepperState);
                }
                currentIdStepper++;
            }
        }
        else if (eType != EHardwareType::UNKNOWN)
        {  // dynamixel
            auto dxlState = std::make_shared<DxlMotorState>(joint_name, eType, common::model::EComponentType::JOINT, static_cast<uint8_t>(joint_id_config));
            if (dxlState)
            {
                double offsetPos = 0.0;
                int direction = 1;
                int positionPGain = 0;
                int positionIGain = 0;
                int positionDGain = 0;
                int velocityPGain = 0;
                int velocityIGain = 0;
                int FF1Gain = 0;
                int FF2Gain = 0;
                int velocityProfile = 0;
                int accelerationProfile = 0;
                double limit_position_min = 0.0;
                double limit_position_max = 0.0;
                double home_position = 0.0;

                std::string currentDxlNamespace = "dynamixels/dxl_" + to_string(currentIdDxl);

                robot_hwnh.getParam(currentDxlNamespace + "/offset_position", offsetPos);
                robot_hwnh.getParam(currentDxlNamespace + "/direction", direction);

                robot_hwnh.getParam(currentDxlNamespace + "/position_P_gain", positionPGain);
                robot_hwnh.getParam(currentDxlNamespace + "/position_I_gain", positionIGain);
                robot_hwnh.getParam(currentDxlNamespace + "/position_D_gain", positionDGain);

                robot_hwnh.getParam(currentDxlNamespace + "/velocity_P_gain", velocityPGain);
                robot_hwnh.getParam(currentDxlNamespace + "/velocity_I_gain", velocityIGain);

                robot_hwnh.getParam(currentDxlNamespace + "/FF1_gain", FF1Gain);
                robot_hwnh.getParam(currentDxlNamespace + "/FF2_gain", FF2Gain);

                robot_hwnh.getParam(currentDxlNamespace + "/velocity_profile", velocityProfile);
                robot_hwnh.getParam(currentDxlNamespace + "/acceleration_profile", accelerationProfile);

                robot_hwnh.getParam(currentDxlNamespace + "/home_position", home_position);
                robot_hwnh.getParam(currentDxlNamespace + "/limit_position_min", limit_position_min);
                robot_hwnh.getParam(currentDxlNamespace + "/limit_position_max", limit_position_max);

                dxlState->setOffsetPosition(offsetPos);
                dxlState->setDirection(static_cast<int8_t>(direction));

                dxlState->setPositionPGain(static_cast<uint32_t>(positionPGain));
                dxlState->setPositionIGain(static_cast<uint32_t>(positionIGain));
                dxlState->setPositionDGain(static_cast<uint32_t>(positionDGain));

                dxlState->setVelocityPGain(static_cast<uint32_t>(velocityPGain));
                dxlState->setVelocityIGain(static_cast<uint32_t>(velocityIGain));

                dxlState->setFF1Gain(static_cast<uint32_t>(FF1Gain));
                dxlState->setFF2Gain(static_cast<uint32_t>(FF2Gain));

                dxlState->setVelProfile(static_cast<uint32_t>(velocityProfile));
                dxlState->setAccProfile(static_cast<uint32_t>(accelerationProfile));

                dxlState->setLimitPositionMin(limit_position_min);
                dxlState->setLimitPositionMax(limit_position_max);

                dxlState->setHomePosition(home_position);

                if (eBusProto == EBusProtocol::TTL)
                {
                    ttl_drv->addHardwareComponent(dxlState);
                }
                currentIdDxl++;
            }
        }
    }  // end for (size_t j = 0; j < nb_joints; j++)
}

// Declare a test
/******************************************************/
/************ Tests of ttl interface ******************/
/******************************************************/
/**
 * @brief The TtlInterfaceTestSuite class
 */
class TtlInterfaceTestSuite : public ::testing::Test
{
  protected:
    static void SetUpTestCase()
    {
        ros::NodeHandle nh("ttl_driver");
        ros::Duration(5.0).sleep();

        ttl_interface = std::make_shared<ttl_driver::TtlInterfaceCore>(nh);

        addJointToTtlInterface(ttl_interface);
        // check connections
        EXPECT_TRUE(ttl_interface->isConnectionOk());
        EXPECT_TRUE(ttl_interface->scanMotorId(2));
        EXPECT_TRUE(ttl_interface->scanMotorId(3));
        EXPECT_TRUE(ttl_interface->scanMotorId(6));
    }

    static void TearDownTestCase() { ros::shutdown(); }

    static std::shared_ptr<ttl_driver::TtlInterfaceCore> ttl_interface;
};

std::shared_ptr<ttl_driver::TtlInterfaceCore> TtlInterfaceTestSuite::ttl_interface;

// Test reboot motors
TEST_F(TtlInterfaceTestSuite, testRebootMotors)
{
    int resutl = ttl_interface->rebootHardware(ttl_interface->getJointState(2));
    EXPECT_EQ(resutl, static_cast<int>(niryo_robot_msgs::CommandStatus::SUCCESS));
}

// Test reboot motor with wrong id
TEST_F(TtlInterfaceTestSuite, testRebootMotorsWrongID)
{
    bool result;
    result = ttl_interface->rebootHardware(std::make_shared<common::model::DxlMotorState>());
    EXPECT_FALSE(result);
}

/**
 * @brief The TtlManagerTestSuite class
 */
class TtlManagerTestSuite : public ::testing::Test
{
  protected:
    static void SetUpTestCase()
    {
        ros::NodeHandle nh("ttl_driver");

        ttl_drv = std::make_shared<ttl_driver::TtlManager>(nh);

        addJointToTtlManager(ttl_drv);
        // check connections
        EXPECT_TRUE(ttl_drv->ping(2));
        EXPECT_TRUE(ttl_drv->ping(3));
        EXPECT_TRUE(ttl_drv->ping(6));
    }

    static std::shared_ptr<ttl_driver::TtlManager> ttl_drv;
};

std::shared_ptr<ttl_driver::TtlManager> TtlManagerTestSuite::ttl_drv;

/******************************************************/
/************** Tests of ttl manager ******************/
/******************************************************/

// Test driver received cmd

TEST_F(TtlManagerTestSuite, testSingleCmds)
{
    auto cmd_1 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE, 2, std::initializer_list<uint32_t>{1});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_1)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    // wrong id
    auto cmd_2 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE, 20, std::initializer_list<uint32_t>{1});
    EXPECT_NE(ttl_drv->writeSingleCommand(std::move(cmd_2)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    // wrong type cmd
    auto cmd_3 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_UNKNOWN, 2, std::initializer_list<uint32_t>{1});
    EXPECT_NE(ttl_drv->writeSingleCommand(std::move(cmd_3)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    // wrong type of cmd object
    auto cmd_4 = std::make_unique<common::model::StepperTtlSingleCmd>(common::model::EStepperCommandType::CMD_TYPE_TORQUE, 5, std::initializer_list<uint32_t>{1});
    EXPECT_NE(ttl_drv->writeSingleCommand(std::move(cmd_4)), COMM_SUCCESS);
}

//  Test control cmds
TEST_F(TtlManagerTestSuite, testSingleControlCmds)
{
    auto cmd_1_torque = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE, 2, std::initializer_list<uint32_t>{1});

    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_1_torque)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    auto cmd_2_torque = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE, 3, std::initializer_list<uint32_t>{1});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_2_torque)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    auto cmd_3_torque = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE, 6, std::initializer_list<uint32_t>{1});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_3_torque)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    ttl_drv->readJointsStatus();
    auto state_motor_2 = std::dynamic_pointer_cast<common::model::JointState>(ttl_drv->getHardwareState(2));
    assert(state_motor_2);
    auto state_motor_3 = std::dynamic_pointer_cast<common::model::JointState>(ttl_drv->getHardwareState(3));
    assert(state_motor_3);
    auto state_motor_6 = std::dynamic_pointer_cast<common::model::JointState>(ttl_drv->getHardwareState(6));
    assert(state_motor_6);

    auto new_pos_2 = static_cast<uint32_t>(state_motor_2->to_motor_pos(state_motor_2->getHomePosition()));
    auto new_pos_3 = static_cast<uint32_t>(state_motor_3->to_motor_pos(state_motor_3->getHomePosition()));
    auto new_pos_6 = static_cast<uint32_t>(state_motor_6->to_motor_pos(state_motor_6->getHomePosition()));

    // single control cmd for stepper ttl id 2
    auto cmd_1 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_POSITION, 2, std::initializer_list<uint32_t>{new_pos_2});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_1)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    // single control cmd for stepper ttl id 3
    auto cmd_2 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_POSITION, 3, std::initializer_list<uint32_t>{new_pos_3});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_2)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    // single control cmd for dxl ttl id 6
    auto cmd_3 = std::make_unique<common::model::DxlSingleCmd>(common::model::EDxlCommandType::CMD_TYPE_POSITION, 6, std::initializer_list<uint32_t>{new_pos_6});
    EXPECT_EQ(ttl_drv->writeSingleCommand(std::move(cmd_3)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    ttl_drv->readJointsStatus();

    //  can miss 1 2 steps
    EXPECT_NEAR(state_motor_2->getPosition(), new_pos_2, 2);
    EXPECT_NEAR(state_motor_3->getPosition(), new_pos_3, 2);
    EXPECT_NEAR(state_motor_6->getPosition(), new_pos_6, 2);
}

TEST_F(TtlManagerTestSuite, testSyncCmds)
{
    common::model::EHardwareType dxl_type;

    dxl_type = common::model::EHardwareType::FAKE_DXL_MOTOR;

    // sync cmd
    auto dynamixel_cmd_1 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    dynamixel_cmd_1->addMotorParam(dxl_type, 2, 1);
    dynamixel_cmd_1->addMotorParam(dxl_type, 6, 1);

    EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_1)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    // redondant id
    auto dynamixel_cmd_3 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    dynamixel_cmd_3->addMotorParam(dxl_type, 2, 1);
    dynamixel_cmd_3->addMotorParam(dxl_type, 2, 1);

    EXPECT_NE(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_3)), COMM_SUCCESS);

    // wrong cmd type
    auto dynamixel_cmd_4 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_UNKNOWN);
    dynamixel_cmd_4->addMotorParam(dxl_type, 2, 1);
    dynamixel_cmd_4->addMotorParam(dxl_type, 6, 1);

    EXPECT_NE(ttl_drv->writeSynchronizeCommand(std::move(dynamixel_cmd_4)), COMM_SUCCESS);
}

TEST_F(TtlManagerTestSuite, testSyncControlCmds)
{
    auto state_motor_2 = std::dynamic_pointer_cast<common::model::JointState>(ttl_drv->getHardwareState(2));
    assert(state_motor_2);
    auto state_motor_3 = std::dynamic_pointer_cast<common::model::JointState>(ttl_drv->getHardwareState(3));
    assert(state_motor_3);
    auto state_motor_6 = std::dynamic_pointer_cast<common::model::JointState>(ttl_drv->getHardwareState(6));
    assert(state_motor_6);

    // sync cmd
    auto cmd_1_torque = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    cmd_1_torque->addMotorParam(state_motor_2->getHardwareType(), 2, 1);
    cmd_1_torque->addMotorParam(state_motor_3->getHardwareType(), 3, 1);
    cmd_1_torque->addMotorParam(state_motor_6->getHardwareType(), 6, 1);

    EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(cmd_1_torque)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    ttl_drv->readJointsStatus();

    auto new_pos_2 = static_cast<uint32_t>(state_motor_2->to_motor_pos(0.47));
    auto new_pos_3 = static_cast<uint32_t>(state_motor_3->to_motor_pos(0.577));
    auto new_pos_6 = static_cast<uint32_t>(state_motor_6->to_motor_pos(1.08));

    auto cmd_1 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_POSITION);
    cmd_1->addMotorParam(state_motor_2->getHardwareType(), 2, new_pos_2);
    cmd_1->addMotorParam(state_motor_3->getHardwareType(), 3, new_pos_3);
    cmd_1->addMotorParam(state_motor_6->getHardwareType(), 6, new_pos_6);

    EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(cmd_1)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    ttl_drv->readJointsStatus();

    EXPECT_EQ(static_cast<uint32_t>(state_motor_2->getPosition()), new_pos_2);
    EXPECT_EQ(static_cast<uint32_t>(state_motor_3->getPosition()), new_pos_3);
    EXPECT_EQ(static_cast<uint32_t>(state_motor_6->getPosition()), new_pos_6);
}

TEST_F(TtlManagerTestSuite, testSyncControlCmdsReturnHome)
{
    auto state_motor_2 = std::dynamic_pointer_cast<common::model::JointState>(ttl_drv->getHardwareState(2));
    assert(state_motor_2);
    auto state_motor_3 = std::dynamic_pointer_cast<common::model::JointState>(ttl_drv->getHardwareState(3));
    assert(state_motor_3);
    auto state_motor_6 = std::dynamic_pointer_cast<common::model::JointState>(ttl_drv->getHardwareState(6));
    assert(state_motor_6);

    // sync cmd
    auto cmd_1_torque = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_TORQUE);
    cmd_1_torque->addMotorParam(state_motor_2->getHardwareType(), 2, 1);
    cmd_1_torque->addMotorParam(state_motor_3->getHardwareType(), 3, 1);
    cmd_1_torque->addMotorParam(state_motor_6->getHardwareType(), 6, 1);

    EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(cmd_1_torque)), COMM_SUCCESS);
    ros::Duration(0.01).sleep();

    auto new_pos_2 = static_cast<uint32_t>(state_motor_2->to_motor_pos(state_motor_2->getHomePosition()));
    auto new_pos_3 = static_cast<uint32_t>(state_motor_3->to_motor_pos(state_motor_3->getHomePosition()));
    auto new_pos_6 = static_cast<uint32_t>(state_motor_6->to_motor_pos(state_motor_6->getHomePosition()));

    auto cmd_1 = std::make_unique<common::model::DxlSyncCmd>(common::model::EDxlCommandType::CMD_TYPE_POSITION);
    cmd_1->addMotorParam(state_motor_2->getHardwareType(), 2, new_pos_2);
    cmd_1->addMotorParam(state_motor_3->getHardwareType(), 3, new_pos_3);
    cmd_1->addMotorParam(state_motor_6->getHardwareType(), 6, new_pos_6);

    EXPECT_EQ(ttl_drv->writeSynchronizeCommand(std::move(cmd_1)), COMM_SUCCESS);
    ros::Duration(0.5).sleep();

    ttl_drv->readJointsStatus();

    EXPECT_EQ(static_cast<uint32_t>(state_motor_6->getPosition()), new_pos_6);
    EXPECT_EQ(static_cast<uint32_t>(state_motor_2->getPosition()), new_pos_2);
    EXPECT_EQ(static_cast<uint32_t>(state_motor_3->getPosition()), new_pos_3);
}

// Test driver scan motors
TEST_F(TtlManagerTestSuite, scanTest) { EXPECT_EQ(ttl_drv->scanAndCheck(), COMM_SUCCESS); }

// Run all the tests that were declared with TEST()
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    ros::init(argc, argv, "ttl_driver_unit_tests");

    std::string hardware_version;

    return RUN_ALL_TESTS();
}
