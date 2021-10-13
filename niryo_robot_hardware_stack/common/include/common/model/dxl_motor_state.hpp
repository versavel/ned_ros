/*
dxl_motor_state.h
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

#ifndef DXL_MOTOR_STATE_H
#define DXL_MOTOR_STATE_H

#include <string>
#include "joint_state.hpp"

namespace common
{
namespace model
{

/**
 * @brief The DxlMotorState class
 */
class DxlMotorState : public JointState
{
    public:
        DxlMotorState();
        DxlMotorState(uint8_t id);
        DxlMotorState(EHardwareType type, EComponentType component_type,
                      uint8_t id);

        DxlMotorState(std::string name, EHardwareType type, EComponentType component_type,
                      uint8_t id);

        DxlMotorState& operator= ( DxlMotorState && ) = delete;
        DxlMotorState& operator= ( const DxlMotorState& ) = delete;

        ~DxlMotorState() override = default;

        // getters
        bool isTool() const;

        // JointState interface
        std::string str() const override;
        void reset() override;
        bool isValid() const override;

        int to_motor_pos(double pos_rad) override;
        double to_rad_pos(int position_dxl) override;

        uint32_t getPositionPGain() const;
        uint32_t getPositionIGain() const;
        uint32_t getPositionDGain() const;

        uint32_t getVelocityPGain() const;
        uint32_t getVelocityIGain() const;

        uint32_t getFF1Gain() const;
        uint32_t getFF2Gain() const;

        double getStepsForOneSpeed() const;
        int getTotalRangePosition() const;
        int getMiddlePosition() const;
        double getTotalAngle() const;

        void setPositionPGain(uint32_t p_gain);
        void setPositionIGain(uint32_t i_gain);
        void setPositionDGain(uint32_t d_gain);

        void setVelocityPGain(uint32_t p_gain);
        void setVelocityIGain(uint32_t i_gain);

        void setFF1Gain(uint32_t ff1_gain);
        void setFF2Gain(uint32_t ff2_gain);

protected:
        uint32_t _pos_p_gain{0};
        uint32_t _pos_i_gain{0};
        uint32_t _pos_d_gain{0};

        uint32_t _vel_p_gain{0};
        uint32_t _vel_i_gain{0};

        uint32_t _ff1_gain{0};
        uint32_t _ff2_gain{0};

        int _total_range_position{0};
        int _middle_position{0};
        double _total_angle{0.0};
        double _steps_for_one_speed{0.0};

protected:
    // see https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c67-a-polymorphic-class-should-suppress-public-copymove
    DxlMotorState( const DxlMotorState& ) = default;
    DxlMotorState( DxlMotorState&& ) = default;
};

/**
 * @brief DxlMotorState::isTool
 * @return
 */
inline
bool DxlMotorState::isTool() const
{
    return (getComponentType() == common::model::EComponentType::TOOL);
}

/**
 * @brief DxlMotorState::getPositionPGain
 * @return
 */
inline
uint32_t DxlMotorState::getPositionPGain() const
{
    return _pos_p_gain;
}

/**
 * @brief DxlMotorState::getPositionIGain
 * @return
 */
inline
uint32_t DxlMotorState::getPositionIGain() const
{
    return _pos_i_gain;
}

/**
 * @brief DxlMotorState::getPositionDGain
 * @return
 */

inline
uint32_t DxlMotorState::getPositionDGain() const
{
    return _pos_d_gain;
}

/**
 * @brief DxlMotorState::getVelocityPGain
 * @return
 */
inline
uint32_t DxlMotorState::getVelocityPGain() const
{
    return _vel_p_gain;
}

/**
 * @brief DxlMotorState::getVelocityIGain
 * @return
 */
inline
uint32_t DxlMotorState::getVelocityIGain() const
{
    return _vel_i_gain;
}

/**
 * @brief DxlMotorState::getFF1Gain
 * @return
 */
inline
uint32_t DxlMotorState::getFF1Gain() const
{
    return _ff1_gain;
}

/**
 * @brief DxlMotorState::getFF2Gain
 * @return
 */
inline
uint32_t DxlMotorState::getFF2Gain() const
{
    return _ff2_gain;
}

} // namespace model
} // namespace common

#endif
