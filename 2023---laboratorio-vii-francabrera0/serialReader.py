import sys
import serial
import time
from PySide6.QtWidgets import QApplication, QMainWindow, QTableWidget, QTableWidgetItem, QGridLayout, QWidget, QLabel, QLineEdit, QPushButton
from PySide6.QtCore import QThread, Signal
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas

class SerialReaderThread(QThread):
    read = Signal(str)
    stop_thread = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        self.serial_port = serial.Serial('/dev/ttyACM0', 9600)

    def run(self):
        while True:
            if self.isInterruptionRequested():
                break
            line = self.serial_port.readline().decode().strip()
            self.read.emit(line)

    def stop(self):
        self.requestInterruption()
        self.wait()


class SerialTableApp(QMainWindow):
    def __init__(self):
        super().__init__()

        self.taskStatusDict = {}

        # TOP
        self.top = QTableWidget()
        self.top.setColumnCount(4)
        self.top.setHorizontalHeaderLabels(["Name", "Stack Free", "Ticks", "CPU %"])
        self.top.setColumnWidth(0, 200)
        self.top.setColumnWidth(1, 140)
        self.top.setColumnWidth(2, 140)
        self.top.setColumnWidth(3, 140)
        self.top.setMinimumHeight(300)
        
        # Temperature
        self.temperatureTable = QTableWidget()
        self.temperatureTable.setColumnCount(1)
        self.temperatureTable.setHorizontalHeaderLabels(["Temperature (C)"])
        self.temperatureTable.setColumnWidth(0, 670)

        # Matplotlib figure and canvas
        self.fig, self.ax = plt.subplots()
        self.canvas = FigureCanvas(self.fig)        

        #filter
        self.filter_label = QLabel("Filter: 8")
        self.filter_label.setStyleSheet("font-size: 50px;")

        self.text_input = QLineEdit()
        self.text_input.setMaximumWidth(500)
        self.send_button = QPushButton("Send")
        self.send_button.setMaximumWidth(200)
        self.send_button.clicked.connect(self.sendData)

        layout = QGridLayout()
        layout.addWidget(self.top, 0, 0, 2, 2)
        layout.addWidget(self.filter_label, 0, 2, 1, 2)
        layout.addWidget(self.text_input, 1, 2, 1, 1)
        layout.addWidget(self.send_button, 1, 3, 1, 1)
        layout.addWidget(self.temperatureTable, 2, 0, 2, 2)
        layout.addWidget(self.canvas, 2, 2, 2, 2)

        mainWidget = QWidget()
        mainWidget.setLayout(layout)
        self.setCentralWidget(mainWidget)

        self.serialReaderThread = SerialReaderThread()
        self.serialReaderThread.read.connect(self.updateData)
        self.serialReaderThread.start()

        self.timestamps = []
        self.temperatures = []
        self.temperatures.extend([0]*20)

        app.aboutToQuit.connect(self.closeApp)

    def updateData(self, data):
        data = data.split(',')

        if len(data) == 4:
            name, stack, ticks, cpu = data
            if cpu == "0":
                cpu = "<1"
            
            if name in self.taskStatusDict:
                row = self.taskStatusDict[name]
                self.top.setItem(row, 1, QTableWidgetItem(stack))
                self.top.setItem(row, 2, QTableWidgetItem(ticks))
                self.top.setItem(row, 3, QTableWidgetItem(cpu))
            else:
                row_position = self.top.rowCount()
                self.top.insertRow(row_position)
                self.top.setItem(row_position, 0, QTableWidgetItem(name))
                self.top.setItem(row_position, 1, QTableWidgetItem(stack))
                self.top.setItem(row_position, 2, QTableWidgetItem(ticks))
                self.top.setItem(row_position, 3, QTableWidgetItem(cpu))

                self.taskStatusDict[name] = row_position

        if data[0].startswith("TEMP:"):
            temperature = data[0].split(":")[1]
            self.updateTemperatureTable(temperature)
        
        elif data[0].startswith("FILTER:"):
            filter_number = data[0].split(":")[1]
            self.updateFilterLabel(filter_number)

    def updateFilterLabel(self, filter_number):
        self.filter_label.setText(f"Filter: {filter_number}")

    def updateTemperatureTable(self, temperature):
        rowPosition = 0
        self.temperatureTable.insertRow(rowPosition)
        self.temperatureTable.setItem(rowPosition, 0, QTableWidgetItem(temperature))

        maxRows = 20
        while self.temperatureTable.rowCount() > maxRows:
            self.temperatureTable.removeRow(maxRows)

        self.timestamps.append(time.time())
        self.temperatures.append(float(temperature))
        self.updateTemperaturePlot()


    def updateTemperaturePlot(self):
        if len(self.timestamps) > 10:
            self.timestamps = self.timestamps[-10:]
            self.temperatures = self.temperatures[-10:]

            self.ax.clear()
            self.ax.set_ylim(10, 40)
            self.ax.plot(self.timestamps, self.temperatures, '-o', label='Temperature')
            self.ax.set_xlabel('Time')
            self.ax.set_ylabel('Temperature (C)')
            self.ax.legend()
            self.canvas.draw()

    def sendData(self):
        text_to_send = self.text_input.text()
        if text_to_send:
            self.serialReaderThread.serial_port.write(text_to_send.encode())

    def closeApp(self):
        self.serialReaderThread.stop()
        self.close()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SerialTableApp()
    window.showMaximized()
    sys.exit(app.exec())
