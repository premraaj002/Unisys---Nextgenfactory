import asyncio
import websockets
import json
from azure.identity import DefaultAzureCredential
from azure.digitaltwins.core import DigitalTwinsClient
from datetime import datetime
import sqlite3
import numpy as np
from sklearn.ensemble import IsolationForest
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class ConveyorBeltWebSocketServer:
    def __init__(self):
        self.adt_url = "AZURE HOST LINK" # Due confiditionly we does not added the link here
        self.credential = DefaultAzureCredential()
        self.dt_client = DigitalTwinsClient(self.adt_url, self.credential)
        self.twin_id = "ConveyorBeltTwin1"
        
        # Initialize database
        self.init_database()
        
        # Simple predictive model (Isolation Forest for anomaly detection)
        self.model = IsolationForest(contamination=0.05)
        self.model_fitted = False
        self.training_data = []
        self.keep_running = True

    def init_database(self):
        self.conn = sqlite3.connect('conveyor_data.db', detect_types=sqlite3.PARSE_DECLTYPES)
        self.conn.execute('PRAGMA journal_mode=WAL')  # Better concurrency
        c = self.conn.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS conveyor_metrics
                     (timestamp TIMESTAMP, travel_time REAL, vibration_warning INTEGER, status TEXT)''')
        self.conn.commit()

    def save_metrics(self, travel_time, vibration_warning, status):
        try:
            c = self.conn.cursor()
            c.execute("INSERT INTO conveyor_metrics VALUES (?, ?, ?, ?)",
                     (datetime.now().timestamp(), travel_time, int(vibration_warning), status))
            self.conn.commit()
        except sqlite3.Error as e:
            logger.error(f"Database error: {e}")
            self.conn.rollback()

    def get_historical_data(self, hours=24):
        try:
            c = self.conn.cursor()
            c.execute("""SELECT datetime(timestamp, 'unixepoch'), travel_time, vibration_warning, status 
                         FROM conveyor_metrics 
                         WHERE timestamp >= ?""",
                     (datetime.now().timestamp() - hours * 3600,))
            return c.fetchall()
        except sqlite3.Error as e:
            logger.error(f"Database error: {e}")
            return []

    def predict_failure(self, travel_time, vibration_warning):
        try:
            features = np.array([[travel_time, vibration_warning]])
            if not self.model_fitted and len(self.training_data) >= 50:
                self.model.fit(self.training_data)
                self.model_fitted = True
                logger.info("Model training completed")
            if len(self.training_data) < 50:
                self.training_data.append([travel_time, vibration_warning])
                return False
            prediction = self.model.predict(features)
            return prediction[0] == -1  # -1 indicates anomaly
        except Exception as e:
            logger.error(f"Prediction error: {e}")
            return False

    async def handle_conveyor_belt(self, websocket):
        try:
            async for message in websocket:
                try:
                    data = json.loads(message)
                    action = data.get("action")

                    if action == "get_status":
                        twin = self.dt_client.get_digital_twin(self.twin_id)
                        response = {
                            "status": twin.get("status", "unknown"),
                            "travelTime": twin.get("travelTime", 0),
                            "vibrationWarning": twin.get("vibrationWarning", False)
                        }
                        await websocket.send(json.dumps(response))

                    elif action == "set_status":
                        patch = [{"op": "add", "path": "/status", "value": data["value"]}]
                        self.dt_client.update_digital_twin(self.twin_id, patch)
                        twin = self.dt_client.get_digital_twin(self.twin_id)
                        response = {
                            "status": twin.get("status", "unknown"),
                            "travelTime": twin.get("travelTime", 0),
                            "vibrationWarning": twin.get("vibrationWarning", False)
                        }
                        await websocket.send(json.dumps(response))

                    elif action == "set_travel_time":
                        travel_time = data["value"]
                        patch = [{"op": "add", "path": "/travelTime", "value": travel_time}]
                        self.dt_client.update_digital_twin(self.twin_id, patch)
                        
                        vibration_warning = data.get("vibration_warning", False)
                        failure_predicted = self.predict_failure(travel_time, vibration_warning)
                        
                        self.save_metrics(travel_time, vibration_warning, "running")
                        
                        response = {
                            "travelTime": travel_time,
                            "vibrationWarning": vibration_warning,
                            "failure_predicted": failure_predicted,
                            "status": "running"
                        }
                        await websocket.send(json.dumps(response))

                    elif action == "set_vibration_warning":
                        vibration_warning = data["value"]
                        patch = [{"op": "add", "path": "/vibrationWarning", "value": vibration_warning}]
                        self.dt_client.update_digital_twin(self.twin_id, patch)
                        twin = self.dt_client.get_digital_twin(self.twin_id)
                        response = {
                            "status": twin.get("status", "unknown"),
                            "vibrationWarning": vibration_warning,
                            "travelTime": twin.get("travelTime", 0)
                        }
                        await websocket.send(json.dumps(response))

                    elif action == "get_history":
                        hours = data.get("hours", 24)
                        history = self.get_historical_data(hours)
                        await websocket.send(json.dumps({
                            "action": "history_data",
                            "data": history
                        }))

                    else:
                        await websocket.send(json.dumps({"error": "Invalid action"}))

                except json.JSONDecodeError as e:
                    await websocket.send(json.dumps({"error": f"Invalid JSON: {str(e)}"}))
                except Exception as e:
                    logger.error(f"Error handling message: {e}")
                    await websocket.send(json.dumps({"error": str(e)}))

        except websockets.exceptions.ConnectionClosed as e:
            logger.info(f"Connection closed: {e}")
        except Exception as e:
            logger.error(f"Unexpected error in handler: {e}")
        finally:
            logger.info("Client disconnected")

    async def start_server(self, host="0.0.0.0", port=8765):
        async with websockets.serve(
            self.handle_conveyor_belt,
            host,
            port,
            ping_interval=20,
            ping_timeout=30,
            close_timeout=10
        ) as server:
            logger.info(f"Conveyor Belt WebSocket server running at ws://{host}:{port}")
            while self.keep_running:
                await asyncio.sleep(1)

    def shutdown(self):
        self.keep_running = False
        self.conn.close()

if __name__ == "__main__":
    server = ConveyorBeltWebSocketServer()
    try:
        asyncio.run(server.start_server())
    except KeyboardInterrupt:
        logger.info("Shutting down server...")
        server.shutdown()
    except Exception as e:
        logger.error(f"Server error: {e}")
        server.shutdown()
