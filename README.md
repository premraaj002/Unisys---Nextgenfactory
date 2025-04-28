# Unisys - Next Gen Factory

This project simulates a smart factory using a digital twin approach. It includes sensor data acquisition, WebSocket-based backend communication, and an animated 3D dashboard visualization.

# Features
- 3D Model Dashboard to monitor factory operations.

- Repair Robot controlled via Arduino code.

- Oil Dispenser Monitoring with gas leakage detection.

- Conveyor Belt Sensors for real-time data acquisition.

- WebSocket Backend for real-time communication.

- SQLite Database for storing and retrieving historical data.

# Getting Started 
Requirements :

- Arduino IDE
- Python 3.8+
- Libraries: websockets, sqlite3, Isolation Forest model
- Any 3D-compatible browser (Chrome recommended)

Steps to Run :
- Upload Arduino Codes (.ino files to their respective hardware (conveyor sensors, oil dispenser, robot)).
- Run this Command in Terminal : "pip install -r requirements.txt"
- Start WebSocket Backend - python app.py
- Open Dashboard - Open 3dmodel.html or 3dmodel_animation.html in a web browser.
