from flask import Flask, request, jsonify, render_template
import time
from collections import deque

app = Flask(__name__)

sensor_data_history = deque(maxlen=50)

def process_data(data):
    sensor_values = {
        "E": float(data.get("sensor_E", 1024)),
        "S": float(data.get("sensor_S", 1024)),
        "W": float(data.get("sensor_W", 1024)),
        "N": float(data.get("sensor_N", 1024)),
    }

    # Valorile senzorilor sunt între 1024 și 0 
    # Prag pentru detectarea câmpului magnetic (< 100)
    problems = {key: 1 if val < 100 else 0 for key, val in sensor_values.items()}
    alert = sum(problems.values()) >= 2

    sensor_data_history.append({"timestamp": time.strftime('%H:%M:%S'), "values": sensor_values, "alert": alert})

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/data", methods=["POST"])
def receive_data():
    data = request.get_json()

    if data:
        process_data(data)
        return jsonify({"status": "success", "message": "Datele au fost primite"}), 200
    else:
        return jsonify({"status": "error", "message": "Date invalide"}), 400

@app.route("/latest", methods=["GET"])
def latest_data():
    return jsonify(list(sensor_data_history))

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
