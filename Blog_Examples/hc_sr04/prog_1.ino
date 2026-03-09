/*
 * HC-SR04 ULTRAZVUKOVÝ SENZOR]
 */
#define trigPin 13      // Trigger Pin
#define echoPin 12      // Echo Pin
#define ledMin 11     // Červena LED na pinu 11
#define ledMax 10     // Zelena LED na pinu 10
int maximumRange = 200;       // maximální vzdálenost
int minimumRange = 0;       // minimální vzdálenost
long duration, distance;    // Vypočítání vzdálenosti
void setup() {
    Serial.begin (9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(ledMin, OUTPUT);
    pinMode(ledMax, OUTPUT);
}
void loop() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;   //vypočítání v cm

    // To je místo, kde LED rozsvítíme a zhasneme.
    if (distance < 4) {         // rozsvítíme červenou LED, vzdálenost do 4cm
        digitalWrite(ledMin,HIGH);    // Když rozsvítíme červenou, zelenou musíme zhasnout
        digitalWrite(ledMax,LOW);
    } else if (distance > 200) {        // rozsvítíme zelenou LED, vzdálenost > 200cm
        digitalWrite(ledMax,HIGH);    // Když rozsvítíme zelenou, červenou musíme zhasnout
        digitalWrite(ledMin,LOW);
    }
    else {          // normální provoz, vzdálenost od 4 do 200cm, nesviti nic
        digitalWrite(ledMin,LOW);
        digitalWrite(ledMax,LOW);
    }
    if (distance >= maximumRange  || distance <= minimumRange){
        Serial.println("Mimo dosah");
    }
    else {
        Serial.print(distance);
        Serial.println(" cm");
    }
    //počkání 500ms
    delay(500);
}
