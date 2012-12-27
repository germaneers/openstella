#!/usr/bin/python

"""
  convert_pinmap.py
  
  converts pin_map.h from TIs driverlib to methods for libopenstellas GPIOPin class
  usage:  cat ../StellarisWare/driverlib/pin_map.h | python CreatePinmapCpp.py > PinMap.cpp 
 
  Copyright 2011, 2012 Germaneers GmbH
  Copyright 2011, 2012 Hubert Denkmair (hubert.denkmair@germaneers.com)
 
  This file is part of libopenstella.
 
  libopenstella is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License,
  or (at your option) any later version.
 
  libopenstella is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with libopenstella.  If not, see <http://www.gnu.org/licenses/>.
"""

import sys
import re

def portNum(port):
	return ord(port)-65;

part = ""
data = dict()
for line in sys.stdin:
	m = re.match("#ifdef PART_(\w*)",line)
	if m: 
		part = m.group(1)

	m = re.match("#define GPIO_P(.)(.)_(\w*).*",line)
	if m:
		port = m.group(1)
		pin  = m.group(2)
		func = m.group(3)
		
		if not part in data: data[part] = dict()
		if not func in data[part]: data[part][func] = dict()
		if not port in data[part][func]: data[part][func][port] = list()
		data[part][func][port].append(pin)

print """
#include "GPIO.h"
#include <stdint.h>
#include <StellarisWare/inc/hw_types.h>
#include <StellarisWare/driverlib/rom.h>
#include <StellarisWare/driverlib/gpio.h>
#include <StellarisWare/driverlib/rom_map.h>
#include <StellarisWare/driverlib/pin_map.h>
"""
for part in data:
	print "#ifdef PART_%s" % part
	
	for func in data[part]:
		print "void GPIOPin::mapAs%s() {" % (func)
		print "\tuint32_t cfg;"
		print "\tswitch (_port_pin) {"
		for port in data[part][func]:
			for pin in data[part][func][port]:
				print "\t\tcase 0x%02x: cfg = GPIO_P%s%s_%s; break; // port %s, pin %s" % (portNum(port)<<4 | int(pin), port, pin, func, port, pin)
		print "\t\tdefault: while(1);"
		print "\t}"
		print "\tMAP_GPIOPinConfigure(cfg);"
		print "}\n"

	print "#endif"

