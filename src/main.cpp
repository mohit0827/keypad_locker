#include <Arduino.h>

#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

void beep();
void doubleBeep();
void writeSetPasswordText();
void writeEnterPasswordText();
void lock();
void unlock();
void unlockOption();
void clearPassowordLine();
void writeBadPasswordText();
void writeLockedText();
bool compare(char a[], char b[]);

const int PAD_ROWNS = 4;
const int PAD_COLS = 4;

const int LCD_COLS = 16;
const int LCD_ROWNS = 2;

byte PAD_ROWN_PINS[PAD_ROWNS] = {9, 8, 7, 6};
byte PAD_COL_PINS[PAD_COLS] = {5, 4, 3, 2};

const int BUZZER_PIN = 12;
const int REPLAY_PIN = 13;

char padKeys[PAD_ROWNS][PAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


Keypad keypad = Keypad(makeKeymap(padKeys), PAD_ROWN_PINS, PAD_COL_PINS, PAD_ROWNS, PAD_COLS);
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWNS);

const int MAX_PASSWORD_LENGTH = 3;

int cursorPositionX = 0;
char inputKeys[MAX_PASSWORD_LENGTH] = {};
char passwordKeys[MAX_PASSWORD_LENGTH] = {};
boolean locked = false;
boolean displayOff = false;

void setup(){
    pinMode(BUZZER_PIN, OUTPUT);

    lcd.init();
    lcd.backlight();
    writeSetPasswordText();
}

void loop(){
    const char key = keypad.getKey();

    if(key){
        if (key == '#'){
            unlock();
            return;
        }

        if(!displayOff){
            if(key == '*'){
                if((cursorPositionX + 1 > MAX_PASSWORD_LENGTH) && !locked){
                    lock();
                }else {
                    clearPassowordLine();
                }

            }else {
                if(cursorPositionX + 1 > MAX_PASSWORD_LENGTH){
                    clearPassowordLine();
                }else {
                    lcd.setCursor(cursorPositionX, 1);
                    lcd.print("*");

                    beep();

                    inputKeys[cursorPositionX] = key;
                    cursorPositionX++;
                }
            } 
        }
    }
}

void writeSetPasswordText(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Zvolte heslo:");
}

void writeEnterPasswordText(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Zadejte heslo:");
}

void writeBadPasswordText(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Spatne heslo");
}

void writeLockedText(){
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Uzamknuto");
}

void beep(){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

void doubleBeep(){
  beep();
  delay(100);
  beep();
}

void clearPassowordLine(){
    doubleBeep(); 
    memset(inputKeys, 0, MAX_PASSWORD_LENGTH);
    cursorPositionX = 0;
    lcd.clear();
    
    if(!locked) writeSetPasswordText();
    else writeEnterPasswordText();
}

void unlockOption(){
    doubleBeep();
    lcd.backlight();
    writeEnterPasswordText();
    displayOff = false;
}

void lock(){
    doubleBeep();
    memcpy(passwordKeys, inputKeys, MAX_PASSWORD_LENGTH);
    memset(inputKeys, 0, MAX_PASSWORD_LENGTH);
    cursorPositionX = 0;

    locked = true;

    writeLockedText();

    lcd.noBacklight();
    displayOff = true;
}

void unlock(){
    if(displayOff){
        unlockOption();
        return;
    }
    
    if(compare(passwordKeys, inputKeys)){
        doubleBeep();

        lcd.clear();
        lcd.print("Odemknuto");

        digitalWrite(REPLAY_PIN, HIGH);
        delay(5000);
        digitalWrite(REPLAY_PIN, LOW);

        //TODO RESET 

        locked = false;
    } else {
        doubleBeep();
        writeBadPasswordText();
        delay(1500);

        clearPassowordLine();
    }
}

bool compare(char a[],char b[]){
    for(int i=0; i < MAX_PASSWORD_LENGTH; i++){
        if(a[i]!=b[i]){
            return false;
        }
    }
    return true;
}




