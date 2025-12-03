# Smart-Ration-Distribution-System
IoT-based anti-corruption ration distribution device using Arduino
# Smart Ration Distribution System 🌾

## Project Overview
An advanced IoT-based automated ration distribution machine designed to eliminate corruption and theft in India's Public Distribution System (PDS). This system ensures 100% transparency using biometric/card authentication and automated dispensing.

## 🛠 Hardware Components
- **Controller:** Arduino Mega 2560 & Arduino UNO
- **Authentication:** RFID Reader (MFRC522) & 4x1 Keypad
- **Sensors:** Ultrasonic Sensor (HC-SR04) & Load Cell/FSR (Weight Sensor)
- **Actuators:** Servo Motor (Automatic Valve Control for Grain Dispensing)
- **Communication:** SIM900A GSM Module (SMS Alerts)
- **Display:** 16x2 LCD Display (User Interface)
- **Storage:** SD Card Module (Offline Data Logging)
- **Indicators:** LEDs (Green/Red Status Lights)

## ⚙️ How It Works
1. **Login:** Beneficiary scans their **RFID Smart Card** or enters PIN via Keypad.
2. **Verification:** System checks database; LCD displays Name & Available Quota.
3. **Dispensing:** **Servo Motor** opens the valve to release grain.
4. **Weighing:** Sensors monitor real-time weight; Servo closes automatically when exact weight (e.g., 5kg) is reached.
5. **Alert & Record:**
   - **SMS** sent to user: "Your 5kg ration distributed successfully."
   - **Data** (Time, Weight, ID) saved to **SD Card**.

## 🚀 Key Features
- **Automated Dispensing:** Servo motor ensures exact quantity; no manual cheating.
- **Smart Authentication:** RFID prevents fake users.
- **Paperless:** Digital records save trees (Environment Friendly).
- **Offline Mode:** Works in villages without internet (SD Card backup).

## 🌍 UN SDG Alignment
- **SDG 2:** Zero Hunger (Food Security)
- **SDG 16:** Peace, Justice & Strong Institutions (Anti-Corruption)
- **SDG 12:** Responsible Consumption & Production (No Wastage)

## License
MIT License
