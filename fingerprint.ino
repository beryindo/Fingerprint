#include <SoftwareSerial.h>
#include <FPM.h>
#include <DFPlayer_Mini_Mp3.h>
//#include <Wire.h> 
//#include <LiquidCrystal_I2C.h>

// Search the DB for a print with this example

// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE/YELLOW wire)
//LiquidCrystal_I2C lcd(0x27,16,2);
SoftwareSerial mySerial(2, 3);

FPM finger;
const int relay1 = 8;
const int relay2 = 9;
const int relay3 = 10;
const int relay4 = 11;
int statusnya = 1;

void setup()  
{
  Serial.begin(9600);
  pinMode(relay3, OUTPUT);
  digitalWrite(relay3, LOW);
  mp3_set_serial(Serial);
  delay(1);
  mp3_reset();
  delay(1);
  mp3_set_volume (100);
//  lcd.begin();
//  lcd.backlight();
//  Serial.println("fingertest");
  mySerial.begin(57600); 
  if (finger.begin(&mySerial)) {
//    lcd.setCursor(0, 0); lcd.print("Found fingerprint sensor");
//    Serial.println("Found fingerprint sensor!");
//    Serial.print("Capacity: "); Serial.println(finger.capacity);
//    Serial.print("Packet length: "); Serial.println(finger.packetLen);
  } else {
//    Serial.println("Did not find fingerprint sensor :(");
//  lcd.setCursor(0, 0); lcd.print("Did not find fingerprint");
    while (1) yield();
  }

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH); 
//  pinMode(relay2, OUTPUT);
//  digitalWrite(relay2, HIGH);
  pinMode(relay4, OUTPUT);
  digitalWrite(relay4, HIGH);
//  lcd.clear(); 
  
}

void loop()                     // run over and over again
{
  if(statusnya == 1) {
    mp3_play (1);
    delay (1300);
    getFingerprintID();
  } else {
    
  }
}

int getFingerprintID() {
  int p = -1;
//  Serial.println("Waiting for a finger...");
//  lcd.clear(); 
//  lcd.setCursor(0, 0); lcd.print("Waiting Auth");
  while (p != FINGERPRINT_OK){
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
//        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
//        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
//        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
//        Serial.println("Imaging error");
        break;
      default:
//        Serial.println("Unknown error");
        break;
    }
    yield();
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
//      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
//      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
//      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
//      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
//      Serial.println("Could not find fingerprint features");
      return p;
    default:
//      Serial.println("Unknown error");
      return p;
  }

//  Serial.println("Remove finger...");
  while (p != FINGERPRINT_NOFINGER){
    p = finger.getImage();
    yield();
  }
//  Serial.println();
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    statusnya = 0;
//    lcd.clear();
//    lcd.setCursor(0, 0); lcd.print("Auth OK");
//    Serial.println("Found a print match!");
    if (finger.fingerID == 6 || finger.fingerID == 7 || finger.fingerID == 8) {
      mp3_play (5);
      delay (1800);
      int16_t id;
      if (get_free_id(&id))
      getFingerprintEnroll(id);  
    } else {
      mp3_play (2);
      delay (1400);
      digitalWrite(relay3, HIGH);
  //    lcd.clear();
  //    lcd.setCursor(0, 0); lcd.print("Conect Cable"); 
      delay(2500); 
      digitalWrite(relay4, LOW);
      delay(100);
      digitalWrite(relay1, LOW);
      mp3_play (3);
      delay (2400);
  //    lcd.clear();
  //    lcd.setCursor(0, 0); lcd.print("Start Engine"); 
//      delay(2000);
      digitalWrite(relay1, HIGH);
      digitalWrite(relay4, HIGH);
      delay(1000);
  //    lcd.clear();
  //    lcd.setCursor(0, 0); lcd.print("Engine ON"); 
  //    digitalWrite(relay2, LOW); 
  //    delay(2000);   
  //    digitalWrite(relay2, HIGH);      
    }
 
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    mp3_play (4);
    delay (1300);
//    lcd.clear();
//    lcd.setCursor(0, 1); lcd.print("Auth Fail");
    Serial.println("Did not find a match");
    return p;
  } else {
//    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
//  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
//  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
}

bool get_free_id(int16_t * id){
  int p = -1;
  for (int page = 0; page < (finger.capacity / FPM_TEMPLATES_PER_PAGE) + 1; page++){
    p = finger.getFreeIndex(page, id);
    switch (p){
      case FINGERPRINT_OK:
        if (*id != FINGERPRINT_NOFREEINDEX){
          Serial.print("Free slot at ID ");
          Serial.println(*id);
          return true;
        }
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error!");
        return false;
      default:
        Serial.println("Unknown error!");
        return false;
    }
    yield();
  }
}

int getFingerprintEnroll(int id) {
  int p = -1;
  Serial.println("Waiting for valid finger to enroll");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
    yield();
  }
  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
    yield();
  }

  p = -1;
  mp3_play (6);
  delay (2400);
//  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
    yield();
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  
  // OK converted!
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  mp3_play (7);
  delay (2500);
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    return 0;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}
