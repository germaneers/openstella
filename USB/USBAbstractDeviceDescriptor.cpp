/*
 * USBAbstractDeviceDescriptor.cpp
 *
 * Copyright 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
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


#include "USBAbstractDeviceDescriptor.h"
#include <string.h>

USBAbstractDeviceDescriptor::USBAbstractDeviceDescriptor(uint8_t numStringDescriptors)
  : _langDescriptor({
		4,
		USB_DTYPE_STRING,
		USBShort(USB_LANG_EN_US)
	}),
	_stringDescriptors(new pchar[numStringDescriptors])
{
	_stringDescriptors[0] = (char *)&_langDescriptor;
	for (int i=1; i<numStringDescriptors; i++) {
		_stringDescriptors[i] = 0;
	}
}

char *USBAbstractDeviceDescriptor::makeDescriptorString(const char *s)
{
	int inlen = strlen(s);
	int reslen = 2+(inlen*2);
	if (reslen>255) return 0;
	char *result = new char[reslen];
	memset(result, 0, reslen);
	result[0] = reslen;
	result[1] = USB_DTYPE_STRING;
	for (int i=0; i<inlen; i++) {
		result[2+(i*2)] = s[i];
	}
	return result;
}

void USBAbstractDeviceDescriptor::setDescriptorString(uint8_t desc_num, const char *s)
{
	if (_stringDescriptors[desc_num]!=0) {	delete(_stringDescriptors[desc_num]);	}
	_stringDescriptors[desc_num] = makeDescriptorString(s);
}
