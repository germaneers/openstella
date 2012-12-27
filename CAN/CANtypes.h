/*
 * CANtypes.h
 *
 * Copyright 2011 Germaneers GmbH
 * Copyright 2011 Hubert Denkmair (hubert.denkmair@germaneers.com)
 *
 * This file is part of libopenstella.
 *
 * libopenstella is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * libopenstella is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libopenstella.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef CANTYPES_H_
#define CANTYPES_H_

#include <stdint.h>

namespace CAN {

	typedef enum {
		channel_0,
		channel_1,
		channel_2,
		channel_none
	} channel_t;

	typedef enum {
		bitrate_100kBit  =  100000,
		bitrate_125kBit  =  125000,
		bitrate_500kBit  =  500000,
		bitrate_1000kBit = 1000000
	} bitrate_t;

	typedef enum {
		message_type_tx = 0,
		message_type_tx_remote = 1,
		message_type_rx = 2,
		message_type_rx_remote = 3,
		message_type_rxtx_remote = 4
	} message_type_t;

	typedef struct {
		uint32_t rx_errors;
		uint32_t tx_errors;
		bool passiveLimitReached;
	} error_counters_t;

}

#endif /* CANTYPES_H_ */
