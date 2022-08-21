#include "hal_application.h"
#include "gpio.h"
#include "usbd_hid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

const uint8_t num_buttons = 4;
const uint8_t button_bitmask = 0b00001111;

#define GPIO_SW_1_PORT GPIOB
#define GPIO_SW_1_PIN GPIO_PIN_8
#define GPIO_SW_2_PORT GPIOB
#define GPIO_SW_2_PIN GPIO_PIN_9
#define GPIO_SW_3_PORT GPIOB
#define GPIO_SW_3_PIN GPIO_PIN_10
#define GPIO_SW_4_PORT GPIOB
#define GPIO_SW_4_PIN GPIO_PIN_11

#define GPIO_USERRIGHT_PORT GPIOB
#define GPIO_USERRIGHT_PIN GPIO_PIN_12
#define GPIO_USERLEFT_PORT GPIOB
#define GPIO_USERLEFT_PIN GPIO_PIN_13
#define GPIO_USERENTER_PORT GPIOB
#define GPIO_USERENTER_PIN GPIO_PIN_14

void send_usb_hid_report(uint8_t *report_data, uint8_t report_size)
{
    USBD_HID_SendReport(&hUsbDeviceFS, report_data, report_size);
}

hal_application_button_state_t update_button_states(void)
{
    //Poll the button states
    hal_application_button_state_t button_state = {0};
    button_state.manipulating_button_state |= !(HAL_GPIO_ReadPin(GPIO_SW_1_PORT, GPIO_SW_1_PIN)) << 0;
    button_state.manipulating_button_state |= !(HAL_GPIO_ReadPin(GPIO_SW_2_PORT, GPIO_SW_2_PIN)) << 1;
    button_state.manipulating_button_state |= !(HAL_GPIO_ReadPin(GPIO_SW_3_PORT, GPIO_SW_3_PIN)) << 2;
    button_state.manipulating_button_state |= !(HAL_GPIO_ReadPin(GPIO_SW_4_PORT, GPIO_SW_4_PIN)) << 3;

    button_state.lcd_button_left = HAL_GPIO_ReadPin(GPIO_USERLEFT_PORT, GPIO_USERLEFT_PIN);
    button_state.lcd_button_right = HAL_GPIO_ReadPin(GPIO_USERRIGHT_PORT, GPIO_USERRIGHT_PIN);
    button_state.lcd_button_enter = HAL_GPIO_ReadPin(GPIO_USERENTER_PORT, GPIO_USERENTER_PIN);

    return button_state;
}