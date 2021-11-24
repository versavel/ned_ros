#!/usr/bin/env python

# To use the API, copy these 4 lines on each Python file you create
from niryo_robot_python_ros_wrapper.ros_wrapper import *
import rospy
import time
import math

WHITE = [255, 255, 255]
GREEN = [50, 255, 0]
BLACK = [0, 0, 0]
BLUE = [15, 50, 255]
PURPLE = [153, 51, 153]
PINK = [255, 0, 255]
RED = [255, 0, 0]
CYAN = [0, 255, 255]

rospy.init_node('niryo_robot_example_python_ros_wrapper')

print "--- Start"

n = NiryoRosWrapper()

try:
    # Calibrate robot first
    n.calibrate_auto()
    n.update_tool()
    n.grasp_with_tool()
    print "Calibration finished !"

    print("Wave")
    n.led_ring.breath(BLUE)
    n.move_joints(*(6 * [0]))
    n.set_arm_max_velocity(10)
    # Niryo wave
    pose_list = [
        [0.3, -0.16, 0.325, 0, 0, 0],
        [0.3, -0.13, 0.32, 0, 0, 0],
        [0.3, -0.09, 0.30, 0, 0, 0],
        [0.3, -0.06, 0.26, 0, 0, 0],
        [0.3, -0.03, 0.21, 0, 0, 0],
        [0.3, 0, 0.2, 0, 0, 0],
        [0.3, 0.03, 0.21, 0, 0, 0],
        [0.3, 0.06, 0.26, 0, 0, 0],
        [0.3, 0.09, 0.30, 0, 0, 0],
        [0.3, 0.13, 0.32, 0, 0, 0],
        [0.3, 0.16, 0.325, 0, 0, 0],
    ]
    full_traj = pose_list[:]
    pose_list.reverse()
    full_traj += pose_list[:]

    n.move_pose(*full_traj[0])
    n.execute_trajectory_from_poses(full_traj, 0.001)
    n.wait(0.5)

    n.set_arm_max_velocity(100)

    print("Moves")
    n.led_ring.chase(GREEN)
    n.move_pose(*[0.094, -0.085, 0.3, 0.927, 1.157, 0.899])
    n.move_pose(*[0.029, 0.217, 0.3, 2.254, 1.476, -2.38])
    n.execute_trajectory_from_poses_and_joints(
        [[0.029, 0.217, 0.3, 2.254, 1.476, -2.38], [0.029, 0.217, 0.15, 2.254, 1.476, -2.38],
         [0.03, 0.217, 0.3, 2.254, 1.476, -2.38], [0.159, 0.126, 0.3, 0.062, 1.535, 0.96],
         [0.159, 0.126, 0.15, 0.062, 1.535, 0.96], [0.16, 0.126, 0.3, 0.062, 1.535, 0.96],
         [0.219, -0.019, 0.3, -1.555, 1.544, -1.526], [0.219, -0.019, 0.15, -1.555, 1.544, -1.526],
         [0.22, -0.019, 0.3, -1.555, 1.544, -1.526], [0.16, -0.175, 0.3, -2.693, 1.529, 2.976],
         [0.16, -0.175, 0.15, -2.693, 1.529, 2.976], [0.17, -0.175, 0.3, -2.693, 1.529, 2.976],
         [-0.015, -0.229, 0.3, -2.552, 1.563, 2.298], [-0.015, -0.229, 0.15, -2.552, 1.563, 2.298],
         [-0.016, -0.229, 0.3, -2.552, 1.563, 2.298]],
        ["pose", "pose", "pose", "pose", "pose", "pose", "pose", "pose", "pose", "pose", "pose", "pose", "pose", "pose",
         "pose"], 0.01)
    n.move_pose(*[0.179, 0.001, 0.264, 2.532, 1.532, 2.618])

    print("Spiral")
    n.led_ring.snake(PINK)
    n.move_pose(0.3, 0, 0.3, 0, 0, 0)
    n.move_spiral(0.1, 5, 216, 1)

    print("Moves")
    n.led_ring.breath(BLUE)
    n.execute_trajectory_from_poses_and_joints(
        [[0.18, 0, 0.17, 0, 0, 0], [0.35, 0, 0.17, 0, 0, 0], [0.18, 0, 0.17, 0, 0, 0],
         [0.065, -0.196, 0.341, -0.099, 0.016, 0.06], [0.096, -0.06, 0.478, -1.695, 0.024, 0.029],
         [0.075, 0.117, 0.289, -0.116, 0.087, -0.013], [0.047, 0.221, 0.189, 2.689, 1.522, 2.826],
         [0.047, 0.23, 0.35, 2.689, 1.522, 2.826], [0, 0.5, -1.25, 0, 0, 0]],
        ["pose", "pose", "pose", "pose", "pose", "pose", "pose", "pose", "joint"], 0)

    print("Circle")
    n.led_ring.breath(PINK)
    n.move_pose(0.3, 0, 0.4, 0, 0, 0)
    n.move_circle(0.3, 0, 0.3)

    print("Moves")
    n.led_ring.rainbow()
    n.execute_trajectory_from_poses_and_joints(
        [[0.1, -0.01, 0.531, -0.045, 0.011, -0.074], [0.152, -0.005, 0.301, -0.063, -0.073, -0.036],
         [-0.011, -0.215, 0.229, -0.307, 0.045, -0.551], [0.009, -0.117, 0.369, 0.437, 0.497, -0.223],
         [-0.018, -0.2, 0.15, -1.57, 0, -1.57], [-0.018, -0.2, 0.3, -1.57, 0, -1.57]],
        ["pose", "pose", "pose", "pose", "pose", "pose"], 0)

    print("Linear")
    n.led_ring.go_up_down(PURPLE)
    n.move_pose(0.2, 0.2, 0.2, 0, 1.57, 0)
    n.move_linear_pose(0.2, -0.2, 0.2, 0, 1.57, 0)

    print("Moves")
    n.led_ring.chase(WHITE)
    n.execute_trajectory_from_poses_and_joints(
        [[0.18, 0, 0.17, 0, 0, 0], [0.35, 0, 0.17, 0, 0, 0], [0.18, 0, 0.17, 0, 0, 0],
         [0.065, -0.196, 0.341, -0.099, 0.016, 0.06], [0.096, -0.06, 0.478, -1.695, 0.024, 0.029],
         [0.075, 0.117, 0.289, -0.116, 0.087, -0.013], [0.047, 0.221, 0.189, 2.689, 1.522, 2.826],
         [0.047, 0.23, 0.35, 2.689, 1.522, 2.826], [0, 0.5, -1.25, 0, 0, 0]],
        ["pose", "pose", "pose", "pose", "pose", "pose", "pose", "pose", "joint"], 0)

    # Slalome
    print("Slalome")
    n.led_ring.rainbow_cycle()
    start_pose = [0.2, -0.2, 0.15, 0, 1.57, 0]
    pose_list = [start_pose]
    for _ in range(2):
        new_pose = pose_list[-1][:]

        new_pose[0] += 0.1
        pose_list.append(new_pose[:])
        new_pose[1] += 0.1
        pose_list.append(new_pose[:])
        new_pose[0] -= 0.1
        pose_list.append(new_pose[:])
        new_pose[1] += 0.1
        pose_list.append(new_pose[:])
    for _ in range(2):
        new_pose = pose_list[-1][:]

        new_pose[0] += 0.1
        pose_list.append(new_pose[:])
        new_pose[1] -= 0.1
        pose_list.append(new_pose[:])
        new_pose[0] -= 0.1
        pose_list.append(new_pose[:])
        new_pose[1] -= 0.1
        pose_list.append(new_pose[:])

    n.move_pose(*pose_list[0])
    n.execute_trajectory_from_poses(pose_list, 0.001)

    print("Moves")
    n.execute_trajectory_from_poses_and_joints(
        [[0.1, -0.01, 0.531, -0.045, 0.011, -0.074], [0.152, -0.005, 0.301, -0.063, -0.073, -0.036],
         [-0.011, -0.215, 0.229, -0.307, 0.045, -0.551], [0.009, -0.117, 0.369, 0.437, 0.497, -0.223],
         [-0.018, -0.2, 0.15, -1.57, 0, -1.57], [-0.018, -0.2, 0.3, -1.57, 0, -1.57]],
        ["pose", "pose", "pose", "pose", "pose", "pose"], 0)

    print("Pick and place")
    n.led_ring.snake(CYAN)
    pick_1 = [
        [0.25, -0.15, 0.25, 0, 1.57, 0],
        [0.25, -0.15, 0.145, 0, 1.57, 0],
    ]
    place_1 = [
        [0.25, -0.15, 0.35, 0, 1.57, 0],
        [0.25, 0.05, 0.35, 0, 1.57, 0],
        [0.25, 0.05, 0.2, 0, 1.57, 0],
    ]

    pick_2 = [
        [0.25, 0.05, 0.25, 0, 1.57, 0],
        [0.25, 0.15, 0.25, 0, 1.57, 0],
        [0.25, 0.15, 0.145, 0, 1.57, 0],
    ]
    place_2 = [
        [0.25, 0.15, 0.35, 0, 1.57, 0],
        [0.25, -0.05, 0.35, 0, 1.57, 0],
        [0.25, -0.05, 0.2, 0, 1.57, 0],
    ]

    end_pose = [
        [0.25, -0.05, 0.25, 0, 1.57, 0],
        [0.2, 0, 0.3, 0, 1.57, 0],
    ]

    n.release_with_tool()
    n.execute_trajectory_from_poses(pick_1, 0.002)
    n.close_gripper(max_torque_percentage=100, hold_torque_percentage=100)
    n.execute_trajectory_from_poses(place_1, 0.002)
    n.release_with_tool()
    n.execute_trajectory_from_poses(pick_2, 0.002)
    n.close_gripper(max_torque_percentage=100, hold_torque_percentage=100)
    n.execute_trajectory_from_poses(place_2, 0.002)
    n.release_with_tool()
    n.execute_trajectory_from_poses(end_pose, 0.002)

    print("Wave")
    n.led_ring.breath(BLUE)
    n.move_joints(*(6 * [0]))
    n.set_arm_max_velocity(50)
    # Niryo wave
    pose_list = [
        [0.3, -0.16, 0.325, 0, 0, 0],
        [0.3, -0.13, 0.32, 0, 0, 0],
        [0.3, -0.09, 0.30, 0, 0, 0],
        [0.3, -0.06, 0.26, 0, 0, 0],
        [0.3, -0.03, 0.21, 0, 0, 0],
        [0.3, 0, 0.2, 0, 0, 0],
        [0.3, 0.03, 0.21, 0, 0, 0],
        [0.3, 0.06, 0.26, 0, 0, 0],
        [0.3, 0.09, 0.30, 0, 0, 0],
        [0.3, 0.13, 0.32, 0, 0, 0],
        [0.3, 0.16, 0.325, 0, 0, 0],
    ]
    full_traj = pose_list[:]
    pose_list.reverse()
    full_traj += pose_list[:]

    n.move_pose(*full_traj[0])
    n.wait(0.5)
    n.execute_trajectory_from_poses(full_traj, 0.001)
    n.wait(0.5)

    n.set_arm_max_velocity(100)

except NiryoRosWrapperException as e:
    print e
    # handle exception here
    # you can also make a try/except for each command separately

print "--- End"
