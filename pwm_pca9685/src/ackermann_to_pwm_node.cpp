#include <rclcpp/create_subscription.hpp>
#include <rclcpp/publisher.hpp>
#include <rclcpp/rclcpp.hpp>

#include <ackermann_msgs/msg/ackermann_drive_stamped.hpp>
#include <std_msgs/msg/int32_multi_array.hpp>

class AckermannToPWM : public rclcpp::Node
{
    public:
        AckermannToPWM();
    private:
        double  pwm_speed_center = 300;
        double  pwm_speed_max = 400;
        double  pwm_speed_min = 200;
        double  pwm_steer_center = 300;
        double  pwm_steer_max = 400;
        double  pwm_steer_min = 200;
        double  max_speed = 1.0;
        double  max_steering_angle = 1.0472;

        rclcpp::Subscription<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr ack_cmd_sub_;
        rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr pwm_cmd_pub_;
};

AckermannToPWM::AckermannToPWM() : Node("ackermann_to_pwm_node")
{
            RCLCPP_INFO(this->get_logger(),"initializing");

            this->declare_parameter("pwm_speed_center", pwm_speed_center);
            this->declare_parameter("pwm_speed_max", pwm_speed_max);
            this->declare_parameter("pwm_speed_min", pwm_speed_min);
            this->declare_parameter("pwm_steer_center", pwm_steer_center);
            this->declare_parameter("pwm_steer_max", pwm_steer_max);
            this->declare_parameter("pwm_steer_min", pwm_steer_min);
            this->declare_parameter("max_speed", max_speed);
            this->declare_parameter("max_steering_steer", max_steering_angle);

            this->get_parameter("pwm_speed_center", pwm_speed_center);
            this->get_parameter("pwm_speed_max", pwm_speed_max);
            this->get_parameter("pwm_speed_min", pwm_speed_min);
            this->get_parameter("pwm_steer_center", pwm_steer_center);
            this->get_parameter("pwm_steer_max", pwm_steer_max);
            this->get_parameter("pwm_steer_min", pwm_steer_min);
            this->get_parameter("max_speed", max_speed);
            this->get_parameter("max_steering_steer", max_steering_angle);

            pwm_cmd_pub_ = this->create_publisher<std_msgs::msg::Int32MultiArray>("/command", 10);
            auto ackermann_callback =
                [this](ackermann_msgs::msg::AckermannDriveStamped::UniquePtr msg) -> void
                {
                    int pwm_speed = static_cast<int>(pwm_speed_center + (pwm_speed_max - pwm_speed_min) / 2 * msg->drive.speed / max_speed);
                    int pwm_steer = static_cast<int>(pwm_steer_center + (pwm_steer_max - pwm_steer_min) / 2 * msg->drive.steering_angle / max_steering_angle);
                    
                    auto cmd = std_msgs::msg::Int32MultiArray();
                    cmd.data = { pwm_speed, pwm_steer, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
                    this->pwm_cmd_pub_->publish(cmd);
                };
            ack_cmd_sub_ = this->create_subscription<ackermann_msgs::msg::AckermannDriveStamped>("/ackermann_cmd", 10, ackermann_callback);
}


int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<AckermannToPWM>());
    rclcpp::shutdown();
    return 0;
}
