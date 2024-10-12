# dc모터 구동 및 모형 차 구동

### 개요
 - 목표: MCU와 L293D 모터 드라이버를 활용한 차량 구동.
 - 기반 기술: PWM(Pulse Width Modulation) 제어 방식 및 STM32 MCU를 사용하여 모터 속도와 방향 제어.

### 결과 자료

</br>
<img src="/data/driver_output.gif" width="25%" height="30%" title="opamp_1_off"></img></br>


### 개발 과정
 - `1`. 주파수 분주
 - `2`. PWM을 통한 속도 제어
 - `3`. 차량 방향 제어 함수
 - `4`. (2,3 개선) 구조체를 통한 코드 개선

 1. TIM 주파수 분주
 - 방식 : 기본 **오실레이터 주파수를 분주하여 타이머 클럭 속도를 조절**합니다. 이후 **타이머 주기를 설정해 원하는 주기 간격으로 동작하도록 설정**합니다.
 - 구현 :  84MHz의 오실레이터 주파수를 1MHz로 분주한 후, 타이머의 Counter Period를 20,000 - 1로 설정하여 20ms의 타이머 주기를 구현하였습니다.
```
htim1.Instance = TIM1;
  htim1.Init.Prescaler = 84-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 20000-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
```


 2. PWM을 통한 속도 제어
 - 방식 : PWM을 사용하여 모터의 속도를 제어합니다. **PWM 신호의 Duty Cycle을 변경하여 모터에 공급되는 평균 전압을 조절함으로써 속도를 제어**할 수 있습니다.

 - 구현 : STM32의 타이머(TIMx) 채널을 이용해 모터의 EN 핀에 PWM 신호를 공급합니다. 이를 통해 모터 속도를 원하는 값으로 조절하였습니다.

```
void set_motor_speed(TIM_HandleTypeDef* htim, uint32_t channel, uint16_t speed) {
    __HAL_TIM_SET_COMPARE(htim, channel, speed);
}

void set_motor_direction(GPIO_TypeDef* port, uint16_t pin1, uint16_t pin2, uint8_t direction) {
    if (direction == 1) {
        HAL_GPIO_WritePin(port, pin1, GPIO_PIN_SET);  // FORWARD
        HAL_GPIO_WritePin(port, pin2, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(port, pin1, GPIO_PIN_RESET);  // REVERSE
        HAL_GPIO_WritePin(port, pin2, GPIO_PIN_SET);
    }
}

```

 3. 차량 방향 제어 함수
 - 구성 : **차량의 좌측 및 우측 모터의 방향을 제어하여 다양한 이동 동작을 구현**합니다. 이 설정을 통해 차량이 전진, 후진, 좌회전, 우회전 등의 동작을 수행할 수 있습니다.

 - 방식 : 주어진 direction 값에 따라 각 모터의 회전 방향을 제어하여 원하는 이동 동작을 수행합니다.

```
void set_car_direction(GPIO_TypeDef* port, uint16_t L_DIR_PIN1, uint16_t L_DIR_PIN2, uint16_t R_DIR_PIN1, uint16_t R_DIR_PIN2, uint8_t direction) {

	if (direction == 0) // 전진
	{
		set_motor_direction(MOTOR_DIR_PIN_PORT, L_DIR_PIN1, L_DIR_PIN2, 1);

		set_motor_direction(MOTOR_DIR_PIN_PORT, R_DIR_PIN1, R_DIR_PIN2, 1);
	}
	else if (direction == 1) // 후진
	{
		set_motor_direction(MOTOR_DIR_PIN_PORT, L_DIR_PIN1, L_DIR_PIN2, 0);

		set_motor_direction(MOTOR_DIR_PIN_PORT, R_DIR_PIN1, R_DIR_PIN2, 0);
	}
	else if (direction == 2) // 우회전
	{
		  // 좌측 모터를 앞으로
		set_motor_direction(MOTOR_DIR_PIN_PORT, L_DIR_PIN1, L_DIR_PIN2, 1);

		// 우측 모터를 뒤로
		set_motor_direction(MOTOR_DIR_PIN_PORT, R_DIR_PIN1, R_DIR_PIN2, 0);

		}
	else if (direction == 3) // 좌회전
	{
		// 좌측 모터를 뒤로
		set_motor_direction(MOTOR_DIR_PIN_PORT, L_DIR_PIN1, L_DIR_PIN2, 1);

		// 우측 모터를 앞으로
		set_motor_direction(MOTOR_DIR_PIN_PORT, R_DIR_PIN1, R_DIR_PIN2, 0);

	}

}
```


 4. (2,3개선) 구조체를 통한 코드 개선 
 - 방식 : **구조체를 활용하여 모터의 방향 및 속도 제어를 구조체 기반으로 개선**하였습니다. 이를 통해 모터 제어에 필요한 정보를 효율적으로 관리하고, 제어 로직의 일관성을 유지했습니다.

 - 구현 : 모터와 관련된 정보(포트, 핀, 타이머 등)를 하나의 구조체로 묶어 관리함으로써 코드의 가독성과 유지보수성을 높였습니다. 구조체를 통해 파라미터를 한 곳에 모아 관리함으로써 코드의 복잡성을 줄이고 유지보수를 용이하게 했습니다.

```
typedef struct {
    GPIO_TypeDef* port;
    uint16_t dir_pin1;
    uint16_t dir_pin2;
    TIM_HandleTypeDef* htim;
    uint32_t pwm_channel;
} MotorConfig;

void set_motor_direction(MotorConfig* motor, uint8_t direction) {
    if (direction == 1) {
        HAL_GPIO_WritePin(motor->port, motor->dir_pin1, GPIO_PIN_SET);  // FORWARD
        HAL_GPIO_WritePin(motor->port, motor->dir_pin2, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(motor->port, motor->dir_pin1, GPIO_PIN_RESET);  // REVERSE
        HAL_GPIO_WritePin(motor->port, motor->dir_pin2, GPIO_PIN_SET);
    }
}

void set_motor_speed(MotorConfig* motor, uint16_t speed) {
    __HAL_TIM_SET_COMPARE(motor->htim, motor->pwm_channel, speed);
}

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
```
MOTOR_DIR_PIN_PORT, L_DIR_PIN1, L_DIR_PIN2, 1
```
// 사용 예시
// 구조체를 통해 모터의 속도 및 방향 제어 코드를 간결하게 만들고, 모터 간 설정을 쉽게 관리할 수 있다
    // 좌측 및 우측 모터 설정
    MotorConfig left_motor = MOTOR_DIR_PIN_PORT, L_DIR_PIN1, L_DIR_PIN2, &htim1, TIM_CHANNEL_1};
    MotorConfig right_motor = MOTOR_DIR_PIN_PORT, R_DIR_PIN1, R_DIR_PIN2, &htim1, TIM_CHANNEL_2};

    // 차량 속도
    set_motor_speed(&left_motor, 500); // 좌측 속도 500
    set_motor_speed(&right_motor, 500); // 우측 속도 500

    // 차량 전진
    set_car_direction(&left_motor, &right_motor, 0);  // 전진 방향

    HAL_Delay(3000);  // 3초 동안 전진

    // 차량 좌회전
    set_car_direction(&left_motor, &right_motor, 3, 500);  // 좌회전
    HAL_Delay(3000);  // 3초 동안 좌회전
```


### 결과
 - 구조체를 사용하여 모터 관련 설정을 효율적으로 관리하였으며, **코드 가독성과 유지보수성이 크게 향상**되었습니다.

### 개선 사항
 1. 모터 특성 파악
 - 직접적인 전압, 전류 테스트를 통해 모터의 특성을 측정하고자 했으나, 실험 장비와 경험의 부족으로 어려움을 겪음.
 - 해결 방안 : 모터 사양에 대한 정보를 알 수 없을 경우, 실험을 통해 모터의 기동 전압 및 작동 전류를 측정하는 방법을 경험할 필요가 있음.
 2. 회로를 통한 사전 시뮬레이션
 - 문제: 모터 구동 회로가 실제 하드웨어에서 정상적으로 동작하지 않을 가능성을 사전에 방지하지 못함.
 - 해결 방안: 회로를 설계할 때 시뮬레이션 툴을 사용하여 미리 회로 동작을 검증할 필요가 있음.
