import sys
from PySide6.QtWidgets import QApplication, QMainWindow, QLabel, QLineEdit, QPushButton, QTextEdit
import requests
import json
import psutil

DASHBOARD = "http://dashboard.com"
SENSORS = "http://sensors.com"

token = ""
user = ""

def register(username, password):
    global user
    user = username
    response = requests.post(f"{DASHBOARD}/createuser", json={"username": username, "password": password})
    if response.status_code == 200:
        response_text.clear()
        response_text.append("Sign Up successful\n")
    else:
        response_text.clear()
        response_text.append("Sign Up failed " + str(response.status_code) + "\n")

def login(username, password):
    global token
    global user
    response = requests.post(f"{DASHBOARD}/login", json={"username": username, "password": password})
    if response.status_code == 200:
        user = username
        token = response.json().get("token")
        response_text.clear()
        response_text.append("Login successful\n")
    else:
        token = ""
        user = ""
        response_text.clear()
        response_text.append("Login failed " + str(response.status_code) + "\n")

def listAll():
    global token
    headers = {
        "Authorization": "Bearer " + token
    }

    response = requests.get(f"{DASHBOARD}/logged/listall", headers=headers)
    if response.status_code == 200:
        data = response.json()
        response_text.clear()
        response_text.append(json.dumps(data, indent=4))
    else:
        response_text.clear()
        response_text.append("User list failed " + str(response.status_code) + "\n")

def submit():
    global user
    global token
    headers = {
        "Authorization": "Bearer " + token
    }

    memoryStat = psutil.virtual_memory()

    totalMemory = (memoryStat.total / (1024 * 1024))
    freeMemory = (memoryStat.available / (1024 * 1024))
    usedMemory = (memoryStat.used / (1024 * 1024))

    response = requests.post(f"{SENSORS}/logged/submit", json={"username": user, "totalmemory": totalMemory,
                                                                "freememory": freeMemory, "usedmemory": usedMemory},
                             headers=headers)
    if response.status_code == 200:
        response_text.clear()
        response_text.append("Information submit successful\n")
    else:
        response_text.clear()
        response_text.append("Submit failed " + str(response.status_code) + "\n")

def summary():
    response = requests.get(f"{SENSORS}/summary")
    if response.status_code == 200:
        data = response.json()
        response_text.clear()
        response_text.append(json.dumps(data, indent=4))
    else:
        response_text.clear()
        response_text.append("Summary failed\n")

def handleLogin():
    username = username_entry.text()
    password = password_entry.text()
    login(username, password)

def handleRegister():
    username = username_entry.text()
    password = password_entry.text()
    register(username, password)

def handleListAll():
    listAll()

def handleSubmit():
    submit()

def handleSummary():
    summary()

app = QApplication(sys.argv)

window = QMainWindow()
window.setWindowTitle("Client")
window.setGeometry(100, 100, 700, 600)

# Username
username_label = QLabel(window)
username_label.setText("Username:")
username_label.setGeometry(10, 10, 80, 30)

username_entry = QLineEdit(window)
username_entry.setGeometry(100, 10, 150, 30)
                           
# Password
password_label = QLabel(window)
password_label.setText("Contraseña:")
password_label.setGeometry(10, 40, 80, 30)

password_entry = QLineEdit(window)
password_entry.setGeometry(100, 40, 150, 30)
password_entry.setEchoMode(QLineEdit.Password)

# Responses
response_text = QTextEdit(window)
response_text.setGeometry(25, 120, 650, 400)

# Buttons
login_button = QPushButton(window)
login_button.setText("Login")
login_button.setGeometry(25, 80, 80, 30)
login_button.clicked.connect(handleLogin)

register_button = QPushButton(window)
register_button.setText("Sign up")
register_button.setGeometry(190, 80, 80, 30)
register_button.clicked.connect(handleRegister)

list_button = QPushButton(window)
list_button.setText("Users list")
list_button.setGeometry(25, 550, 80, 30)
list_button.clicked.connect(handleListAll)

upload_button = QPushButton(window)
upload_button.setText("Submit info")
upload_button.setGeometry(290, 550, 100, 30)
upload_button.clicked.connect(handleSubmit)

download_button = QPushButton(window)
download_button.setText("Summary info")
download_button.setGeometry(550, 550, 100, 30)
download_button.clicked.connect(handleSummary)

window.show()
sys.exit(app.exec())
