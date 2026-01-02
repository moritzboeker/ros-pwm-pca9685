#include <algorithm>

#include <pwm_pca9685/ackermann_to_pwm_node.h>

AckermannToPWM::AckermannToPWM() : Node("ackermann_to_pwm_node")
{
            RCLCPP_INFO(this->get_logger(),"ackermann_to_pwm_node initializing");

            pwm_servo_channel_speed_ = this->declare_parameter("pwm_servo_channel_speed", 0);
            pwm_servo_channel_steering_ = this->declare_parameter("pwm_servo_channel_steering", 1);
            pwm_speed_center_ = this->declare_parameter("pwm_speed_center", 300);
            pwm_speed_max_ = this->declare_parameter("pwm_speed_max", 400);
            pwm_speed_min_ = this->declare_parameter("pwm_speed_min", 200);
            pwm_steer_center_ = this->declare_parameter("pwm_steer_center", 300);
            pwm_steer_max_ = this->declare_parameter("pwm_steer_max", 400);
            pwm_steer_min_ = this->declare_parameter("pwm_steer_min", 200);
            max_speed_ = this->declare_parameter("max_speed", 1.0);
            max_steering_angle_ = this->declare_parameter("max_steering_angle", 1.0472);

            RCLCPP_INFO(
                this->get_logger(),
                "PWM servo channels: speed->%d, steering->%d", pwm_servo_channel_speed_, pwm_servo_channel_steering_);
            RCLCPP_INFO(
                this->get_logger(),
                "PWM speed [%f, %f], center=%f | max_speed=%f",
                pwm_speed_min_, pwm_speed_max_, pwm_speed_center_, max_speed_
            );

            RCLCPP_INFO(
                this->get_logger(),
                "PWM steer [%f, %f], center=%f | max_steer=%f",
                pwm_steer_min_, pwm_steer_max_, pwm_steer_center_, max_steering_angle_
            );

            pwm_cmd_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>("/command", 10);
            ack_cmd_sub_ = this->create_subscription<ackermann_msgs::msg::AckermannDriveStamped>(
              "/ackermann_cmd",
              rclcpp::QoS(1),
              std::bind(&AckermannToPWM::onAckermannCmd, this, std::placeholders::_1)
            );
}

void AckermannToPWM::onAckermannCmd(const ackermann_msgs::msg::AckermannDriveStamped::SharedPtr msg)
{
    double speed_norm = std::clamp<double>(msg->drive.speed / max_speed_, -1.0, 1.0);
    double steer_norm = std::clamp<double>(msg->drive.steering_angle / max_steering_angle_, -1.0, 1.0);

    int pwm_speed = static_cast<int>(pwm_speed_center_ + speed_norm * (pwm_speed_max_ - pwm_speed_center_));
    int pwm_steer = static_cast<int>(pwm_steer_center_ + steer_norm * (pwm_steer_max_ - pwm_steer_center_));
    
    pwm_speed = std::clamp<int>(pwm_speed, pwm_speed_min_, pwm_speed_max_);
    pwm_steer = std::clamp<int>(pwm_steer, pwm_steer_min_, pwm_steer_max_);
    
    std_msgs::msg::Int32MultiArray pwm_cmd;
    pwm_cmd.data.resize(kPwmArraySize_, kUnusedChannelValue_);
    pwm_cmd.data[pwm_servo_channel_speed_] = pwm_speed;
    pwm_cmd.data[pwm_servo_channel_steering_] = pwm_steer;
    this->pwm_cmd_pub_->publish(pwm_cmd);
}


int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AckermannToPWM>());
    rclcpp::shutdown();
    return 0;
}
