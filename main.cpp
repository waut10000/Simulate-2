#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <security.h>

// define pins
#define trigPin 5
#define echoPin 2
#define magnetPin 25
// time for deepsleep
#define timeToSleep 5
#define timeToSecond 1000000
// debounce for magnet
#define DEBOUNCE_DELAY 50
// define vuilbak dept
#define diepte_vuilbak 42
// define sound speed in cm/uS
#define SOUND_SPEED 0.034
// deep dive variable voor magneet
bool lastSteadyState = 0;
bool lastBounceState = 0;
bool currentState;
unsigned long lastDebounceTime = 0;
// distance in cm
float distanceCm;
// count how many times the lid opens
int counter = 0;
// Variable for millis
unsigned long previousTime = 0;  
unsigned long delayTime = 2000;

void connectToWiFi()
{
  // Connect to Wi-Fi network
  WiFi.disconnect(true);
  // number of wifi connections
  const int numNetworks = sizeof(ssid) / sizeof(ssid[0]);
  // Scan for available WiFi networks
  int visibleNetworks = WiFi.scanNetworks();
  Serial.print("Scanning for networks...");
  Serial.println(visibleNetworks);

  for (int i = 0; i < visibleNetworks; i++)
  {
    for (int x = 0; x < numNetworks; x++)
    {
      if (WiFi.SSID(i) == ssid[x])
      {
        WiFi.begin(ssid[x], passwords[x]);
        Serial.print("Connecting to WiFi: ");
        while (WiFi.status() != WL_CONNECTED)
        {
          Serial.print('.');
          delay(700);
        }
        Serial.println(WiFi.status());
        Serial.println("Connected");
        break;
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);      // Starts the serial communication
  //connectToWiFi();           // start to strongest connection that i know
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as Input
  pinMode(magnetPin, INPUT); // set magnet sensor as Input
  // esp_sleep_enable_timer_wakeup(timeToSleep * timeToSecond); //wake up trigger by timer
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, LOW); // wake up trikker by magnet sensor
}

void volheid()
{
  // procent converter
  long procent;
  // bron https://randomnerdtutorials.com/esp32-hc-sr04-ultrasonic-arduino/
  long duration;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED / 2;

  float waarde = diepte_vuilbak - distanceCm;
  // map functie to get procentige
  procent = map(waarde, 0, diepte_vuilbak, 0, 100);

  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print(procent);
  Serial.println("% gevuld");
}

void deepsleep()
{
  Serial.println("Going to sleep now");
  delay(200);
  Serial.flush();
  esp_deep_sleep_start();
  // Serial.println("No print :)");
}

void send_data(float value)
{
  String data = "{\"vuilnisbak\":\"" + vuilnisbak_id + "\",\"value\":\"" + value + "\"}";

  // Send the POST request
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(data);

  if (httpResponseCode == 200)
  {
    Serial.println("Data inserted successfully");
    // delay(1000);
    deepsleep();
  }
  else
  {
    Serial.print("Error occurred. HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void loop()
{
  // 0 is open, 1 is closed
  currentState = digitalRead(magnetPin);
  // read status of magnet
  if (currentState != lastBounceState)
  {
    lastDebounceTime = millis();
    lastBounceState = currentState;
  }
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY && lastSteadyState != currentState)
  {
    if (currentState == LOW)
    {
      Serial.println("Open");
      counter++;
      Serial.print("Vuilbak is ");
      Serial.print(counter);
      Serial.println(" keer geopend");
    }
    else
    {
      Serial.println("Closed");
      Serial.println("Meting: ");
      previousTime = millis(); // Record the current time
      // Wait for the specified delay time to pass
      while (millis() - previousTime < delayTime)
      {
        // Do nothing
      }
      volheid();
      send_data(distanceCm);
    }
    lastSteadyState = currentState;
  }
}
