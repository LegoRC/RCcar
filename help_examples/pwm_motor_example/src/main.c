#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// Both in pins are for direction, forward & backward
#define IN1_PIN 15 //forwards
#define IN2_PIN 16 //backwards
#define ENABLE1_PIN 17 // enables motor

// Button pins
#define FORWARD_BTN_PIN 14 // to drive forward
#define BACKWARD_BTN_PIN 13 // to drive backward

// PWM duty cycle for speed (0 - 65535) is determined by pwm_set_wrap function. Constant value maybe make dynamic for RCCar?
#define MOTOR_SPEED 65535

// PWM slice number
uint slice_num;

void setup_pwm(uint gpio_pin) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);  // activate pwm on given gpio_pin
    slice_num = pwm_gpio_to_slice_num(gpio_pin);  // get slice number of that pin
    pwm_set_wrap(slice_num, 65535);  // 16-bit resolution. Here you choose duty cycle. In other words, What is max?
    pwm_set_enabled(slice_num, true); // activate pin "or slice"
}

void drive_motor(bool direction, uint16_t speed) {
    // Set direction
    gpio_put(IN1_PIN, direction);
    gpio_put(IN2_PIN, !direction);

    // Set speed
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(ENABLE1_PIN), speed);

    // GPIO share frequency and wrap but not duty cycle. For example GPIO 0 and GPIO 1 both have the same slicenum which is 0 BUT
    // they are on different channels. GPIO 0 == CHANNEL A & GPIO 1 == CHANNEL B.

    // wrap == highest value pwm-counter counts to before re-count.
    // frequency == how many times per second a PWM-cycle is run.
    // wrap and frequency is set to default settings if not specified with pwm_set_wrap() & pwm_set_clkdiv()

}

void setup_gpio() {
    // Motor pins
    gpio_init(IN1_PIN);
    gpio_set_dir(IN1_PIN, GPIO_OUT);
    gpio_init(IN2_PIN);
    gpio_set_dir(IN2_PIN, GPIO_OUT);

    // Buttons with pull-ups
    gpio_init(FORWARD_BTN_PIN);
    gpio_set_dir(FORWARD_BTN_PIN, GPIO_IN);
    gpio_pull_up(FORWARD_BTN_PIN);

    gpio_init(BACKWARD_BTN_PIN);
    gpio_set_dir(BACKWARD_BTN_PIN, GPIO_IN);
    gpio_pull_up(BACKWARD_BTN_PIN);

    // PWM setup
    setup_pwm(ENABLE1_PIN);
}

int main() {
    stdio_init_all();
    setup_gpio();

    while (true) {
        bool forward = !gpio_get(FORWARD_BTN_PIN);   // Active LOW
        bool backward = !gpio_get(BACKWARD_BTN_PIN); // Active LOW

        if (forward && !backward) {
            drive_motor(true, MOTOR_SPEED);  // Forward
        } else if (!forward && backward) {
            drive_motor(false, MOTOR_SPEED); // Backward
        } else {
            drive_motor(true, 0); // Stop
        }

        sleep_ms(10);
    }

    return 0;
}
