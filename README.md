# Unisys - Next Gen Factory

This project simulates a smart factory using a digital twin approach. It includes sensor data acquisition, WebSocket-based backend communication, and an animated 3D dashboard visualization.

# Features
- 3D Model Dashboard to monitor factory operations.

- Repair Robot controlled via Arduino code.

- Oil Dispenser Monitoring with gas leakage detection.

- Conveyor Belt Sensors for real-time data acquisition.

- WebSocket Backend for real-time communication.

- SQLite Database for storing and retrieving historical data.

# Getting Started : 
Requirements:

1.Arduino IDE
2.Python 3.8+
3.Libraries: websockets, sqlite3, flask 
4.Any 3D-compatible browser (Chrome recommended)

Steps to Run
1.Upload Arduino Codes (.ino files to their respective hardware (conveyor sensors, oil dispenser, robot)).
2.Run this Command in Terminal - pip install -r requirements.txt
3.Start WebSocket Backend - python app.py
4.Open Dashboard - Open 3dmodel.html or 3dmodel_animation.html in a web browser.
