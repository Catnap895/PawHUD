from flask import Flask, request
import csv
from datetime import datetime

app = Flask(__name__)

# CSV file to log Arduino data
CSV_FILE = "arduino_data_log.csv"

# Ensure CSV has headers
try:
    with open(CSV_FILE, "x", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["Timestamp", "Data"])
except FileExistsError:
    pass  # File already exists

@app.route("/", methods=["GET", "POST"])
def receive():
    if request.method == "POST":
        data = request.data.decode("utf-8")  # Read message from Arduino
        print(f"[{datetime.now()}] Arduino says: {data}")

        # Log to CSV
        with open(CSV_FILE, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow([datetime.now(), data])

        return "OK", 200

    else:
        return "Server is running. Use POST to send Arduino data.", 200

if __name__ == "__main__":
    print("Flask Arduino server running on http://0.0.0.0:5000")
    app.run(host="0.0.0.0", port=5000)