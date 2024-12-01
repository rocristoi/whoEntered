#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <Base64.h> 

const char* ssid = ""; // Your network's SSID
const char* password = ""; // Your network's password
const char* serverUrl = "http://192.168.100.204:3000/data"; // Adjust to your server!

const int relay = D1;

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
  SoftwareSerial mySerial(D2, D3);  // Pin #2 is IN, Pin #3 is OUT
#else
  #define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

int getFingerprintIDez();
uint8_t id = 1;  // Keeping the ID fixed for the same user
int retryLimit = 3;  // Max retries for operations

String fingerprintTemplateBase64 = "";  // Variable to hold the Base64 encoded fingerprint template

void setup() {
  while (!Serial);
  Serial.begin(9600);
  Serial.println("Fingerprint template extractor");
  pinMode(relay, OUTPUT);

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);  // Halt if sensor not found
  }

  // Set up Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");

  // Loop indefinitely, handling multiple users
  while (true) {
    if (getFingerprintEnroll()) {
      downloadFingerprintTemplate(id);
      deleteFingerprint(id);  // Delete the fingerprint after download
    }
    delay(5000);  // Wait for 5 seconds before enrolling the next user
  }
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);

  // Continuously check for a valid (non-blank) image
  while (true) {
    p = finger.getImage();
    if (p == FINGERPRINT_OK) {
      Serial.println("Image taken");
      break;  // Valid fingerprint image detected, break the loop
    } else if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");
      delay(500);  // Wait for half a second before checking again
    } else {
      Serial.println("Error while capturing image");
      return false;
    }
  }

  // Convert the image to a template
  p = finger.image2Tz(1);  // Store the first image template
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to convert image to template");
    return false;
  }

  Serial.println("Remove finger");
  delay(2000); // Wait for the user to remove their finger

  // Continuously wait until the finger is removed
  while (true) {
    p = finger.getImage();  // Wait for finger removal
    if (p == FINGERPRINT_NOFINGER) {
      break;  // Finger removed successfully
    } else {
      delay(500);
    }
  }

  // Wait for the same finger again
  Serial.println("Place same finger again");
  while (true) {
    p = finger.getImage();
    if (p == FINGERPRINT_OK) {
      Serial.println("Image taken");
      break;  // Valid fingerprint image detected, break the loop
    } else if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");
      delay(500);  // Wait for half a second before checking again
    } else {
      Serial.println("Error while capturing second image");
      return false;
    }
  }

  // Convert the second image to template
  p = finger.image2Tz(2);  // Store the second image template
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to convert second image to template");
    return false;
  }

  // Create the model and store it
  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to create fingerprint model");
    return false;
  }

  p = finger.storeModel(id);  // Store the model at the given ID
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to store model");
    return false;
  }

  Serial.print("Fingerprint enrolled as ID "); Serial.println(id);
  digitalWrite(relay, HIGH);
  delay(5000);
  digitalWrite(relay, LOW);
  return true;  // Enrollment successful
}
void sendFingerprintTemplate(String fingerprintTemplateBase64) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;  // Create a WiFiClient instance


    // Begin HTTP request
    http.begin( client, serverUrl);  // serverUrl is already defined
    http.addHeader("Content-Type", "application/json");

    // Prepare the JSON payload
    String payload = "{\"fingerprint_template\":\"" + fingerprintTemplateBase64 + "\"}";

    // Send HTTP POST request
    int httpResponseCode = http.POST(payload);

    // Check response
    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error on sending POST request: ");
      Serial.println(httpResponseCode);
    }

    // Close HTTP connection
    http.end();
  } else {
    Serial.println("WiFi not connected, cannot send fingerprint data");
  }
}

void downloadFingerprintTemplate(uint8_t id) {
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #"); Serial.println(id);
  uint8_t p = finger.loadModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.print("Template "); Serial.print(id); Serial.println(" loaded");
  } else {
    Serial.println("Failed to load template");
    return;
  }

  Serial.print("Attempting to get #"); Serial.println(id);
  p = finger.getModel();
  if (p == FINGERPRINT_OK) {
    Serial.print("Template "); Serial.print(id); Serial.println(" transferring:");
  } else {
    Serial.println("Failed to get model");
    return;
  }

  uint8_t bytesReceived[534];  // 2 data packets
  memset(bytesReceived, 0xff, 534);

  uint32_t starttime = millis();
  int i = 0;
  while (i < 534 && (millis() - starttime) < 20000) {
    if (mySerial.available()) {
      bytesReceived[i++] = mySerial.read();
    }
  }
  Serial.print(i); Serial.println(" bytes read.");
  Serial.println("Decoding packet...");

  uint8_t fingerTemplate[512];  // The real template
  memset(fingerTemplate, 0xff, 512);

  // Filter and store the data packets
  int uindx = 9, index = 0;
  memcpy(fingerTemplate + index, bytesReceived + uindx, 256);   // first 256 bytes
  uindx += 256;  // Skip data
  uindx += 2;    // Skip checksum
  uindx += 9;    // Skip next header
  index += 256;  // Advance pointer
  memcpy(fingerTemplate + index, bytesReceived + uindx, 256);   // second 256 bytes

  // Convert the fingerprint template to a Base64 string
  fingerprintTemplateBase64 = base64::encode(fingerTemplate, 512);

  // Print out the fingerprint template in hex
  for (int i = 0; i < 512; ++i) {
    printHex(fingerTemplate[i], 2);
  }
  Serial.println("\ndone.");
  sendFingerprintTemplate(fingerprintTemplateBase64);

}

void deleteFingerprint(uint8_t id) {
  Serial.print("Attempting to delete template #"); Serial.println(id);
  uint8_t p = finger.deleteModel(id);  // Delete the fingerprint model
  if (p == FINGERPRINT_OK) {
    Serial.print("Template #"); Serial.print(id); Serial.println(" deleted");
  } else {
    Serial.println("Failed to delete template");
  }
}

void printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}

void loop() {
}
