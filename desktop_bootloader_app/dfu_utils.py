import serial
import struct
import os
import time
from enum import Enum

#Create enum class to store packet types
class PacketType(Enum):
    DFU_PACKET_START = 0,
    DFU_PACKET_DATA = 1,
    DFU_PACKET_ACK = 2,

class dfu_host:
    def __init__(self) -> None:
        self.SOF_IDENTIFIER = 0x01
        self.binary_size = 0
        
        self.ACK_STRUCT_CODE = ">BBH"
        self.default_byte_size = 60
        pass

    def open(self, comport: str) -> None:
        #Open the serial port
        self.ser = serial.Serial(comport, baudrate=115200)
        print("Serial port opened: " + self.ser.name)
        pass

    def upload_file(self, filename: str) -> None:
        #Upload the file to the microcontroller
        print("Uploading file: " + filename)
        with open(filename, 'rb') as f:
            #Send prog info packet
            prog_info_packet = self.__generate_prog_info_packet(filename)
            packet = self.__generate_packet_header(PacketType.DFU_PACKET_START, len(prog_info_packet))
            packet.extend(prog_info_packet)
            self.ser.write(packet)

            #Check for acknowledgement
            if not self.__check_acknowledgement(len(prog_info_packet)):
                return
            self.bytes_sent = 0
            while self.bytes_sent < self.binary_size:
                #Send data packet
                data_packet = f.read(self.default_byte_size)
                packet = self.__generate_packet_header(PacketType.DFU_PACKET_DATA, len(data_packet))
                packet.extend(data_packet)
                self.ser.write(packet)
                #Check for acknowledgement
                if not self.__check_acknowledgement(len(data_packet)):
                    return
                self.bytes_sent += len(data_packet)
                print("Bytes sent: " + str(self.bytes_sent))

        print("File uploaded")
        pass

    def __generate_packet_header(self, packet_type: PacketType, packet_length: int) -> bytes:
        #Generate the packet header
        packet = bytearray()
        packet.append(self.SOF_IDENTIFIER)
        packet.append(packet_type.value[0])
        chunk_size = packet_length.to_bytes(2, byteorder='big')
        packet.extend(chunk_size)
        return packet

    def __generate_prog_info_packet(self, filepath: str) -> bytes:
        #Generate the program info packet
        print("Generating program info packet")
        #store the binary size

        packet = bytearray()
        self.binary_size = int(os.stat(filepath).st_size)
        print("Binary size: " + str(self.binary_size))
        packet.extend(self.binary_size.to_bytes(4, byteorder='big'))

        for i in range(0, 4):
            packet.append(0x00) #TODO: CRC calculation. Not being implemented at this moment.

        for i in range(0, 4):
            packet.append(0x00) #Reserved bytes for later

        return packet

    def __check_acknowledgement(self, expected_packet_length: int) -> bool:
        response_len_bytes = struct.calcsize(self.ACK_STRUCT_CODE)
        response = self.ser.read(response_len_bytes)
        sof, packet_type, packet_length = struct.unpack(self.ACK_STRUCT_CODE, response)
        if sof == self.SOF_IDENTIFIER and packet_type == PacketType.DFU_PACKET_ACK.value[0] and packet_length == expected_packet_length:
            return True
        else:
            return False



    def close(self) -> None:
        #Close the serial port
        self.ser.close()
        print("Serial port closed")
        pass

