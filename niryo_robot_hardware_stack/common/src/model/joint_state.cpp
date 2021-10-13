/*
    joint_state.hpp
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

#include "common/model/joint_state.hpp"

// C++
#include <sstream>
#include <string>
#include <utility>

namespace common
{
namespace model
{

/**
 * @brief JointState::JointState
 * @param name
 * @param type
 * @param component_type
 * @param bus_proto
 * @param id
 */
JointState::JointState(std::string name, EHardwareType type,
                       EComponentType component_type,
                       EBusProtocol bus_proto, uint8_t id) :
    AbstractMotorState(type, component_type, bus_proto, id),
    _name(std::move(name))
{
}

/**
 * @brief JointState::operator ==
 * @param other
 * @return
 */
bool JointState::operator==(const JointState& other) const
{
    return((this->_hw_type == other._hw_type) && (this->_id == other._id));
}

/**
 * @brief JointState::setName
 * @param name
 */
void JointState::setName(std::string& name)
{
    _name = name;
}

/**
 * @brief JointState::setOffsetPosition
 * @param offset_position
 */
void JointState::setOffsetPosition(double offset_position)
{
    _offset_position = offset_position;
}

/**
 * @brief JointState::setDirection
 * @param direction
 */
void JointState::setDirection(int direction)
{
    _direction = direction;
}

// ***********************
//  AbstractMotor intf
// ***********************

/**
 * @brief JointState::reset
 */
void JointState::reset()
{
    AbstractMotorState::reset();
    _name.clear();
    _need_calibration = false;
}

/**
 * @brief common::model::JointState::isValid
 * @return
 */
bool common::model::JointState::isValid() const
{
    return (0 != getId() && EHardwareType::UNKNOWN != getHardwareType());
}

/**
 * @brief JointState::str
 * @return
 */
std::string JointState::str() const
{
    std::ostringstream ss;

    ss << "JointState : ";
    ss << "name: " << "\"" << _name << "\"" << ",\n";
    ss << "offset position: " << _offset_position << ", ";
    ss << "need calibration: " << (_need_calibration ? "true" : "false") << ", ";
    ss << "direction : " << _direction << ",\n";
    ss << "pos(" << pos << "), ";
    ss << "cmd(" << cmd << "), ";
    ss << "vel(" << vel << "), ";
    ss << "eff(" << eff << ")";
    ss << "\n---\n";
    ss << "\n";
    ss << AbstractMotorState::str();

    return ss.str();
}

}  // namespace model
}  // namespace common

