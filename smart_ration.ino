#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <SD.h>

// ---------------- PIN CONFIG ----------------
#define SS_PIN 53
#define RST_PIN 49
#define TRIG_PIN A1
#define ECHO_PIN A2
#define SERVO_PIN 6
#define BUZZER_PIN 2
#define LED_PIN 3

#define LCD_RS 8
#define LCD_EN 9
#define LCD_D4 10
#define LCD_D5 11
#define LCD_D6 12
#define LCD_D7 13

#define SD_CS 4

// ---------------- OBJECTS ----------------
MFRC522 rfid(SS_PIN, RST_PIN);
Servo servoMotor;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// SIM900A on Serial1 (Arduino Mega)
#define sim Serial1

// ---------------- CARD DATA ----------------
byte card1[4] = {0xA6, 0x71, 0x32, 0xF9}; // Utkarsh
byte card2[4] = {0x36, 0x17, 0xAC, 0xF8}; // Ramdas

String userName = "";
String userAddress = "";
String userNumber = "";

const int DIST_LIMIT = 10;

// ---------- UTILITY: read line from SIM with timeout ----------
String simReadWithTimeout(unsigned long timeoutMs) {
  unsigned long start = millis();
  String res = "";
  while (millis() - start < timeoutMs) {
    while (sim.available()) {
      char c = char(sim.read());
      res += c;
    }
    if (res.length() > 0) delay(20);
  }
  return res;
}

// ---------------- FUNCTIONS ----------------
long readDistance() {
  digitalWrite(TRIG_PIN,LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN,LOW);
  long duration = pulseIn(ECHO_PIN,HIGH,30000);
  if(duration == 0) return 999;
  return duration * 0.034 / 2;
}

void softBeep(int freq=800,int duration=80){
  tone(BUZZER_PIN, freq, duration);
}

void wrongCardAlert(){
  for(int i=0;i<4;i++){
    digitalWrite(LED_PIN,HIGH);
    tone(BUZZER_PIN,1000,180);
    delay(200);
    digitalWrite(LED_PIN,LOW);
    delay(150);
  }
}

void showIdle(){
  lcd.clear(); delay(50);
  lcd.setCursor(0,0); lcd.print("Welcome!");
  delay(700);
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Scan Rashan Card");
  lcd.setCursor(0,1); lcd.print("AnnSeva Card");
  delay(700);
}

bool waitForObject(){
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Please Get Your");
  lcd.setCursor(0,1); lcd.print("Rashan");
  while(true){
    long d = readDistance();
    if(d < DIST_LIMIT){
      delay(500);
      if(readDistance() < DIST_LIMIT) return true;
    }
  }
}

// ---------------- SIM900A DATE-TIME (safer) ----------------
String getDateTime(){
  sim.println("AT+CCLK?");
  delay(300);
  String resp = simReadWithTimeout(1500);
  // Example response: +CCLK: "21/11/28,14:32:10+04"
  int q1 = resp.indexOf('"');
  int q2 = resp.indexOf('"', q1+1);
  if(q1 != -1 && q2 != -1 && q2 > q1){
    String inside = resp.substring(q1+1, q2);
    // Return full inside (e.g. 21/11/28,14:32:10+04)
    return inside;
  }
  return "00/00/00,00:00:00";
}

// ---------------- SMS FUNCTION ----------------
void sendSMS(String number, String name, String address){
  String dt = getDateTime();
  String day = "NA";

  String msg =
    "राशन सम्बन्धित जानकारी\n"
    "दिनांक - " + dt + "\n"
    "दिन - " + day + "\n"
    "नाम - " + name + "\n"
    "पता - " + address + "\n"
    "आपका राशन निकाला गया है l\n"
    "मशीन क्रमांक - 99999999\n"
    "आपका धन्यवाद 🙂🙏";

  Serial.println("Sending SMS to " + number);
  sim.println("AT+CMGF=1"); // text mode
  delay(300);
  sim.print("AT+CMGS=\"");
  sim.print(number);
  sim.println("\"");
  delay(300);
  sim.print(msg);
  sim.write(26); // CTRL-Z
  // wait for send response
  String r = simReadWithTimeout(4000);
  Serial.println("SIM response after SMS: ");
  Serial.println(r);
}

// ---------------- SD SAVE ----------------
void saveToSD(String name, String address){
  String dt = getDateTime();
  File f = SD.open("ration.txt", FILE_WRITE);
  if(f){
    f.print("नाम: "); f.print(name);
    f.print(" | पता: "); f.print(address);
    f.print(" | समय: "); f.println(dt);
    f.close();
    Serial.println("Saved to SD.");
  } else {
    Serial.println("Failed to open ration.txt on SD.");
  }
}

// ---------------- FOOD DELIVERY ----------------
void deliverFood(){
  digitalWrite(LED_PIN,HIGH);
  servoMotor.write(90);

  unsigned long t = millis();
  while(millis() - t < 4000){
    softBeep();
    delay(300);
  }

  servoMotor.write(0);
  digitalWrite(LED_PIN,LOW);

  // SMS + SD
  sendSMS(userNumber, userName, userAddress);
  saveToSD(userName, userAddress);

  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Successful Sent");
  lcd.setCursor(0,1); lcd.print(userName);
  delay(2000);

  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Dhanyawad!!");
  lcd.setCursor(0,1); lcd.print(userName);
  delay(2000);
}

// ---------------- SETUP ----------------
void setup(){
  Serial.begin(115200);
  while(!Serial){}
  Serial.println("Booting...");

  // SIM on Serial1 (Mega)
  sim.begin(9600);
  delay(200);
  Serial.println("SIM Serial1 started.");

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID initialized.");

  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0);

  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(LED_PIN,OUTPUT);
  pinMode(BUZZER_PIN,OUTPUT);

  lcd.begin(16,2);

  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed! Check wiring or CS pin.");
  } else {
    Serial.println("SD initialized.");
  }
}

// ---------------- LOOP ----------------
void loop(){
  showIdle();

  if(!rfid.PICC_IsNewCardPresent()) return;
  if(!rfid.PICC_ReadCardSerial()) return;

  byte *uid = rfid.uid.uidByte;

  if(uid[0]==card1[0] && uid[1]==card1[1] && uid[2]==card1[2] && uid[3]==card1[3]){
    userName = "Utkarsh Namdeo";
    userAddress = "38 No. Danga Majhauli";
    userNumber = "8817350645";
    tone(BUZZER_PIN,600,120);
    lcd.clear(); lcd.setCursor(0,0); lcd.print("Namaste!"); lcd.setCursor(0,1); lcd.print(userName); delay(3000);
    lcd.clear(); lcd.setCursor(0,0); lcd.print("38 No. Danga"); lcd.setCursor(0,1); lcd.print("Majhauli M.P."); delay(4000);
    if(waitForObject()) deliverFood();
    return;
  }

  if(uid[0]==card2[0] && uid[1]==card2[1] && uid[2]==card2[2] && uid[3]==card2[3]){
    userName = "Ramdas Saket";
    userAddress = "08 No. Danga Majhauli";
    userNumber = "9589535941";
    tone(BUZZER_PIN,600,120);
    lcd.clear(); lcd.setCursor(0,0); lcd.print("Namaste!"); lcd.setCursor(0,1); lcd.print(userName); delay(3000);
    lcd.clear(); lcd.setCursor(0,0); lcd.print("08 No. Danga"); lcd.setCursor(0,1); lcd.print("Majhauli M.P."); delay(4000);
    if(waitForObject()) deliverFood();
    return;
  }

  wrongCardAlert();
}
