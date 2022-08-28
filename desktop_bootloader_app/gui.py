import sys
from dfu_utils import dfu_host

#Starter code from:
# https://realpython.com/python-pyqt-gui-calculator/

# 1. Import `QApplication` and all the required widgets
from PyQt5.QtWidgets import QApplication, QLabel, QWidget, QComboBox, QLineEdit, QPushButton

import serial.tools.list_ports

class Window(QWidget):
    def __init__(self, parent=None) -> None:
        super().__init__(parent)
        self.width = 600
        self.height = 300
        self.setWindowTitle("Robotic HID DFU")
        self.setGeometry(900, 1000, self.width, self.height)
        self.move(900, 1000)

        self.title = QLabel("<h1>Robotic HID Device</h1>", parent=self)
        self.title2 = QLabel("<h1>Firmware Update GUI</h1>", parent=self)
        self.title.move(60, 15)
        self.title2.move(60, 65)

        self.status = QLabel("Waiting to hit upload", parent=self)
        self.status.move(60, 200)

        self.__create_combo_box()
        self.__create_file_path_input()
        self.__create_button()
        self.dfu_host = dfu_host()

    
    def __create_combo_box(self):
        #combo box tutorial from: https://www.tutorialspoint.com/pyqt/pyqt_qcombobox_widget.htm
        self.cb_label = QLabel('Select a serial port', parent=self)
        self.cb_label.move(60, 120)
        self.cb = QComboBox(self)
        serial_ports = self.__get_serial_ports()
        self.cb.addItems(serial_ports)
        self.cb.move(60, 150)
        pass

    def cb_selection_change(self):
        print(self.cb.currentText())
        pass

    def __get_serial_ports(self) -> list:
        port_list_str = []
        ports = serial.tools.list_ports.comports()
        for port, desc, hwid in sorted(ports):
            port_list_str.append(str(port))
        
        return port_list_str

    def __create_file_path_input(self):
        self.line_edit = QLineEdit(self)
        self.line_edit_label = QLabel('Enter filepath to binary', parent=self)
        self.line_edit_label.move(300, 120)
        self.line_edit.move(300, 150)
        self.line_edit_text = ""
        self.line_edit.textChanged.connect(self.line_edit_changed)

    def line_edit_changed(self, text):
        self.line_edit_text = text

    def __create_button(self):
        self.button = QPushButton('Upload', self)
        self.button.move(300, 210)
        self.button.clicked.connect(self.button_clicked)

    def button_clicked(self):
        self.status.setText("Uploading...")
        self.dfu_host.open(self.cb.currentText())
        try:
            self.dfu_host.upload_file(self.line_edit_text.strip("\""))
        except Exception as e:
            self.status.setText(str(e))
            return
        self.status.setText("Upload complete")
        pass
        


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = Window()
    window.show()

    # window = QWidget()
    # window.setWindowTitle('Bootloader')
    # window.setGeometry(900, 1000, 1200, 600)
    # window.move(60, 15)
    # title = QLabel('<h1>Robotic HID Device Firmware Update GUI</h1>', parent=window)
    # title.move(60, 15)

    # instructions = QLabel('Paste the location of the binary below', parent=window)
    # instructions.move(60, 90)



    
    window.show()
    
    sys.exit(app.exec_())