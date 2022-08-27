import serial
import struct
import os
import time
from enum import Enum

#Create enum class to store packet types
class PacketType(Enum):

class dfu_host:
    def __init__(self) -> None:
        self.SOF_IDENTIFIER = 0x01
        pass

    def open(self, comport: str) -> None:
        #Open the serial port
        self.ser = serial.Serial(comport, baudrate=115200, timeout=1)
        print("Serial port opened: " + self.ser.name)
        pass

    def upload_file(self, filename: str) -> None:
        #Upload the file to the microcontroller
        with open(filename, 'rb') as f:
            pass
        print("File uploaded")
        pass

    def __generate_packet_header(self, packet_type: int, packet_length: int) -> bytes:
        #Generate the packet header
        return struct.pack('<BBH', packet_type, 0, packet_length)
        pass

    def close(self) -> None:
        #Close the serial port
        self.ser.close()
        print("Serial port closed")
        pass

