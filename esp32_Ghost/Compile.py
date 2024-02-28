import os
import subprocess
from pathlib import Path

# Function to detect connected boards and return their FQBN and port
def detect_board():
    result = subprocess.run(["arduino-cli", "board", "list"], capture_output=True, text=True)
    lines = result.stdout.splitlines()
    for line in lines[1:]:  # Skip the header line
        parts = line.split()
        if "Serial Port" in line and len(parts) >= 6:
            port = parts[0]
            fqbn = parts[-1]
            return port, fqbn
    return None, None

# Board information: Add or modify entries as needed
boards = [
    {"name": "Deneyap Mini", "fqbn": "esp32:esp32:deneyapmini"},
    {"name": "Adafruit Feather ESP32-S2", "fqbn": "esp32:esp32:adafruit_feather_esp32s2"},
    # Add more boards here
]

# Get the username of the current user
username = os.getlogin()

# Construct the path to the Arduino libraries folder
libraries_dir = Path(f"C:/Users/{username}/Documents/Arduino/libraries")

# Specify your project directory here
project_dir = Path("D:/UE Installations/GhostESP/esp32_Ghost")  # Adjust this to your project directory

# Detect the board
port, detected_fqbn = detect_board()
if port and detected_fqbn:
    print(f"Detected board at {port} with FQBN {detected_fqbn}")
    # Find the matching board in our boards array
    selected_board = next((board for board in boards if board["fqbn"] == detected_fqbn), None)
    if not selected_board:
        # If a specific board wasn't found, try to match by the base FQBN (before the colon if applicable)
        base_fqbn = detected_fqbn.split(':')[0] + ':esp32'
        selected_board = next((board for board in boards if board["fqbn"].startswith(base_fqbn)), None)
    if selected_board:
        fqbn = selected_board["fqbn"]
    else:
        print(f"No matching board configuration found for {detected_fqbn}.")
        exit(1)
else:
    print("No board detected. Please connect a board.")
    exit(1)

# Construct the compile command
compile_command = f'arduino-cli compile --fqbn {fqbn} --libraries "{libraries_dir}" "{project_dir}"'
print("Compile command:", compile_command)

# Execute the compile command
try:
    subprocess.run(compile_command, check=True, shell=True, text=True)
    print("Compilation successful")
except subprocess.CalledProcessError as e:
    print(f"Compilation failed: {e}")
    exit(1)