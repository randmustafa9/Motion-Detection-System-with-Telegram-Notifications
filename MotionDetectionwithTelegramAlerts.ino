#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_PASSWORD";

// Telegram BOT Token and chat ID
const char* telegramBotToken = "YOUR_TELEGRAM_BOT_TOKEN";
const char* chat_id = "YOUR_CHAT_ID"; // Your Telegram user ID USE @myidbot to get youre id number

// PIR Sensor
const int pirPin = 4; // GPIO pin where the PIR sensor is connected

WiFiClientSecure client;
UniversalTelegramBot bot(telegramBotToken, client);

// NTP client to get the time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Update every 60 seconds

void setup() {
  Serial.begin(115200);

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  client.setInsecure(); // Disables SSL verification (for debugging)

  // Initialize PIR sensor pin
  pinMode(pirPin, INPUT);

  // Initialize NTP client
  timeClient.begin();
}

void loop() {
  timeClient.update();
  int motionDetected = digitalRead(pirPin);

  if (motionDetected == HIGH) {
    Serial.println("Motion detected!");

    // Get the current date and time
    String formattedTime = timeClient.getFormattedTime();
    time_t rawtime = timeClient.getEpochTime();
    struct tm *timeinfo = localtime(&rawtime);
    char buffer[30];
    strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", timeinfo);
    String dateTime(buffer);

    String message = "Motion detected!\nDate and Time: " + dateTime;
    if (bot.sendMessage(chat_id, message, "")) {
      Serial.println("Message sent successfully");
    } else {
      Serial.println("Failed to send message");
    }
    
    delay(10000); // Wait 10 seconds before checking again to avoid spamming
  }

  delay(500); // Poll every 0.5 seconds
}
