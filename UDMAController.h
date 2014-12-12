/*
 * UDMAController.h
 *
 *  Created on: 23.10.2013
 *      Author: denkmahu
 */

#ifndef UDMACONTROLLER_H_
#define UDMACONTROLLER_H_

#include <stdint.h>

class UDMAController {
private:
	unsigned char ucControlTable[1024] __attribute__ ((aligned(1024)));
	static UDMAController *_instance;
	UDMAController();

public:

	typedef enum {
		channel_usbep1rx = 0,
		channel_usbep1tx = 1,
		channel_usbep2rx = 2,
		channel_usbep2tx = 3,
		channel_usbep3rx = 4,
		channel_usbep3tx = 5,
		channel_eth0rx   = 6,
		channel_eth0tx   = 7,
		channel_uart0rx  = 8,
		channel_uart0tx  = 9,
		channel_ssi0rx  = 10,
		channel_ssi0tx  = 11,
		channel_adc0    = 14,
		channel_adc1    = 15,
		channel_adc2    = 16,
		channel_adc3    = 17,
		channel_tmr0a   = 18,
		channel_tmr0b   = 19,
		channel_tmr1a   = 20,
		channel_tmr1b   = 21,
		channel_uart1rx = 22,
		channel_uart1tx = 23,
		channel_ssi1rx  = 24,
		channel_ssi1tx  = 25,
		channel_i2s0rx  = 28,
		channel_i2s0tx  = 29,
		channel_sw      = 30
	} channel_t;

	typedef enum {
		structure_primary = 0x00,
		structure_alternative = 0x20
	} control_structure_used_t;

	typedef enum {
		attribute_use_burst = 1,
		attribute_alt_select = 2,
		attribute_high_prio = 4,
		attribute_req_mask = 8,
		attribute_all = 0x0F
	} channel_attribute_t;

	typedef enum {
		param_dst_inc_8    = 0x00000000,
		param_dst_inc_16   = 0x40000000,
		param_dst_inc_32   = 0x80000000,
		param_dst_inc_none = 0xc0000000,
		param_src_inc_8    = 0x00000000,
		param_src_inc_16   = 0x40000000,
		param_src_inc_32   = 0x80000000,
		param_src_inc_none = 0x0c000000,
		param_size_8       = 0x00000000,
		param_size_16      = 0x11000000,
		param_size_32      = 0x22000000,
		param_arb_1			= 0x00000000,
		param_arb_2			= 0x00004000,
		param_arb_4			= 0x00008000,
		param_arb_8			= 0x0000c000,
		param_arb_16		= 0x00010000,
		param_arb_32		= 0x00014000,
		param_arb_64		= 0x00018000,
		param_arb_128		= 0x0001c000,
		param_arb_256		= 0x00020000,
		param_arb_512		= 0x00024000,
		param_arb_1024		= 0x00028000,
		param_use_burst     = 0x00000008
	} channel_parameters_t;

	typedef enum {
		mode_stop = 0,
		mode_basic = 1,
		mode_auto = 2,
		mode_pingpong = 3,
		mode_mem_scatter_gather = 4,
		mode_per_scatter_gather = 6,
		mode_alt_select = 1
	} mode_t;

	static UDMAController* getInstance();
	void setup();

	void enableChannelAttribute(channel_t channel, uint32_t channel_attributes);
	void disableChannelAttribute(channel_t channel, uint32_t channel_attributes);

	void setControlParameters(channel_t channel, control_structure_used_t structure, uint32_t parameters);
	void setTransferParameters(channel_t channel, control_structure_used_t structure, mode_t mode, void *source, void *destination, uint32_t len);

	mode_t getMode(channel_t channel, control_structure_used_t structure);

	bool getChannelInterruptStatus(channel_t channel);

	void enableChannel(channel_t channel);
	void disableChannel(channel_t channel);
};

#endif /* UDMACONTROLLER_H_ */
