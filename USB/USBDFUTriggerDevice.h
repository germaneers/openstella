/*
 * USBDFUTriggerDevice.h
 *
 *  Created on: 05.06.2013
 *      Author: denkmahu
 */

#ifndef USBDFUTRIGGERDEVICE_H_
#define USBDFUTRIGGERDEVICE_H_

#include "USBController.h"
#include "USBDevice.h"
#include <StellarisWare/usblib/device/usbddfu-rt.h>

class USBDFUTriggerDevice : public USBDevice {
private:
	tUSBDDFUDevice _dev;
	bool _shouldDetach;
public:
	USBDFUTriggerDevice(USBController *controller);
	virtual uint32_t getCompositeSize() { return COMPOSITE_DDFU_SIZE; }
	virtual tDeviceInfo *getDeviceInfoStruct()  { return &g_sDFUDeviceInfo; }
	virtual void enable();
	virtual void *compositeInit();

	void detachEventFromISR();

	bool shouldDetachToDFU();
	void beginUpdate();
};

#endif /* USBDFUTRIGGERDEVICE_H_ */
