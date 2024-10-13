#include "motor_control.h"

// 모터 방향 설정 함수
void set_motor_direction(MotorConfig* motor, uint8_t direction) {
    if (direction == 1) {
        HAL_GPIO_WritePin(motor->port, motor->dir_pin1, GPIO_PIN_SET);  // 전진
        HAL_GPIO_WritePin(motor->port, motor->dir_pin2, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(motor->port, motor->dir_pin1, GPIO_PIN_RESET);  // 후진
        HAL_GPIO_WritePin(motor->port, motor->dir_pin2, GPIO_PIN_SET);
    }
}

// 모터 속도 설정 함수
void set_motor_speed(MotorConfig* motor, uint16_t speed) {
    __HAL_TIM_SET_COMPARE(motor->htim, motor->pwm_channel, speed);
}

// 차량의 방향을 설정하는 함수
void set_car_direction(MotorConfig* left_motor, MotorConfig* right_motor, uint8_t direction) {
    if (direction == 0) {
        set_motor_direction(left_motor, 1);  // 좌측 모터 전진
        set_motor_direction(right_motor, 1); // 우측 모터 전진
    } else if (direction == 1) {
        set_motor_direction(left_motor, 0);  // 좌측 모터 후진
        set_motor_direction(right_motor, 0); // 우측 모터 후진
    } else if (direction == 2) {
        set_motor_direction(left_motor, 1);  // 좌측 모터 전진
        set_motor_direction(right_motor, 0); // 우측 모터 후진
    } else if (direction == 3) {
        set_motor_direction(left_motor, 0);  // 좌측 모터 후진
        set_motor_direction(right_motor, 1); // 우측 모터 전진
    }
}
