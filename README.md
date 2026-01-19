# MoodDial Bot 🎛️🤖
*A Tangible AI-Based Emotional Interaction System*


## Project Overview

MoodDial Bot is a physical, emotion-aware interaction device that allows users to express how they feel using a **rotary dial**. Instead of typing messages or navigating apps, users select a mood through a tangible interface, and receive a **supportive AI-generated response** via **Telegram**.

The system emphasizes **low cognitive load**, **accessibility**, and **human-centered interaction** by combining embedded hardware, a minimal UI, and AI-powered backend processing.


## Key Features

- Tangible mood selection using a rotary encoder  
- OLED display for real-time mood visualization  
- NeoPixel LED with color-coded emotional feedback  
- AI-generated responses tailored to selected moods  
- Telegram-based message delivery (asynchronous interaction)  
- Secure backend communication using a shared secret key  
- No need to open ChatGPT or type messages manually  


## Hardware Components

- ESP32 (ESP32-C3 / ESP32)
- Rotary Encoder with push button
- 0.96” OLED Display (SSD1306)
- NeoPixel LED (WS2812B)
- Wi-Fi status LED
- Battery
- Boost Module
- wires


## Wire Connection

| ESP32 Pin | Component |
|----------|-----------|
| GPIO 3 | Encoder CLK |
| GPIO 2 | Encoder DT |
| GPIO 1 | Encoder Button |
| GPIO 0 | NeoPixel Data |
| GPIO 8 | OLED SDA |
| GPIO 9 | OLED SCL |
| GPIO 10 | Wi-Fi Status LED |
| 5V | OLED, LEDs & Rotary Encoder |
| GND | Common Ground |


## System Flow Summary

1. ESP32 boots and connects to Wi-Fi  
2. OLED displays splash screen and greeting  
3. User rotates the encoder to select a mood  
4. NeoPixel changes color based on mood  
5. User presses the encoder button to confirm  
6. ESP32 sends mood data securely to the backend  
7. Backend processes mood using AI  
8. AI-generated response is delivered via Telegram  
9. User reads the response anytime (asynchronous experience)


## Software Architecture

MoodDial Device (ESP32) -> Backend Server (Node.js + OpenAI) -> Telegram Bot -> User Inbox


## Supported Moods

- Angry
- Calm
- Depressed
- Energetic
- Feared
- Happy
- Shameful
- hocked
- Stressed
- Sad

Each mood triggers a unique UI state, LED color, and AI response.


## Use Cases

- Emotional self check-ins  
- Stress relief during work or study  
- Tangible User Interface (TUI) research  
- Human-Computer Interaction (HCI) projects  
- Accessibility-focused interaction systems  


## Future Improvements

- Display AI responses on OLED  
- Animated mood transitions  
- Offline fallback messages  
- Multi-user support  
- Battery optimization and deep sleep  
- BLE-based interaction mode  


## License

This project is intended for educational, research, and personal use.


**MoodDial Bot**  
*Turn your mood. Get support.*
