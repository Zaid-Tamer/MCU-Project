#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// Wi-Fi credentials
const char* ssid = "taher";          // Replace with your Wi-Fi SSID
const char* password = "Ta113*@#1984!&  ";  // Replace with your Wi-Fi password

// CallMeBot credentials for WhatsApp
const char* phoneNumber = "201122139407";   // Replace with your phone number (omit + sign)
const char* apiKey = "5361920";             // Replace with your CallMeBot API key
const char* message = "watch out, There's Someone Here";   // WhatsApp message text

// PIR Sensor and Buzzer pins
#define PIR_PIN D1    // GPIO pin for PIR sensor
#define BUZZER_PIN D2 // GPIO pin for buzzer

WiFiClientSecure wifiClient; // Use WiFiClientSecure for HTTPS

// Function to URL-encode the message
String urlencode(String str) {
  String encoded = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (isalnum(c)) {
      encoded += c;
    } else {
      encoded += "%" + String(c, HEX);
    }
  }
  return encoded;
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");
  Serial.println("Device IP Address: ");
  Serial.println(WiFi.localIP());

  // Allow insecure HTTPS (not recommended for production but simplifies setup)
  wifiClient.setInsecure();
  wifiClient.setTimeout(15000); // Set timeout for HTTPS connection
}

void sendWhatsAppMessage() {
  if (WiFi.status() == WL_CONNECTED) { // Check if connected to Wi-Fi
    WiFiClientSecure httpsClient; // Declare a WiFiClientSecure object for HTTPS
    httpsClient.setInsecure();
    httpsClient.setTimeout(15000); // Increase timeout for connection

    HTTPClient https; // Declare the HTTPClient object
    String encodedMessage = urlencode(message);
    String url = "https://api.callmebot.com/whatsapp.php?phone=" + String(phoneNumber) + "&text=" + encodedMessage + "&apikey=" + String(apiKey);

    Serial.println("Connecting to CallMeBot...");
    Serial.println("URL: " + url);

    if (https.begin(httpsClient, url)) { // Initialize the connection with HTTPS
      int httpCode = https.GET(); // Make the GET request

      if (httpCode > 0) {
        // HTTP request was successful
        Serial.println("WhatsApp message sent successfully.");
        Serial.println("Response code: " + String(httpCode));
        String payload = https.getString();
        Serial.println("Response payload: " + payload);
      } else {
        // HTTP request failed
        Serial.println("Error in sending WhatsApp message.");
        Serial.println("HTTP error code: " + String(httpCode));
      }
      https.end(); // Close the connection
    } else {
      Serial.println("HTTPS begin failed. Check URL or connection.");
    }
  } else {
    Serial.println("Wi-Fi not connected. Cannot send message.");
  }
}

void loop() {
  int motionDetected = digitalRead(PIR_PIN);

  if (motionDetected) {
    Serial.println("Motion detected!");

    // Activate the buzzer
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000); // Buzzer ON for 1 second
    digitalWrite(BUZZER_PIN, LOW);

    // Send WhatsApp alert
    sendWhatsAppMessage();

    delay(1000); // Delay to prevent spamming messages
  } else {
    Serial.println("No motion detected.");
  }

  delay(50); // Short delay for stability
}
