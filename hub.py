from flask import Flask, request, jsonify
from google.cloud import storage
import time
import ssl
import os

service_account_file = "document-logger-9762675d2c87.json"
client = storage.Client.from_service_account_json(service_account_file)
bucket_name = 'tyrelx-bucket'
bucket = client.bucket(bucket_name)
file_index = 1

app = Flask(__name__)

context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
context.load_cert_chain(certfile='server.crt', keyfile='server.key')

def process_data(data):
	global file_index
	sensor_values = {
		"E": float(data.get("sensor_E", 1024)),
		"S": float(data.get("sensor_S", 1024)),
		"W": float(data.get("sensor_W", 1024)),
		"N": float(data.get("sensor_N", 1024)),
	}

	# Valorile senzorilor sunt între 1024 și 0 
	# Prag pentru detectarea câmpului magnetic (< 100)
	problems = {key: 1 if val < 200 else 0 for key, val in sensor_values.items()}
	alert = sum(problems.values()) >= 2

	file_name = f"sensor_data_{file_index}.txt"
	file_index += 1

	with open(file_name, "w") as f:
		f.write(str({"timestamp": time.strftime('%H:%M:%S'), "values": sensor_values, "alert": alert}))

	blob = bucket.blob(file_name)
	blob.upload_from_filename(file_name)
	
	os.remove(file_name)

	return alert

@app.route("/data", methods=["POST"])
def receive_data():
	data = request.get_json()

	if data:
		alert = process_data(data)
		if not alert:
			return jsonify({"status": "success", "message": "Datele au fost primite"}), 200
		else:
			return jsonify({"status": "alert", "message": "Camp magnetic detectat"}), 200
	else:
		return jsonify({"status": "error", "message": "Date invalide"}), 400

if __name__ == "__main__":
	app.run(host="0.0.0.0", port=5000, debug=True, ssl_context=context)
