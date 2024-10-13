#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "stm32f1xx_hal.h"

// 모터 설정 구조체 정의
typedef struct {
    GPIO_TypeDef* port;         // 모터의 GPIO 포트
    uint16_t dir_pin1;          // 모터의 방향 제어 핀 1
    uint16_t dir_pin2;          // 모터의 방향 제어 핀 2
    TIM_HandleTypeDef* htim;    // 타이머 핸들러
    uint32_t pwm_channel;       // PWM 채널
} MotorConfig;

// 함수 선언
void set_motor_direction(MotorConfig* motor, uint8_t direction);
void set_motor_speed(MotorConfig* motor, uint16_t speed);
void set_car_direction(MotorConfig* left_motor, MotorConfig* right_motor, uint8_t direction);

#endif // MOTOR_CONTROL_H