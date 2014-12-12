/*
 * UDMAController.cpp
 *
 *  Created on: 23.10.2013
 *      Author: denkmahu
 */

#include "UDMAController.h"
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/inc/hw_udma.h>
#include <StellarisWare/driverlib/sysctl.h>
#include <StellarisWare/driverlib/udma.h>
#include <StellarisWare/driverlib/rom_map.h>

UDMAController *UDMAController::_instance = 0;

UDMAController::UDMAController()
{
}

UDMAController *UDMAController::getInstance()
{
	if (_instance==0) {
		_instance = new UDMAController();
	}
	return _instance;
}

void UDMAController::setup()
{
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
	MAP_uDMAEnable();
	MAP_uDMAControlBaseSet(ucControlTable);
}

void UDMAController::enableChannelAttribute(channel_t channel, uint32_t channel_attributes)
{
	MAP_uDMAChannelAttributeEnable(channel, channel_attributes);
}

void UDMAController::disableChannelAttribute(channel_t channel, uint32_t channel_attributes)
{
	MAP_uDMAChannelAttributeDisable(channel, channel_attributes);
}

void UDMAController::setControlParameters(channel_t channel, control_structure_used_t structure, uint32_t parameters)
{
	MAP_uDMAChannelControlSet(channel | structure, parameters);
}

void UDMAController::setTransferParameters(channel_t channel, control_structure_used_t structure, mode_t mode, void *source, void *destination, uint32_t len)
{
	MAP_uDMAChannelTransferSet(channel | structure, mode, source, destination, len);
}

UDMAController::mode_t UDMAController::getMode(channel_t channel, control_structure_used_t structure)
{
	return (mode_t)MAP_uDMAChannelModeGet(channel | structure);
}

bool UDMAController::getChannelInterruptStatus(channel_t channel)
{
	return (HWREG(UDMA_CHIS) & (1<<channel)) != 0;
}

void UDMAController::enableChannel(channel_t channel)
{
	MAP_uDMAChannelEnable(channel);
}

void UDMAController::disableChannel(channel_t channel)
{
	MAP_uDMAChannelDisable(channel);
}


