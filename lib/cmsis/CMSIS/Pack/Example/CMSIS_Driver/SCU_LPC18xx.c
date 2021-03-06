/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2014 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        11. August 2014
 * $Revision:    V1.01
 *
 * Project:      SCU Driver for NXP LPC18xx
 * -------------------------------------------------------------------------- */
 
/* History:
 *  Version 1.01
 *    - Corrected SCU_SFSCLKx(clk_pin) and SCU_ENAIOx(n) macros
 *  Version 1.00
 *    - Initial release
 */

#include "LPC18xx.h"
#include "SCU_LPC18xx.h"

#define PORT_OFFSET          ( 0x80 )
#define PIN_OFFSET           ( 0x04 )
#define SCU_SFSPx(port, pin) (*((volatile uint32_t *) ((LPC_SCU_BASE + PORT_OFFSET * port + PIN_OFFSET * pin))))
#define SCU_SFSCLKx(clk_pin) (*((volatile uint32_t *) (&(LPC_SCU->SFSCLK_0) + clk_pin)))
#define SCU_ENAIOx(n)        (*((volatile uint32_t *) (&(LPC_SCU->ENAIO0) + n)))


/**
  \fn          int32_t SCU_PinConfiguare (uint8_t port, uint8_t pin, uint32_t pin_cfg)
  \brief       Set pin function and electrical characteristics
  \param[in]   port       Port number (0..15)
  \param[in]   pin        Pin number (0..31)
  \param[in]   pin_cfg    pin_cfg configuration bit mask
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_PinConfigure (uint8_t port, uint8_t pin, uint32_t pin_cfg) {

  if ((port > 15) || (pin > 31)) return -1;
  SCU_SFSPx(port, pin) = pin_cfg;
  return 0;
}

/**
  \fn          int32_t SCU_CLK_PinConfigure (uint8_t clk_pin, uint32_t pin_cfg)
  \brief       Set pin function and electrical characteristics for CLK pins
  \param[in]   clk_pin    Clock pin number should be 0..3
  \param[in]   pin_cfg    pin_cfg
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_CLK_PinConfigure (uint8_t pin_clk, uint32_t pin_cfg) {

  if (pin_clk > 3) return -1;
  SCU_SFSCLKx(pin_clk) = pin_cfg;
  return 0;
}

/**
  \fn          int32_t SCU_USB1_PinConfigure (uint32_t USB1_pin_cfg)
  \brief       Pin configuration for USB1 USB_DP/USBDM  pins
  \param[in]   USB1_pin_cfg   USB1_pin_cfg configuration bit mask
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_USB1_PinConfigure (uint32_t USB1_pin_cfg) {
  LPC_SCU->SFSUSB = USB1_pin_cfg;
  return 0;
}

/**
  \fn          int32_t SCU_I2C_PinConfigure (uint32_t I2C_mode)
  \brief       Set I2C pin configuration
  \param[in]   I2C_mode:  SCU_I2C_PIN_MODE_DISABLED
                          SCU_I2C_PIN_MODE_STANDARD_FAST
                          SCU_I2C_PIN_MODE_FAST_PLUS
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_I2C_PinConfigure (uint32_t I2C_mode) {

  switch (I2C_mode) {
    case SCU_I2C_PIN_MODE_DISABLED:      break;
    case SCU_I2C_PIN_MODE_STANDARD_FAST: break;
    case SCU_I2C_PIN_MODE_FAST_PLUS:     break;
    default: return -1;
  }
  LPC_SCU->SFSI2C0 = I2C_mode;
  return 0;
}

/**
  \fn          int32_t SCU_ADC_ChannelPinConfigure (uint8_t ADC_num, uint8_t channel, uint32_t cmd)
  \brief       ADC Channel configuration
  \param[in]   ADC_num:  0 = ADC0, 1 = ADC1
  \param[in]   channel:  channel number 0..7
  \param[in]   cmd:      1 - enabled, 0 - disabled
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_ADC_ChannelPinConfigure (uint8_t ADC_num, uint8_t channel, uint32_t cmd) {

  if ((ADC_num > 1) || (channel > 7) || (cmd > 1)) return -1;
  cmd ? (SCU_ENAIOx(ADC_num) |= (1 << channel)) : (SCU_ENAIOx(ADC_num) &= ~(1 << channel));
  return 0;
}

/**
  \fn          int32_t SCU_DAC_PinConfigure (uint32_t cmd)
  \brief       Analog function on P4_4
  \param[in]   cmd:      1 - enabled, 0 - disabled
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_DAC_PinConfigure (uint32_t cmd) {

  if (cmd > 1) return -1;
  cmd ? (LPC_SCU->ENAIO2 |= SCU_ENAIO2_DAC) : (LPC_SCU->ENAIO2 &= ~SCU_ENAIO2_DAC);
  return 0;
}

/**
  \fn          int32_t SCU_PinInterruptSourceSelect (uint8_t pin_int, uint8_t port, uint8_t pin)
  \brief       Select interrupt source pin
  \param[in]   pin_int:  pin interrupt 0..7
  \param[in]   port:     GPIO port number 0..7
  \param[in]   pin:      GPIO pin number 0..31
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t SCU_PinInterruptSourceSelect (uint8_t pin_int, uint8_t port, uint8_t pin) {

  if ((port > 7) || (pin > 31) || (pin_int > 7)) return -1;

  if (pin_int < 4) {
    LPC_SCU->PINTSEL0 &= ~(0xFF << (8 * pin_int));
    LPC_SCU->PINTSEL0 |=  ((pin | (port << 5)) << (8 * pin_int));
  } else {
    pin_int -= 4;
    LPC_SCU->PINTSEL1 &= ~(0xFF << (8 * pin_int));
    LPC_SCU->PINTSEL1 |=  ((pin | (port << 5)) << (8 * pin_int));
  }

  return 0;
}
