from flask_login import LoginManager, UserMixin, login_user, login_required, logout_user
from flask import Flask, jsonify, render_template, request, url_for, redirect
from google.cloud import storage
from collections import deque
import hashlib
import secrets
import string
import csv
import ssl
import ast
import os

def generate_secret_key(length=50):
	''' Generates a secret key for login process. '''
	characters = string.ascii_letters + string.digits + string.punctuation
	secret_key = ''.join(secrets.choice(characters) for _ in range(length))
	return secret_key

service_account_file = "document-logger-9762675d2c87.json"
client = storage.Client.from_service_account_json(service_account_file)
bucket_name = 'tyrelx-bucket'
bucket = client.bucket(bucket_name)
file_index = 1

app = Flask(__name__)

credentials = {}
app.secret_key = generate_secret_key()
login_manager = LoginManager()
login_manager.init_app(app)

context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
context.load_cert_chain(certfile='server.crt', keyfile='server.key')

sensor_data_history = deque(maxlen=50)

class User(UserMixin):
	'''Class used to represent the user.'''
	def __init__(self, id):
		self.id = id
	
	@login_manager.user_loader
	def load_user(id):
		'''Loads the user.'''
		return User(id)

def get_data():
	global file_index
	while True:
		src_file_name = f"sensor_data_{file_index}.txt"
		file_name = f"sensor_data_from_cloud{file_index}.txt"

		blob = bucket.blob(src_file_name)

		try:
			blob.download_to_filename(file_name)
		except Exception as e:
			break

		try:
			with open(file_name, "r") as f:
				data = f.read()
				parsed_data = ast.literal_eval(data)
				if isinstance(parsed_data, dict):
					sensor_data_history.append(parsed_data)
		except (ValueError, SyntaxError):
			pass
		finally:
			os.remove(file_name)
		
		file_index += 1

def load_credentials():
	global credentials
	with open('credentials/credentials.csv') as file:
		reader = csv.DictReader(file)
		for row in reader:
			credentials[row['username']] = row['password']

@app.route("/home")
@login_required
def index():
	return render_template("index.html")

@app.route("/latest", methods=["GET"])
@login_required
def latest_data():
	get_data()
	return jsonify(list(sensor_data_history))

@app.route('/', methods=['GET'])
def serve_login():
	return render_template('login.html')

@app.route('/login', methods=['POST'])
def verify_login():
	data = request.json
	username = data.get('username')
	password = hashlib.md5(data.get('password').encode()).hexdigest()
	
	if username in credentials and credentials[username] == password:
		user = User(username)
		login_user(user)
		return jsonify({"success": True})
	else:
		return jsonify({"success": False, "error": "Username sau parola incorecte!"})

@app.route('/logout', methods=['POST'])
@login_required
def logout():
    logout_user()
    return redirect(url_for('serve_login'))

if __name__ == "__main__":
	load_credentials()
	app.run(host="0.0.0.0", port=5001, debug=True, ssl_context=context)
