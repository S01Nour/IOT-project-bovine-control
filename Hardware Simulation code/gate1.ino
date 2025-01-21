#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#include <time.h> // For time-related functions on ESP32
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
// For ESP8266, use an NTP library or other method for date/time
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

// Hard-coded values
const int cowID = 98;
const float temperature = 40;
const float weight = 420.5;

// Motion sensor pin
const int PIN_TO_SENSOR = 19; // GPIO19 pin for motion sensor
int pinStateCurrent = LOW;
int pinStatePrevious = LOW;
String motionStatus = "No Motion";

// LED pin
const int LED_PIN = 2; // Adjust if needed

// Timing
unsigned long sendDataPrevMillis = 0;
const unsigned long interval = 15000; // 15 seconds
bool signupOK = false;

// NTP and timezone settings
const char *ntpServer = "pool.ntp.org";
// Adjust offsets as needed for your timezone
const long gmtOffset_sec = 0;     // e.g., 0 for GMT
const int daylightOffset_sec = 0; // e.g., 3600 for daylight saving

// Helper function to initialize time via NTP
void initTime()
{
  // On ESP32, configTime is used to set up the NTP server, GMT offset, etc.
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Getting time from NTP...");

  // Wait until time is set
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2)
  { // Wait for the time to be > Jan 1, 1970
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nTime initialized!");
}

// Helper function to get formatted date-time string
String getFormattedDateTime()
{
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo); // Uses the device's local time settings

  // Create a buffer to store the date-time string
  char timeStringBuff[50];
  // Format: YYYY-MM-DD HH:MM:SS
  strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);

  return String(timeStringBuff);
}

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

  // Initialize time from NTP
  initTime();

  // Firebase initialization
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

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

  // Pin modes
  pinMode(PIN_TO_SENSOR, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Ensure LED is off initially
}

void loop()
{
  // Read motion sensor
  pinStatePrevious = pinStateCurrent;
  pinStateCurrent = digitalRead(PIN_TO_SENSOR);

  // Detect rising edge => Motion detected
  if (pinStatePrevious == LOW && pinStateCurrent == HIGH)
  {
    motionStatus = "Motion detected";
    Serial.println(motionStatus);
    // Turn LED ON
    digitalWrite(LED_PIN, HIGH);
  }
  // Detect falling edge => Motion stopped
  else if (pinStatePrevious == HIGH && pinStateCurrent == LOW)
  {
    motionStatus = "Motion stopped";
    Serial.println(motionStatus);
    // Turn LED OFF
    digitalWrite(LED_PIN, LOW);
  }

  // Send data to Firebase at intervals
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > interval || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // Get formatted date/time
    String dateTime = getFormattedDateTime();
    Serial.println("Current Date/Time: " + dateTime);

    // Write motion status
    if (Firebase.RTDB.setString(&fbdo, "/motion/status", motionStatus))
    {
      Serial.println("PASSED: Motion status updated");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    // Write date-time to Firebase
    if (Firebase.RTDB.setString(&fbdo, "/motion/dateTime", dateTime))
    {
      Serial.println("PASSED: dateTime updated");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    // Write ID
    if (Firebase.RTDB.setInt(&fbdo, "/motion/ID", cowID))
    {
      Serial.println("PASSED: ID updated");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    // Write temperature
    if (Firebase.RTDB.setFloat(&fbdo, "/motion/temperature", temperature))
    {
      Serial.println("PASSED: Temperature updated");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }

    // Write weight
    if (Firebase.RTDB.setFloat(&fbdo, "/motion/weight", weight))
    {
      Serial.println("PASSED: Weight updated");
    }
    else
    {
      Serial.printf("FAILED: %s\n", fbdo.errorReason().c_str());
    }
  }
}
