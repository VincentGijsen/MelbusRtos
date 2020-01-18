/*
 * usb.h
 *
 *  Created on: Jan 5, 2020
 *      Author: vincent
 */

#ifndef USB_H_
#define USB_H_

#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//lower number == higher prio
#define IRQ_PRI_USB (3<<4)

void usb_init();

bool usb_vcp_is_connected(void);

uint16_t usb_vcp_avail(void);
int usb_vcp_recv_byte(void);
void usb_vcp_send_byte(uint8_t ch);
void usb_vcp_send_strn(const char *str, size_t len);
void usb_vcp_send_strn_cooked(const char *str, size_t len);


#endif /* USB_H_ */
