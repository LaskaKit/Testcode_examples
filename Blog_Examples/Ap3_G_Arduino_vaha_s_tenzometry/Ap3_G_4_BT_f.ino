/* aplikace vážení, 2xdvojité snímače á 1000 W, modul HX711,
 funkce čtení spusteniMereni(kanal)
 výstup na BT a oled 1,3" 
*/
#include "HX711.h"
#include <Wire.h>  // komunikace I2C
int pSCK = 2;
int pDT = 3;
byte p_tlac=4;byte tlac;byte n1;byte n2;byte n;
  
#include <SoftwareSerial.h>   // pro HC05, propojení BT
SoftwareSerial BT(11, 12); // (11) z Ardu jde na BT(TX), (12) z Ardu jde na BT(RX) 

float offset;float g_index; //pevná hodnota, jinak kody_pins();
float g_inde[8]; float ofset[8];
byte val_8; byte val_9; byte val_10; byte val_11; byte p_bin;  // byte val_12;byte val_8;

float output;float output1;float output20;float output21;float output0;
float hmotnost; //pro HBM kHm=217, pro VTS=326, pro Tesla = 89,12, 
float hmotnost0;float hmotnost1;byte zavazi=2;
float sum; float hx[3];float hx_klou;
float pom; float hx_m[3];float hx_medi;
float roz;
// definování různých nastavení kanálů a jejich zesílení
#define kanal_A_zesil_128 1
#define kanal_B_zesil_32 2
#define kanal_A_zesil_64 3

byte i;
byte flag_tara=1; //korekce offset, ==0 je settup=0, =1, je offset=output1/ 
byte flag_cejch; 
byte pruchod_korekce=0;// korekce po ř..10 průchodu v loop*/ 
float kore;
byte flag_print=1;  //(nastavení print USB)

//--------------------------------------------------------
void setup()  {
  Serial.begin(115200);delay(2000);

  pinMode(p_tlac, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(pSCK, OUTPUT);
  pinMode(pDT, INPUT);
  // probuzení modulu z power-down módu
  digitalWrite(pSCK, LOW);
  ofset[0]=-15685;ofset[1]=0;ofset[2]=0;ofset[3]=-1585;ofset[4]=-1560;
  ofset[5]=619;ofset[6]=118;ofset[7]=-165;
  g_inde[0]=171;g_inde[1]=1;g_inde[2]=1;g_inde[3]=150;g_inde[4]=(160)/1;g_inde[5]=180;
  g_inde[6]=166;g_inde[7]=46;
  
  BT.begin(9600);delay(1000);// Sériová komunikace s Bluetooth modulem
   
   Serial.println(" ");Serial.println("Začínám aplikaci AP3_G_kHmotnost...........");
  //Serial.print("tlac= "); Serial.println(tlac);
  delay(5);
}
//----------------------------------------------------------------
void loop()  {
 kody_pins(); offset=ofset[p_bin];g_index=g_inde[p_bin]; 
 output1=spusteniMereni(kanal_A_zesil_128 ); //čtení HX711 1x
 //if (n<=2)  { hx[n]=output1;sum+=output1;}   //vynechání počátku měření
 //if (n==2)  { hx_klou=sum/3; }     // výpočet prvního klouz.průměru
 if (n>=3) { hx[0]=hx[1];hx[1]=hx[2];hx[2]=output1;sum=0;   //vytvoření řetězce dat
      for (int i = 0; i < 3; i++) {
            sum+=hx[i];
            hx_klou=sum/3;     //klouzavý peůměr 
            roz=hx[2]-hx[1];    //rozdíl
          }
    }
 hmotnost=(output1-offset)/g_index;
 delay(10);
 if (flag_print==1)  {
   Serial.print(" n: ");Serial.print(n);
   Serial.print(" p_bin: ");Serial.print(p_bin);
   Serial.print(", hmotnost: ");Serial.print(hmotnost,4);
   Serial.print(", output1: ");Serial.print(output1);
   Serial.print(", roz: ");Serial.print(roz,1);
   Serial.println(" konec loop, čekání ......");
  }
 //.................výstup na BT:
 String dataString = String(n) + " ;" + String(p_bin) + " ;"+String(hmotnost,2);   // + " ; " + String(output1,1)+ " ;"  +String(roz,3);
 
 BT.println(dataString);  // Odeslání hodnoty přes Bluetooth
 delay(500); n++;
}
//-----------------------------------------------------
// vytvoření funkce pro měření z nastaveného kanálu
long spusteniMereni(byte mericiMod)  {
  byte index;
  long vysledekMereni = 0L;
  // načtení 24-bit dat z modulu
  while(digitalRead(pDT));
  for (index = 0; index < 24; index++)   {
    digitalWrite(pSCK, HIGH);
    vysledekMereni =  (vysledekMereni << 1) | digitalRead(pDT); //fukce OR
    digitalWrite(pSCK, LOW);
  }
  // nastavení měřícího módu
  for (index = 0; index < mericiMod; index++)    {
    digitalWrite(pSCK, HIGH);
    digitalWrite(pSCK, LOW);
  }
  // konverze z 24-bit dvojdoplňkového čísla na 32-bit znaménkové číslo
  if (vysledekMereni >= 0x800000)
    vysledekMereni = vysledekMereni | 0xFF000000L;
  vysledekMereni = vysledekMereni/128/2; //       /128/2
  return vysledekMereni;
}
//................................
void kody_pins()  {
  val_8=digitalRead(8);delay(10);
  val_9=digitalRead(9);delay(10);
  val_10=digitalRead(10);delay(10);   // val_12=digitalRead(pin_12);
  if (val_8==0&&val_9==0&&val_10==0) {p_bin=0;}
  if (val_8==1&&val_9==0&&val_10==0) {p_bin=1;}
  if (val_8==0&&val_9==1&&val_10==0) {p_bin=2;}
  if (val_8==1&&val_9==1&&val_10==0) {p_bin=3;}
  if (val_8==0&&val_9==0&&val_10==1) {p_bin=4;}
  if (val_8==1&&val_9==0&&val_10==1) {p_bin=5;}
  if (val_8==0&&val_9==1&&val_10==1) {p_bin=6;}
  if (val_8==1&&val_9==1&&val_10==1) {p_bin=7;}
 /* if (val_9==0&&val_10==0&&val_11==0&&val_12==1) {p_bin=8;}
  if (val_9==1&&val_10==0&&val_11==0&&val_12==1) {p_bin=9;} 
  if (val_9==0&&val_10==1&&val_11==0&&val_12==1) {p_bin=10;} 
  if (val_9==1&&val_10==1&&val_11==0&&val_12==1) {p_bin=11;} 
  if (val_9==0&&val_10==0&&val_11==1&&val_12==1) {p_bin=12;} 
  if (val_9==1&&val_10==0&&val_11==1&&val_12==1) {p_bin=13;} 
  if (val_9==0&&val_10==1&&val_11==1&&val_12==1) {p_bin=14;} 
  if (val_9==1&&val_10==1&&val_11==1&&val_12==1) {p_bin=15;}  */
  
  /*ofset[0]=0;ofset[1]=0;ofset[2]=0;ofset[3]=0;ofset[4]=0;
  ofset[5]=0;ofset[6]=0;ofset[7]=4395;
  /* ofset[8]=0;ofset[9]=0;
  ofset[10]=0;ofset[11]=0;ofset[12]=0;ofset[13]=0;ofset[14]=0;ofset[15]=305;
  g_inde[0]=1;g_inde[1]=1;g_inde[2]=1;g_inde[3]=1;g_inde[4]=1;g_inde[5]=1;
  g_inde[6]=1;g_inde[7]=78;
  g_inde[8]=1;g_inde[9]=1;g_inde[10]=1;g_inde[11]=1;
  g_inde[12]=1;g_inde[13]=1;g_inde[14]=1;g_inde[15]=75;*/ 
  // offset=ofset[p_bin];g_index=g_inde[p_bin];  // pevné hodnoty ř.15
      
  if (flag_print==3) {
    Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    Serial.print("pin8:");Serial.print(val_8);Serial.print(",pin9:");Serial.print(val_9);Serial.print(",pin10:");Serial.println(val_10);
    //Serial.print(",pin12:");Serial.print(val_12);
    //Serial.print("Nastavení: p_bin:");Serial.print(p_bin);
    //Serial.print(", offset=");Serial.print(offset);Serial.print(",g_index=");Serial.println(g_index,2); 
        //Serial.print(", flag_korekce ");Serial.print(flag_korekce);
    //Serial.print(", flag_tara ");Serial.println(flag_tara);
  }
}
//....................................................................
/*void displayWeight() {
  //float weight;
  display.setTextSize(2);
  display.setTextColor(WHITE); // pro 0,96" 
  // display.setTextColor(SH110X_WHITE); 
  display.setCursor(0, 5);
  display.print(hmotnost, 1);
  //display.print(" ");
  display.println(" kg ");
  display.setCursor(0, 25);
  display.setTextSize(1);
  display.print(" Pruchod= ");
  display.println(n);
  display.drawLine(0, 35, 128, 35, SSD1306_WHITE);
  // display.drawLine(0, 35, 128, 35, SH110X_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 38);
  display.print("Ap3_G_2b_1306"); display.print(", kod="); display.print(p_bin); 
  // display.print("Ap3_G_2_110X"); display.print(", kod="); display.print(p_bin); 
  display.setCursor(0, 48);
  display.print("flag_cejch="); display.print(flag_cejch); 
}*/
