#include <rclcpp/rclcpp.hpp>

#include <ackermann_msgs/msg/ackermann_drive_stamped.hpp>
#include <std_msgs/msg/int32_multi_array.hpp>

class AckermannToPWM : public rclcpp::Node
{
    public:
        AckermannToPWM();
    private:
        void onAckermannCmd(const ackermann_msgs::msg::AckermannDriveStamped::SharedPtr msg);

        // parameters
        int pwm_servo_channel_speed_;
        int pwm_servo_channel_steering_;
        double  pwm_speed_center_;
        double  pwm_speed_max_;
        double  pwm_speed_min_;
        double  pwm_steer_center_;
        double  pwm_steer_max_;
        double  pwm_steer_min_;
        double  max_speed_;
        double  max_steering_angle_;

        static constexpr std::size_t kPwmArraySize_ = 16;
        static constexpr int kUnusedChannelValue_ = -1;

        rclcpp::Subscription<ackermann_msgs::msg::AckermannDriveStamped>::SharedPtr ack_cmd_sub_;
        rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr pwm_cmd_pub_;
};


