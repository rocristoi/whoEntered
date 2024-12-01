# whoEntered

**whoEntered** is a smart access tracking system that uses fingerprint authentication to log and display entry data. It consists of three components:

1. **ESP32 Code**: Reads fingerprints, stores them, and triggers a relay to unlock the door.
2. **Backend Server**: Processes and stores fingerprint data received from the ESP32 in a JSON file.
3. **React Frontend**: Displays minutiae points of users who have entered.

## Features
- Fingerprint-based door access.
- JSON-based data storage on the backend.
- Web interface to visualize user fingerprint minutiae points.

## Configuration and Setup

### Hardware Setup
1. Connect the fingerprint module's RX and TX to the **D2** and **D3** ports of the ESP32.
2. Connect the relay input to the **D1** port of the ESP32.

### ESP32 Configuration
1. Configure the **WLAN settings** in the ESP32 code to connect to your Wi-Fi network.
2. Set the **backend server address** in the ESP32 code.

### Backend Server Configuration
1. Ensure the backend server is running and reachable by the ESP32.

### React Frontend Configuration
1. Open `/src/App.jsx` in the React project.
2. Set the **backend server address** on **line 11**.

### Deployment
1. Flash the ESP32 with the provided code.
2. Start the backend server to process and store data.
3. Deploy the React frontend to visualize entry data.

## How It Works
1. A user scans their fingerprint.
2. The ESP32 matches and stores the fingerprint data.
3. The ESP32 triggers the relay to unlock the door.
4. The fingerprint data is sent to the backend server.
5. The React frontend retrieves and displays the minutiae points.

## License
This project is licensed under the [MIT License](LICENSE).

---
Feel free to contribute or report any issues!
