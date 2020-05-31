/*  Projecto Tortuga: Módulo de locomoción básico  
    ESP32 Wroom
    Control de motores: Configuración 4 motores controlados con 2 L298N
    Control de giro mediante difernecial de velocidad ejes opuestos
    
    Inicio 20/05/2020

    GMV
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"

#define MOTOR0_A 17
#define MOTOR0_B 16
#define MOTOR1_A 32   
#define MOTOR1_B 33

#define LEDPIN 15

float speed = 80; //Velocidad mínima aceptable: 40

static void config_gpio(void)
{
    printf("Configurando gpio para mcpwm...\n");
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTOR0_A);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, MOTOR0_B);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, MOTOR1_A);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, MOTOR1_B);
    printf("Configurando gpio led...\n");
    gpio_pad_select_gpio(LEDPIN);
    gpio_set_direction(LEDPIN, GPIO_MODE_OUTPUT);

}


static void motor_forward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_A, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0); 
}


static void motor_backward(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num , float duty_cycle)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_duty(mcpwm_num, timer_num, MCPWM_OPR_B, duty_cycle);
    mcpwm_set_duty_type(mcpwm_num, timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);  
}


static void motor_stop(mcpwm_unit_t mcpwm_num, mcpwm_timer_t timer_num)
{
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_A);
    mcpwm_set_signal_low(mcpwm_num, timer_num, MCPWM_OPR_B);
}

static void robot_forward(float duty_cycle)
{
    motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_cycle);
    motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty_cycle);
}

static void robot_left(float duty_cycle)
{
    motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_cycle);
    motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty_cycle);
}

static void robot_right(float duty_cycle)
{
    motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_cycle);
    motor_forward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty_cycle); 
} 

static void robot_backward(float duty_cycle)
{
    motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_0, duty_cycle);
    motor_backward(MCPWM_UNIT_0, MCPWM_TIMER_1, duty_cycle);
}

static void robot_stop(void)
{
    motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
    motor_stop(MCPWM_UNIT_0, MCPWM_TIMER_1);
}

static void test_motor_control(void *arg)
{
    //Configuración GPIO
    config_gpio();

    //Presets mcpwm
    printf("Parametros iniciales mcpwm...\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;    
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);
    //mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_2, &pwm_config);
    //mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);
   // vTaskDelay(5000/ portTICK_RATE_MS);
    while (1) {
        robot_forward(50);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
        robot_left(80);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
        robot_forward(50);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
        robot_right(80);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
        robot_forward(50);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
        robot_backward(50);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
        robot_stop();
        vTaskDelay(1000/ portTICK_PERIOD_MS);
        } 
}

static void led_blink (void *arg)
{
        printf("Led OFF\n");
        gpio_set_level(LEDPIN, 0);
        vTaskDelay(1000/ portTICK_PERIOD_MS); 
        printf("Led ON\n");
        gpio_set_level(LEDPIN, 1);
        vTaskDelay(1000/ portTICK_PERIOD_MS);
}

void app_main(void)
{
    printf("Test motores...\n");
    xTaskCreate(test_motor_control, "Test_motor_control", 4096, NULL, 5, NULL);
    xTaskCreate(led_blink, "Led", 1024, NULL, 6, NULL);
}
