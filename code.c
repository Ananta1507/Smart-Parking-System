#include <ESP32Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TRIGGER_PIN1  12  // Pin trigger sensor ultrasonik 1
#define ECHO_PIN1     13  // Pin echo sensor ultrasonik 1
#define TRIGGER_PIN2  19  // Pin trigger sensor ultrasonik 2
#define ECHO_PIN2     18  // Pin echo sensor ultrasonik 2
#define SERVO_PIN     15  // Pin servo motor
#define BUZZER_PIN    2   // Pin buzzer

Servo myservo;  // Objek untuk mengontrol servo
NewPing sonar1(TRIGGER_PIN1, ECHO_PIN1); // Sensor ultrasonik 1
NewPing sonar2(TRIGGER_PIN2, ECHO_PIN2); // Sensor ultrasonik 2

#define I2C_ADDR     0x27
#define LCD_COLS     16
#define LCD_ROWS     2
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

int availableParking = 0; // Jumlah slot parkir yang tersedia
bool objectDetected = false; // Status deteksi objek pada sensor ultrasonik kedua

void setup() {
  myservo.attach(SERVO_PIN);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  int distance1 = sonar1.ping_cm(); // Baca jarak dari sensor ultrasonik 1
  int distance2 = sonar2.ping_cm(); // Baca jarak dari sensor ultrasonik 2

  if (distance1 > 0 && distance1 < 10) { // Jika objek terdeteksi dalam jarak 10 cm dari sensor 1
    myservo.write(90); // Buka servo pada 90 derajat
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ada mobil,");
    lcd.setCursor(0, 1);
    lcd.print("buka palang");
    Serial.println("Servo opened!");

    // Bunyikan buzzer 1 kali ketukan saat servo membuka
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100); // Bunyikan buzzer selama 100 ms
    digitalWrite(BUZZER_PIN, LOW);

    delay(5000); // Tunggu 5 detik sebelum menutup kembali servo

    myservo.write(0); // Tutup servo kembali
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Closed          "); // Menampilkan "Closed" pada baris pertama
    Serial.println("Servo closed!");

    // Bunyikan buzzer 1 kali ketukan saat servo menutup
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100); // Bunyikan buzzer selama 100 ms
    digitalWrite(BUZZER_PIN, LOW);
    
    // Kurangi jumlah slot parkir yang tersedia jika ada mobil masuk
    availableParking--;
  }

  if (distance2 > 1 && distance2 <= 5 && !objectDetected) { // Jika objek terdeteksi dalam jarak 1-5 cm dari sensor 2 dan belum ada deteksi sebelumnya
    // Bunyikan buzzer 3 kali ketukan
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100); // Bunyikan buzzer selama 100 ms
      digitalWrite(BUZZER_PIN, LOW);
      delay(100); // Tunggu 100 ms sebelum bunyi berikutnya
    }
    
    objectDetected = true; // Set status deteksi objek menjadi true
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Portal Closed   "); // Menampilkan "Portal Closed" pada baris pertama
    lcd.setCursor(0, 1);
    lcd.print("Parkir full !");
  } else if (distance2 <= 1 || distance2 > 5) { // Jika tidak ada objek terdeteksi atau objek terdeteksi di luar jarak yang ditentukan
    objectDetected = false; // Set status deteksi objek menjadi false
    digitalWrite(BUZZER_PIN, LOW); // Pastikan buzzer mati jika tidak ada objek terdeteksi
    lcd.clear();
    if (myservo.read() == 0) { // Jika posisi servo adalah 0 derajat
      lcd.setCursor(0, 0);
      lcd.print("Closed          "); // Menampilkan "Closed" pada baris pertama
    } else {
      lcd.setCursor(0, 0);
    lcd.print("Portal Closed   "); // Menampilkan "Portal Closed" pada baris pertama
      lcd.setCursor(0, 1);
      lcd.print("Parkir +1(P14) "); // Tampilkan pesan "Parkir +1" jika tidak ada objek terdeteksi
      availableParking++; // Tambah jumlah slot parkir yang tersedia jika tidak ada mobil
    }
    
  }
}
