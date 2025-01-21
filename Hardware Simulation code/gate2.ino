#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Student-PI"
#define WIFI_PASSWORD "student2023"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAxY_fxzCxo-cSZHCOj7pLvJS8U1CO2gxo"
// Insert RTDB URL
#define DATABASE_URL "https://bovine-health-default-rtdb.europe-west1.firebasedatabase.app/"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Timing
unsigned long sendDataPrevMillis = 0;
const unsigned long interval = 15000; // e.g., every 15 seconds
bool signupOK = false;

// Variables to read from Firebase
String motionStatus = "";
float temperature = 0.0;
float weight = 0.0;
float ID = 0.0;

// “sick” logic. If you're also storing `sick` in Firebase,
// you can read it the same way you read ID, temperature, etc.
int sick = 1; // Hardcoded for demonstration

// LED pin (adjust to your board)
const int LED_PIN = 2;

void setup()
{
  Serial.begin(115200);

  // Wi-Fi setup
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Firebase initialization
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Sign up (anonymous)
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Firebase sign-up successful.");
    signupOK = true;
  }
  else
  {
    Serial.printf("Firebase sign-up failed: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback; // Monitor token status
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop()
{
  // Only run the Firebase read/write steps every `interval` milliseconds
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > interval || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // 1. READ ID
    if (Firebase.RTDB.getFloat(&fbdo, "/motion/ID"))
    {
      if (fbdo.dataType() == "float")
      {
        ID = fbdo.floatData();
        Serial.print("Retrieved ID: ");
        Serial.println(ID);
      }
    }
    else
    {
      Serial.print("Failed to get ID: ");
      Serial.println(fbdo.errorReason());
    }

    // 2. READ temperature
    if (Firebase.RTDB.getFloat(&fbdo, "/motion/temperature"))
    {
      if (fbdo.dataType() == "float")
      {
        temperature = fbdo.floatData();
        Serial.print("Retrieved Temperature: ");
        Serial.println(temperature);
      }
    }
    else
    {
      Serial.print("Failed to get temperature: ");
      Serial.println(fbdo.errorReason());
    }

    // 3. READ weight
    if (Firebase.RTDB.getFloat(&fbdo, "/motion/weight"))
    {
      if (fbdo.dataType() == "float")
      {
        weight = fbdo.floatData();
        Serial.print("Retrieved Weight: ");
        Serial.println(weight);
      }
    }
    else
    {
      Serial.print("Failed to get weight: ");
      Serial.println(fbdo.errorReason());
    }

    // 4. READ motionStatus (string)
    if (Firebase.RTDB.getString(&fbdo, "/motion/status"))
    {
      if (fbdo.dataType() == "string")
      {
        motionStatus = fbdo.stringData();
        Serial.print("Retrieved motion status: ");
        Serial.println(motionStatus);
      }
    }
    else
    {
      Serial.print("Failed to get motion status: ");
      Serial.println(fbdo.errorReason());
    }

    // 5. Check if cow is sick
    //    Condition: temperature > 39.5 AND sick == 1
    bool isSick = (temperature > 39.5 || sick == 1);

    // 6. Control LED based on isSick
    if (isSick)
    {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("ALERT: Cow is SICK, LED ON");
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
      Serial.println("Cow is not sick, LED OFF");
    }

    // 7. Print a summary to Serial Monitor
    Serial.println("---- Current Cow Data ----");
    Serial.print("ID: ");
    Serial.println(ID);
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.print("Weight: ");
    Serial.println(weight);
    Serial.print("Motion Status: ");
    Serial.println(motionStatus);
    Serial.print("Health Status: ");
    Serial.println(isSick ? "SICK" : "HEALTHY");
    Serial.println("--------------------------\n");

    // 8. WRITE a summary to Firebase under "/summary"
    //    Save all relevant fields
    if (Firebase.RTDB.setFloat(&fbdo, "/summary/ID", ID))
    {
      Serial.println("PASSED: ID updated in /summary");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "/summary/temperature", temperature))
    {
      Serial.println("PASSED: temperature updated in /summary");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "/summary/weight", weight))
    {
      Serial.println("PASSED: weight updated in /summary");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    if (Firebase.RTDB.setString(&fbdo, "/summary/motionStatus", motionStatus))
    {
      Serial.println("PASSED: motionStatus updated in /summary");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    String healthStatus = isSick ? "SICK" : "HEALTHY";
    if (Firebase.RTDB.setString(&fbdo, "/summary/healthStatus", healthStatus))
    {
      Serial.println("PASSED: healthStatus updated in /summary");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    // If you also want to store the raw "sick" int (1 or 0):
    if (Firebase.RTDB.setInt(&fbdo, "/summary/sick", sick))
    {
      Serial.println("PASSED: sick updated in /summary");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    // Next loop iteration in ~15 seconds
  }
}
