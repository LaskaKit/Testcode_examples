#include <Wire.h>  // Nacteni knihovny pro praci s I2C
#include <Adafruit_PWMServoDriver.h>  // Nacteni knihovny pro praci se servo shieldem PCA9685

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();  // Vytvoreni objektu pwm pro praci se servo shieldem

#define SERVOMIN  320 // Hodnota odpovidajici natoceni serva do polohy 0°
#define SERVOMAX  490 // Hodnota odpovidajici natoceni serva do polohy 180°
#define SERVO_FREQ 50 // Nastaveni frekvence PWM pro servomotor - 50Hz

// Prirazeni vystupnich pinu k jednotlivym servomotorum
#define SERVO1 0  // Servomotor 1 pripojen na vystup 0
#define SERVO2 1  // Servomotor 2 pripojen na vystup 1
#define SERVO3 2  // Servomotor 3 pripojen na vystup 2
#define SERVO4 3  // Servomotor 4 pripojen na vystup 3
#define SERVO5 4  // Servomotor 5 pripojen na vystup 4
#define SERVO6 5  // Servomotor 6 pripojen na vystup 5

// Prirazeni ovladacich potenciometru ke vstupnim pinum Arduina
#define POT1 A0  // Potenciometr 1 pripojen na analogovy vstup A0
#define POT2 A1  // Potenciometr 2 pripojen na analogovy vstup A1
#define POT3 A2  // Potenciometr 3 pripojen na analogovy vstup A2
#define POT4 A3  // Potenciometr 4 pripojen na analogovy vstup A3

void setup() {
    pwm.begin();  // Start komunikace se servo shieledem
    pwm.setOscillatorFrequency(25000000);  // Kalibrace vnitrniho ocilatoru cipu PCA9685
    pwm.setPWMFreq(SERVO_FREQ);  // Nastaveni frekvence PWM

    delay(10);
}

void loop() {
    int P1 = analogRead(POT1);  // Nacte hodnotu na analogovem pinu, ktera odpovida aktualni poloze natoceni potenciometru v rozsahu 0-1023
    P1 = mMap(P1, 0, 1023, SERVOMIN, SERVOMAX);  // Premapuje rozsah z analogoveho pinu 0-1023 na rozsah ovladani servomotoru SERVOMIN-SERVOMAX
    pwm.setPWM(SERVO1, 0, P1);  // Nastavi hodnotu pro servomotor a ten se podle ni natoci do pozadovane polohy 0-180°

    int P2 = analogRead(POT2);  // Nacte hodnotu na analogovem pinu, ktera odpovida aktualni poloze natoceni potenciometru v rozsahu 0-1023
    P2 = mMap(P2, 0, 1023, SERVOMIN, SERVOMAX);  // Premapuje rozsah z analogoveho pinu 0-1023 na rozsah ovladani servomotoru SERVOMIN-SERVOMAX
    pwm.setPWM(SERVO2, 0, P2);  // Nastavi hodnotu pro servomotor a ten se podle ni natoci do pozadovane polohy 0-180°

    int P3 = analogRead(POT3);  // Nacte hodnotu na analogovem pinu, ktera odpovida aktualni poloze natoceni potenciometru v rozsahu 0-1023
    P3 = mMap(P3, 0, 1023, SERVOMIN, SERVOMAX);  // Premapuje rozsah z analogoveho pinu 0-1023 na rozsah ovladani servomotoru SERVOMIN-SERVOMAX
    pwm.setPWM(SERVO3, 0, P3);  // Nastavi hodnotu pro servomotor a ten se podle ni natoci do pozadovane polohy 0-180°

    int P4 = analogRead(POT4);  // Nacte hodnotu na analogovem pinu, ktera odpovida aktualni poloze natoceni potenciometru v rozsahu 0-1023
    P4 = mMap(P4, 0, 1023, SERVOMIN, SERVOMAX);  // Premapuje rozsah z analogoveho pinu 0-1023 na rozsah ovladani servomotoru SERVOMIN-SERVOMAX
    pwm.setPWM(SERVO4, 0, P4);  // Nastavi hodnotu pro servomotor a ten se podle ni natoci do pozadovane polohy 0-180°
}

// Funkce pro premapovani jednoho rozsahu do druheho.
// Originalni funkce map() nedava vzdy spravne vysledky
long mMap(long x, long in_min, long in_max, long out_min, long out_max)
{
    long in_size = in_max - in_min;
    long out_size = out_max - out_min;
    if( abs(in_size) > abs(out_size) )
    {
        if( in_size < 0 ) in_size--; else in_size++;
        if( out_size < 0 ) out_size--; else out_size++;
    }
    return (x - in_min) * (out_size) / (in_size) + out_min;
}
