#include <IRremote.h>
#include <NewPing.h>
#include <NewTone.h>
#include <LiquidCrystal.h>

const int TRIG_PIN = 2;
const int ECHO_PIN = 4;
const int SOUND_PIN = 3; // PWM
const int RED_LED_PIN = 5;
const int LCD_ON_PIN = 6;
const int RECEIVER_PIN = 7;

NewPing sonar(TRIG_PIN, ECHO_PIN, 500);
IRrecv irrecv(RECEIVER_PIN);
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
int distance = 0;
int normalDistance = 0;
int alarmTimes = 0;
int oldReceiverValue = 0;
int speedTime = 500;
int fullAlarmTimes = 0;
bool loopActive = false;
bool sound = true;
bool light = true;
bool screen = true;
bool ledFast = false;
bool alwaysAlarm = false;
bool stopAlarm = false;
byte powerButton[8] = {
  B00100,
  B00100,
  B01110,
  B10101,
  B10001,
  B10001,
  B01110,
  B00000
};
byte playButton[8] = {
  B11000,
  B11100,
  B11110,
  B11111,
  B11110,
  B11100,
  B11000,
  B00000
};

void (*reset) () = 0;

void lcdMessage(String message, String message2 = "") {
  if (! screen) {
    digitalWrite(LCD_ON_PIN, HIGH);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.setCursor(0, 1);
  lcd.print(message2);
  delay(1500);
  if (! screen) {
    digitalWrite(LCD_ON_PIN, LOW);
  }
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SOUND_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(LCD_ON_PIN, OUTPUT);

  digitalWrite(LCD_ON_PIN, HIGH);
  irrecv.enableIRIn();

  lcd.createChar(0, powerButton);
  lcd.createChar(1, playButton);
  lcd.begin(16, 2);
  lcdMessage("Arduino Alarm", "by Thijzert");

  lcd.clear();
  lcd.print("Press ");
  lcd.write(byte(0));
  lcd.print(" or ");
  lcd.write(byte(1));
  lcd.setCursor(0, 1);
  lcd.print("to start");
  delay(2000);
  lcd.clear();
}

void loop() {
  checkReceiver();
  calcDistance();
  if (loopActive) {
    if (mustAlarm(false)) {
      loopAlarm();
    }
  }
  
  delay(speedTime);
}

void calcDistance() {
  distance = sonar.ping_cm();
  updateScreen();
}

void updateScreen() {
  lcd.clear();
  if (screen) {
    digitalWrite(LCD_ON_PIN, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("D: ");
    lcd.print(distance);
    lcd.setCursor(7, 0);
    lcd.print("ND: ");
    lcd.print(normalDistance);
    lcd.setCursor(0, 1);
    lcd.print("S: ");
    lcd.print(speedTime / 100);
    lcd.setCursor(7, 1);
    lcd.print("A: ");
    lcd.print(fullAlarmTimes);
    if (! loopActive) {
      lcd.setCursor(13, 1);
      lcd.print("off");
    }
  } else {
    digitalWrite(LCD_ON_PIN, LOW);
  }
}

bool mustAlarm(bool isAlarm) {
  if (alwaysAlarm && isAlarm) {
    return true;
  }
  if (distance < (normalDistance - 5) || distance > (normalDistance + 5)) {
    if (! isAlarm) {
      if (alarmTimes >= 2) {
        alarmTimes = 0;
        fullAlarmTimes++;
        return true;
      } else {
        alarmTimes++;
        if (light) {
          digitalWrite(RED_LED_PIN, HIGH);
        }
        return false;
      }
    } else {
      return true;
    }
  }
  alarmTimes = 0;
  digitalWrite(RED_LED_PIN, LOW);
  return false;
}

void loopAlarm() {
  while (mustAlarm(true)) {
    if (loopActive) {
      alarm();
      calcDistance();
    } else {
      delay(speedTime);
    }
    checkReceiver();
    if (stopAlarm) {
      stopAlarm = false;
      break;
    }
  }
}

void alarm() {
  int duration;
  if (ledFast) {
    duration = speedTime / 5 * 3;
  } else {
    duration = 300;
  }
  
  if (light) {
    digitalWrite(RED_LED_PIN, HIGH);
  }
  if (sound) {
    NewTone(SOUND_PIN, 740, duration);
  }

  delay(duration);

  digitalWrite(RED_LED_PIN, LOW);
  lcd.clear();
  if (sound) {
    NewTone(SOUND_PIN, 750, duration);
  }
  delay(duration);
}

void stopLoop() {
  digitalWrite(RED_LED_PIN, LOW);
  loopActive = false;
  stopAlarm = true;
}

void checkReceiver() {
  if (irrecv.decode()) {
    unsigned long receiverValue = irrecv.decodedIRData.decodedRawData;
    
    unsigned long powerButton = 0xFD020707;
    unsigned long exitButton = 0xD22D0707;
    unsigned long pauseButton = 0xB54A0707;
    unsigned long playButton = 0xB8470707;
    unsigned long returnButton = 0xA7580707;
    
    unsigned long upButton = 0x9F600707;
    unsigned long downButton = 0x9E610707;
    
    unsigned long mainButton = 0x97680707;
    
    unsigned long leftButton = 0x9A650707;
    unsigned long backwardButton = 0xBA450707;
    
    unsigned long rightButton = 0x9D620707;
    unsigned long forwardButton = 0xB7480707;
    
    unsigned long oneButton = 0xFB040707;
    unsigned long fourButton = 0xF7080707;
    unsigned long sevenButton = 0xF30C0707;
    
    unsigned long twoButton = 0xFA050707;
    unsigned long fiveButton = 0xF6090707;
    unsigned long eightButton = 0xF20D0707;
    
    unsigned long threeButton = 0xF9060707;
    unsigned long sixButton = 0xF50A0707;
    unsigned long nineButton = 0xF10E0707;
    unsigned long muteButton = 0xF00F0707;

    unsigned long zeroButton = 0xEE110707;

    unsigned long redButton = 0x936C0707;
    unsigned long greenButton = 0xEB140707;
    unsigned long yellowButton = 0xEA150707;
    unsigned long blueButton = 0xE9160707;

    unsigned long highButton = 0xED120707;
    unsigned long lowButton = 0xEF100707;

    unsigned long recordButton = 0xB6490707;
    unsigned long stoprecordButton = 0xB9460707;

    if (receiverValue == powerButton) {
      if (loopActive) {
        stopLoop();
      } else {
        loopActive = true;
      }
      delay(500);
    } else if (receiverValue == exitButton) {
      if (loopActive) {
        stopLoop();
        delay(500);
      } else {
        reset();
      }
    } else if (receiverValue == upButton) {
      if (normalDistance < 500) {
        normalDistance++;
      }
    } else if (receiverValue == downButton) {
      if (normalDistance > 0) {
        normalDistance--;
      }
    } else if (receiverValue == mainButton) {
      stopAlarm = true;
    } else if (receiverValue == leftButton) {
      if (normalDistance >= 10) {
        normalDistance -= 10;
      }
    } else if (receiverValue == rightButton) {
      if (normalDistance <= 490) {
        normalDistance += 10;
      }
    } else if (receiverValue == oneButton || receiverValue == fourButton || receiverValue == sevenButton) {
      sound = true;
      light = true;
      lcdMessage("Sound: on", "Light: on");
    } else if (receiverValue == twoButton || receiverValue == fiveButton || receiverValue == eightButton) {
      sound = true;
      light = false;
      lcdMessage("Sound: on", "Light: off");
    } else if (receiverValue == threeButton || receiverValue == sixButton || receiverValue == nineButton || (receiverValue == muteButton && sound == true)) {
      sound = false;
      light = true;
      lcdMessage("Sound: off", "Light: on");
    } else if (receiverValue == muteButton && sound == false) {
      sound = true;
      light = false;
      lcdMessage("Sound: on", "Light: off");
    } else if (receiverValue == backwardButton) {
      if (normalDistance >= 100) {
        normalDistance -= 100;
      }
    } else if (receiverValue == forwardButton) {
      if (normalDistance <= 400) {
        normalDistance += 100;
      }
    } else if (receiverValue == redButton) {
      normalDistance = 200;
    } else if (receiverValue == greenButton) {
      normalDistance = 300;
    } else if (receiverValue == yellowButton) {
      normalDistance = 400;
    } else if (receiverValue == blueButton) {
      normalDistance = 500;
    } else if (receiverValue == returnButton) {
      if (! alwaysAlarm) {
        alwaysAlarm = true;
        lcdMessage("Always loop", "alarm: on");
      } else {
        alwaysAlarm = false;
        lcdMessage("Always loop", "alarm: off");
      }
    } else if (receiverValue == highButton) {
      if (speedTime <= 950) {
        speedTime += 100;
      }
    } else if (receiverValue == lowButton) {
      if (speedTime >= 200) {
        speedTime -= 100;
      }
    } else if (receiverValue == pauseButton) {
      stopLoop();
    } else if (receiverValue == playButton) {
      lcdMessage("Alarm: on");
      loopActive = true;
    } else if (receiverValue == zeroButton) {
      if (! sound && ! light && ! screen) {
        sound = true;
        light = true;
        lcdMessage("Sound: on", "Light: on");
        screen = true;
      } else {
        if (! sound && ! light) {
          screen = false;
        } else {
          sound = false;
          light = false;
          lcdMessage("Sound: off", "Light: off");
        }
      }
    } else if (receiverValue == recordButton || receiverValue == stoprecordButton) {
      if (ledFast) {
        ledFast = false;
        lcdMessage("Fast light: off");
      } else {
        ledFast = true;
        lcdMessage("Fast light: on");
      }
      delay(500);
    }

    irrecv.resume();
  }
}
