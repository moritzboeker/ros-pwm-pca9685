import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution

def generate_launch_description():

    config = LaunchConfiguration('config')
    config_filepath = LaunchConfiguration('config_filepath')

    return LaunchDescription([

        DeclareLaunchArgument('config', default_value='esc_diff_drive'),
        DeclareLaunchArgument('config_filepath', default_value=[
            PathJoinSubstitution([FindPackageShare('pwm_pca9685'), 'config', config, '.config.yaml'])]),

        # Node(
        #     package='pwm_pca9685', executable='mixer_node',
        #     name='mixer_node',
        #     remappings=[
        #             ('/cmd_vel', '/cmd_vel'),
        #             ('/command', '/command'),
        #     ],
        #     parameters=[config_filepath]
        # ),

        Node(
            package='pwm_pca9685', executable='pca9685_node',
            name='pwm_node',
            remappings=[
                    ('/command', '/command'),
            ],
            parameters=[config_filepath]
        ),

        Node(
            package='pwm_pca9685', executable='ackermann_to_pwm_node',
            name='ackermann_to_pwm_node',
            remappings=[
                    ('/ackermann_cmd', '/ackermann_cmd'),
                    ('/command', '/command'),
            ],
            parameters=[config_filepath]
        ),


    ])

