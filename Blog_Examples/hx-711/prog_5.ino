/* úpravy HX711_read/get_test
 A*utor: (C) fhruska */
#include "HX711.h"
#define g_factor 10 //-7050, his value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN 3
#define LOADCELL_SCK_PIN 2
HX711 scale;
byte n;
float Out1; float Out2; float Out3; float Out4;float tara_a;
byte flag_tara;
//.....................................................................
void setup() {
    Serial.begin(115200);delay(2000);
    Serial.println(">>>>>>>>>>>>HX711 testování");
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    Serial.println("Čeká se na tara...........");
    delay(1000);
    scale.set_scale(g_factor); //zadá se podle předchozích měření
    scale.tare(); //resetuje se na 0
    Out2=(scale.read_average(1)); //průměr nkrát čtení z HX
    Out3=(scale.get_value(1));//průměruje n1 krát čtení a počítá (HX- tara), (jeli nastavená)
    tara_a=Out2-Out3;
    Serial.print("tara_a=Out3-Out2: "); Serial.print(tara_a);
    Serial.print(", Out3 "); Serial.print(Out3);
    Serial.print(", Out2 "); Serial.println(Out2);
}
//....................................................................................................
void loop() {
    n++;
    Out1=(scale.read()); //1x údaj načtený z HX
    Out2=(scale.read_average(2)); //průměr nkrát čtení z HX
    Out3=(scale.get_value(2));//průměr n1krát čtení z HX- tara, (jeli nastavená)
    Out4=(scale.get_units(2)); // průměr n2krát čtení z (HX- tara)/g_factor, (je-li nastavené)
    Serial.print(">> Výpis n=");Serial.print(n); Serial.print(", Out1=");Serial.print(Out1);
    Serial.print(", Out2="); Serial.print(Out2);Serial.print(", Out3="); Serial.print(Out3);
    Serial.print(", Out4="); Serial.println(Out4);
    delay(2000);
}
