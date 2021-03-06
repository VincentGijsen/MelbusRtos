/* This file is part of the libopencm3 project.
 *
 * It was generated by the irq2nvic_h script from ./include/libopencm3/gd32/f1x0/irq.json
 */

#ifndef LIBOPENCM3_GD32F1X0_NVIC_H
#define LIBOPENCM3_GD32F1X0_NVIC_H

#include <libopencm3/cm3/nvic.h>

/** @defgroup CM3_nvic_defines_irqs User interrupts for GD32F1x0 Series
    @ingroup CM3_nvic_defines

    @{*/

#define NVIC_WWDG_IRQ 0
#define NVIC_PVD_IRQ 1
#define NVIC_RTC_IRQ 2
#define NVIC_FLASH_IRQ 3
#define NVIC_RCC_IRQ 4
#define NVIC_EXTI0_1_IRQ 5
#define NVIC_EXTI2_3_IRQ 6
#define NVIC_EXTI4_15_IRQ 7
#define NVIC_TSC_IRQ 8
#define NVIC_DMA_CHANNEL1_IRQ 9
#define NVIC_DMA_CHANNEL2_3_IRQ 10
#define NVIC_DMA_CHANNEL4_5_IRQ 11
#define NVIC_ADC_COMP_IRQ 12
#define NVIC_TIM1_BRK_UP_TRG_COM_IRQ 13
#define NVIC_TIM1_CC_IRQ 14
#define NVIC_TIM2_IRQ 15
#define NVIC_TIM3_IRQ 16
#define NVIC_TIM6_DAC_IRQ 17
#define NVIC_RESERVED0_IRQ 18
#define NVIC_TIM14_IRQ 19
#define NVIC_TIM15_IRQ 20
#define NVIC_TIM16_IRQ 21
#define NVIC_TIM17_IRQ 22
#define NVIC_I2C1_EV_IRQ 23
#define NVIC_I2C2_EV_IRQ 24
#define NVIC_SPI1_IRQ 25
#define NVIC_SPI2_IRQ 26
#define NVIC_USART1_IRQ 27
#define NVIC_USART2_IRQ 28
#define NVIC_RESERVED1_IRQ 29
#define NVIC_CEC_CAN_IRQ 30
#define NVIC_RESERVED2_IRQ 31
#define NVIC_I2C1_ER_IRQ 32
#define NVIC_RESERVED3_IRQ 33
#define NVIC_I2C2_ER_IRQ 34
#define NVIC_I2C3_EV_IRQ 35
#define NVIC_I2C3_ER_IRQ 36
#define NVIC_USB_LP_IRQ 37
#define NVIC_USB_HP_IRQ 38
#define NVIC_RESERVED4_IRQ 39
#define NVIC_RESERVED5_IRQ 40
#define NVIC_RESERVED6_IRQ 41
#define NVIC_USB_WAKEUP_IRQ 42
#define NVIC_RESERVED7_IRQ 43
#define NVIC_RESERVED8_IRQ 44
#define NVIC_RESERVED9_IRQ 45
#define NVIC_RESERVED10_IRQ 46
#define NVIC_RESERVED11_IRQ 47
#define NVIC_DMA_CHANNEL6_7_IRQ 48
#define NVIC_RESERVED12_IRQ 49
#define NVIC_RESERVED13_IRQ 50
#define NVIC_SPI3_IRQ 51

#define NVIC_IRQ_COUNT 52

/**@}*/

/** @defgroup CM3_nvic_isrprototypes_GD32F1x0 User interrupt service routines (ISR) prototypes for GD32F1x0 Series
    @ingroup CM3_nvic_isrprototypes

    @{*/

BEGIN_DECLS

void wwdg_isr(void);
void pvd_isr(void);
void rtc_isr(void);
void flash_isr(void);
void rcc_isr(void);
void exti0_1_isr(void);
void exti2_3_isr(void);
void exti4_15_isr(void);
void tsc_isr(void);
void dma_channel1_isr(void);
void dma_channel2_3_isr(void);
void dma_channel4_5_isr(void);
void adc_comp_isr(void);
void tim1_brk_up_trg_com_isr(void);
void tim1_cc_isr(void);
void tim2_isr(void);
void tim3_isr(void);
void tim6_dac_isr(void);
void reserved0_isr(void);
void tim14_isr(void);
void tim15_isr(void);
void tim16_isr(void);
void tim17_isr(void);
void i2c1_ev_isr(void);
void i2c2_ev_isr(void);
void spi1_isr(void);
void spi2_isr(void);
void usart1_isr(void);
void usart2_isr(void);
void reserved1_isr(void);
void cec_can_isr(void);
void reserved2_isr(void);
void i2c1_er_isr(void);
void reserved3_isr(void);
void i2c2_er_isr(void);
void i2c3_ev_isr(void);
void i2c3_er_isr(void);
void usb_lp_isr(void);
void usb_hp_isr(void);
void reserved4_isr(void);
void reserved5_isr(void);
void reserved6_isr(void);
void usb_wakeup_isr(void);
void reserved7_isr(void);
void reserved8_isr(void);
void reserved9_isr(void);
void reserved10_isr(void);
void reserved11_isr(void);
void dma_channel6_7_isr(void);
void reserved12_isr(void);
void reserved13_isr(void);
void spi3_isr(void);

END_DECLS

/**@}*/

#endif /* LIBOPENCM3_GD32F1X0_NVIC_H */
