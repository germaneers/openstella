/*
 * USBDFUTriggerDevice.cpp
 *
 *  Created on: 05.06.2013
 *      Author: denkmahu
 */

#include "USBDFUTriggerDevice.h"
#include "USBDeviceDescriptor.h"
#include "../OS/Task.h"
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/driverlib/sysctl.h>

static unsigned long DFUDetachCallback(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgData, void *pvMsgData)
{
    if(ulEvent == USBD_DFU_EVENT_DETACH)
    {
    	((USBDFUTriggerDevice*) pvCBData)->detachEventFromISR();
    }

    return(0);
}

USBDFUTriggerDevice::USBDFUTriggerDevice(USBController *controller)
	: USBDevice(controller, new USBDefaultDeviceDescriptor(6)), _shouldDetach(false)
{
	USBDefaultDeviceDescriptor *desc = (USBDefaultDeviceDescriptor*)_deviceDescriptor;
	desc->setProductString("Openstella DFU Trigger");
	desc->setPrivateData(new tDFUInstance);

	_dev.pfnCallback = DFUDetachCallback;
	_dev.pvCBData = this;
	_dev.psPrivateDFUData = (tDFUInstance*)desc->getPrivateData();

}

void USBDFUTriggerDevice::enable()
{
}



void *USBDFUTriggerDevice::compositeInit()
{
	return USBDDFUCompositeInit(_controller->getNumber(), (tUSBDDFUDevice *)&_dev);
}

void USBDFUTriggerDevice::detachEventFromISR()
{
	_shouldDetach = true;
}

bool USBDFUTriggerDevice::shouldDetachToDFU()
{
	return _shouldDetach;
}

void USBDFUTriggerDevice::beginUpdate()
{
	vTaskSuspendAll();
	//USBDDFUUpdateBegin();
	uint32_t *x = (uint32_t*)0x20017FFC;
	*x = 0x23422342;
	SysCtlReset();
}






