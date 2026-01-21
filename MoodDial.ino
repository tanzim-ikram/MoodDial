#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// ---------- BACKEND ----------
const char* BACKEND_URL = "https://telegram-ai-bot-backend-production.up.railway.app/webhook";
const char* CHAT_ID    = "YOUR_CHAT_ID";

// ---------- PIN DEFINITIONS ----------
#define ENCODER_CLK 3
#define ENCODER_DT 2
#define ENCODER_SW 1
#define NEOPIXEL_PIN 0
#define OLED_SDA 8
#define OLED_SCL 9
#define WIFI_LED_PIN 10

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- NEOPIXEL ----------
Adafruit_NeoPixel pixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// ---------- MOODS ----------
const char* moods[] = {"Angry", "Calm", "Depressed", "Energetic", "Feared", "Happy", "Shameful", "Shocked", "Stressed", "Sad"};
const char* emojis[] = {"(o_O)", "(^-^)", "(T^T)", "(o^v^o)", "(o_o)", "(o^-^o)", "(^_^!)", "(O.O)", "(>_<)", "(T-T)"};
int moodIndex = 0;

uint32_t moodColors[] = {
  pixel.Color(255, 0, 0),    // Angry - Red
  pixel.Color(0, 255, 255),  // Calm - Cyan
  pixel.Color(127, 0, 255),  // Depressed - Violet
  pixel.Color(0, 255, 0),  // Energetic - Green
  pixel.Color(100, 100, 100),  // Feared - Grey
  pixel.Color(255, 255, 0),   // Happy - Yellow
  pixel.Color(255, 20, 147),  // Shameful - Deep Pink
  pixel.Color(255, 105, 180),  // Shocked - Hot Pink
  pixel.Color(255, 165, 0),    // Stressed - Orange
  pixel.Color(0, 0, 255)    // Sad - Blue
};

// ---------- ENCODER ----------
volatile int lastStateCLK;
volatile bool encoderMoved = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 5;

// ---------- UI ----------
enum UIState {
  SPLASH,
  GREETING,
  MOOD_SELECTION
};
UIState currentState = SPLASH;
unsigned long stateTimer = 0;

// ---------- BUTTON ----------
bool lastButtonState = HIGH;
unsigned long lastButtonPress = 0;
const unsigned long buttonDebounce = 50;

void IRAM_ATTR encoderISR() {
  unsigned long now = millis();
  if (now - lastDebounceTime > debounceDelay) {
    int currentCLK = digitalRead(ENCODER_CLK);
    if (currentCLK != lastStateCLK && currentCLK == LOW) {
      if (digitalRead(ENCODER_DT) == HIGH) moodIndex++;
      else moodIndex--;

      if (moodIndex < 0) moodIndex = 9; // Wrap around the moodIndex if it's below 0
      if (moodIndex > 9) moodIndex = 0; // Wrap around the moodIndex if it's above 9

      encoderMoved = true;
    }
    lastStateCLK = currentCLK;
    lastDebounceTime = now;
  }
}

// ---------- BACKEND SEND ----------
void sendMood(const char* mood) {
  if (WiFi.status() != WL_CONNECTED) return;

  // Clear the display and show sending status
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 22);
  display.print("Sending mood...");
  display.setCursor(0, 36);
  display.print(mood);
  display.display();

  HTTPClient http;
  http.begin(BACKEND_URL);
  http.addHeader("Content-Type", "application/json");

  String payload =
    "{\"message\": {\"chat\": {\"id\": \"" + String(CHAT_ID) + "\"}, \"text\": \"" + String(mood) + "\"}}";

  int httpCode = http.POST(payload);
  http.end();

  // Display status based on response
  display.clearDisplay();
  display.setCursor(0, 28);
  display.setTextSize(1);
  display.print(httpCode == 200 ? "AI responding..." : "Backend error");
  display.display();

  delay(2000);
  currentState = GREETING;
  stateTimer = millis();
}

// ---------- UI (UNCHANGED) ----------
void drawSplash() {
  display.clearDisplay();

  // Centered MoodDial text
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 35);
  display.print("MoodDial");
  display.display();
}

void drawGreeting() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Center the text better
  display.setCursor(10, 18);
  display.print("Hi!");
  display.setCursor(10, 32);
  display.print("How are you");
  display.setCursor(10, 44);
  display.print("feeling today?");
  display.display();
}

void drawMoodSelection() {
  display.clearDisplay();
  
  // Top section: Mood name
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 2);
  display.print("Mood:");
  
  // Mood name - dynamically calculated for centering
  display.setTextSize(1);
  int textWidth = strlen(moods[moodIndex]) * 6; // Approximate width of the text
  int xPos = (SCREEN_WIDTH - textWidth) / 2;  // Center align the text
  display.setCursor(xPos, 14);
  display.print(moods[moodIndex]);
  
  // Middle section: Emoji - dynamically calculated for centering
  display.setTextSize(2);  // Size for the emoji
  int emojiWidth = strlen(emojis[moodIndex]) * 12; // Approximate width for the emoji text
  int emojiX = (SCREEN_WIDTH - emojiWidth) / 2;  // Center align the emoji
  display.setCursor(emojiX, 32);
  display.print(emojis[moodIndex]);
  
  // Bottom section: Visual slider
  int y = 56;  // Y-position for the circles
  int spacing = (SCREEN_WIDTH - 28) / 9;  // Dynamically calculate spacing based on total number of moods (10 in this case)
  
  // Draw the circles and make the selected one bigger
  for (int i = 0; i < 10; i++) {
    int x = 14 + i * spacing;  // Calculate X-position for each circle
    if (i == moodIndex) {
      display.fillCircle(x, y, 4, SSD1306_WHITE);  // Selected mood has a bigger circle
    } else {
      display.drawCircle(x, y, 3, SSD1306_WHITE);  // Other moods have a smaller circle
    }
  }

  display.display();
  pixel.setPixelColor(0, moodColors[moodIndex]);  // Set NeoPixel color based on selected mood
  pixel.show();
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  pinMode(WIFI_LED_PIN, OUTPUT);

  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pixel.begin();
  pixel.setBrightness(50);
  pixel.clear();
  pixel.show();

  drawSplash();

  // Initialize WiFiManager
  WiFiManager wifiManager;
  wifiManager.autoConnect("MoodDial"); // This will start a captive portal with the SSID "MoodDial"
  
  // After successful connection
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 22);
  display.print("Connected to WiFi");
  display.display();

  lastStateCLK = digitalRead(ENCODER_CLK);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), encoderISR, CHANGE);
}

// ---------- LOOP ----------
void loop() {
  digitalWrite(WIFI_LED_PIN,
    WiFi.status() == WL_CONNECTED ? HIGH : (millis() / 500) % 2);

  if (currentState == SPLASH && millis() - stateTimer > 2000) {
    currentState = GREETING;
    stateTimer = millis();
    drawGreeting();
  }

  if (currentState == GREETING &&
     (encoderMoved || millis() - stateTimer > 5000)) {
    encoderMoved = false;
    currentState = MOOD_SELECTION;
    drawMoodSelection();
  }

  if (currentState == MOOD_SELECTION && encoderMoved) {
    encoderMoved = false;
    drawMoodSelection();
  }

  bool btn = digitalRead(ENCODER_SW);
  if (btn == LOW && lastButtonState == HIGH &&
      millis() - lastButtonPress > buttonDebounce) {

    lastButtonPress = millis();
    for (int i = 0; i < 3; i++) {
      pixel.setPixelColor(0, moodColors[moodIndex]);
      pixel.show(); delay(100);
      pixel.clear(); pixel.show(); delay(100);
    }

    sendMood(moods[moodIndex]);
  }
  lastButtonState = btn;
}
