#! /usr/bin/python3
# -*- coding: utf-8 -*-

import serial 
import cgi
import sys
print('Content-Type: text/plain	\n')

ser = serial.Serial()
ser.port = '/dev/ttyACM0'
ser.baudrate = 9600
ser.open() 
r = 'r'.encode("iso-8859-1")
ser.write(r)
ser.close()
print("success")
print(sys.path)
