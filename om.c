#include <WiFi.h>
#include <HTTPClient.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// WiFi credentials
const char* ssid = "Omomomom";
const char* password = "11111111";

// Dhwani API details
const char* serverUrl = "https://dwani-dwani-api.hf.space/api/endpoint"; 
const char* apiKey = "firepanda2669@gmail.com_dwani"; 

// GPS setup
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);  

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); 

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected");
}

void loop() {
  // Read GPS data
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (!gps.location.isValid()) {
    Serial.println("⏳ Waiting for valid GPS fix...");
    delay(2000);
    return; 
  }

  Serial.println("✅ GPS fix acquired");
  float lat = gps.location.lat();
  float lng = gps.location.lng();
  Serial.printf("📍 Latitude: %.6f, Longitude: %.6f\n", lat, lng);

  
  uint8_t dummyAudio[] = {0x01, 0x02, 0x03}; 
  size_t audioLength = sizeof(dummyAudio);

  String transcription = sendToDhwaniAPI(dummyAudio, audioLength);

  if (transcription.length() > 0) {
    Serial.print("📝 Transcription: ");
    Serial.println(transcription);

    if (transcription.indexOf("activate") != -1 || transcription.indexOf("help") != -1) {
      Serial.println("🎤 Trigger keyword detected!");
      Serial.printf("📍 Sending GPS: https://www.google.com/maps?q=%.6f,%.6f\n", lat, lng);
      // Here, you can add code to send SMS, upload to cloud, etc.
    }
  } else {
    Serial.println("⚠ No transcription received");
  }

  delay(15000); // Wait before next capture
}

String sendToDhwaniAPI(uint8_t* audioData, size_t length) {
  HTTPClient http;
  http.begin(serverUrl);
  http.setTimeout(10000);
  http.addHeader("Content-Type", "application/octet-stream");
  http.addHeader("X-API-Key", apiKey);

  int httpResponseCode = http.POST(audioData, length);
  if (httpResponseCode > 0) {
    String response = http.getString();
    http.end();
    return response;  // Assuming API returns plain text transcription
  } else {
    Serial.printf("❌ HTTP POST failed: %d\n", httpResponseCode);
    http.end();
    return "";
  }
}