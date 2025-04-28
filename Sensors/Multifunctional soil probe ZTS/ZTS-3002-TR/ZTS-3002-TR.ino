/* Test code for Multifunctional soil probe ZTS-3002-TR-ECTHNPKPH-N01, PH, NPK, conductivity, temperature and humidity RS485
 * 
 * Board:   LaskaKit UNO R3, ATmega328P                                     https://www.laskakit.cz/arduino-uno-r3--atmega328p--precizni-klon/
 *          TTL to RS 485, MAX485 converter                                 https://www.laskakit.cz/prevodnik-ttl-na-rs-485--max485/
 * Sensor: Multifunctional soil probe ZTS-3002-TR-ECTHNPKPH-N01, PH, NPK, 
 *          conductivity, temperature and humidity RS485                    https://www.laskakit.cz/multifunkcni-pudni-sonda-zts-3000-tr-ws-n01--teplota-a-vlhkost-rs485/
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <SoftwareSerial.h>

#define rxPin 3
#define txPin 2

SoftwareSerial ser485(rxPin, txPin);

const byte inq1[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x06, 0xC5, 0xC8}; // dotaz na adresu 1
const byte inq2[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x06, 0xC5, 0xFB};  // dotaz na adresu 2
const byte ch_addr[] = {0x01, 0x06, 0x07, 0xD0, 0x00, 0x02, 0x08, 0x86}; // command pro nastaveni adresy 2 z puvodni 1
const byte ch_spd_2400[] = {0x01, 0x06, 0x07, 0xD1, 0x00, 0x00, 0xD8, 0x87}; // command pro nastaveni rychlosti komunikace na 2400b adresa senzoru 1
const byte ch_spd_4800[] = {0x01, 0x06, 0x07, 0xD1, 0x00, 0x01, 0x19, 0x47}; // command pro nastaveni rychlosti komunikace na 4800b adresa senzoru 1
const byte ch_spd_9600[] = {0x01, 0x06, 0x07, 0xD1, 0x00, 0x02, 0x59, 0x46}; // command pro nastaveni rychlosti komunikace na 9600b adresa senzoru 1
const byte inq_er[] = {0xFF, 0x03, 0x00, 0x00, 0x00, 0x06, 0xD0, 0x16};  // emergency dotaz, kdyz neni znama adresa. musi byt pripojen jen jeden senzor

//const byte ch_addr2[] = {0x01, 0x06, 0x07, 0xD0, 0x00, 0x02}; // command pro nastaveni adresy 2 bez crc

void setup(){
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
    
  ser485.begin(4800);
  Serial.begin(115200);

  //Serial.println(calcCRC(ch_addr2, sizeof(ch_addr2)), HEX);  // vypocet crc
  //ser485.write(ch_addr, sizeof(ch_addr));  // nastaveni adresy snimace

  delay(1000);
}

void loop(){
  //while (Serial.available() > 0) {
  //  String comm = Serial.readStringUntil('\n');
  //  if(comm == "print"){
      getData(inq1, sizeof(inq1));
      delay(50);
      Serial.println();
      getData(inq2, sizeof(inq2));
      Serial.println("..............................");
  //  }
  //}
  //Serial.println();
  delay(1000);
}

void getData(byte d[], int i){
  byte values[17];
  byte crc[2];
  byte data[15];
  
  if(ser485.write(d, i) == 8){
    for(uint8_t i=0; i<17; i++){
      values[i] = ser485.read();
      if(i < 15){
        data[i] = values[i];
        //Serial.print(values[i], HEX);
        //Serial.print(" ");
      }else{
        if (i == 15) crc[0] = values[i];
        if (i == 16) crc[1] = values[i];
      }
    }
    //Serial.println();
  }else{
    Serial.println("NO DATA");
    return;
  }

  if (values[0] == 255){
    Serial.println("Sensor Not Found!");
    return;
  }

  if ((((uint16_t)crc[1] << 8) | (uint16_t)crc[0]) != calcCRC(data, sizeof(data))){
    Serial.print("Sensor ID: ");
    Serial.print(values[0]);
    Serial.println(": CRC FAIL!");
    return;
  }

  int16_t val;

  Serial.print("Sensor ID: ");
  Serial.println(values[0]);
  Serial.println("------------------------");
                          
  //0 index 3     moisture content
  val = (values[3] << 8) + values[4];
  Serial.print( "Humidity....: " ); Serial.print( (float)val/10.0, 1 ); Serial.println("%");
  
  //1 index 5     temperature
  val = (values[5] << 8) + values[6];
  Serial.print( "Temperature.: " ); Serial.print( (float)val/10.0, 1 ); Serial.println("°C");

  //2 index 7     conductivity
  val = (values[7] << 8) + values[8];
  Serial.print( "Conductivity: " ); Serial.print( val ); Serial.println("us/cm");

  //3 index 9     PH
  val = (values[9] << 8) + values[10];
  Serial.print( "pH..........: " ); Serial.print( (float)val/10.0 ); Serial.println("");

  //4 index 11    nitrogen content
  val = (values[11] << 8) + values[12];
  Serial.print( "Nitrogen....: " ); Serial.print( val ); Serial.println("ul");
  
  //5 index 13    phosphorus content
  val = (values[13] << 8) + values[14];
  Serial.print( "Phosphorus..: " ); Serial.print( val ); Serial.println("ul");
}

unsigned short calcCRC(const unsigned char *indata, int len){
  unsigned short crc=0xffff;
  unsigned char temp;
  int n;
  while(len--){
    crc=*indata^crc;
    for(n=0;n<8;n++){
      char TT;
      TT=crc&1;
      crc=crc>>1;
      crc=crc&0x7fff;
      if (TT==1){
        crc=crc^0xa001;
        crc=crc&0xffff;
      }
    }
    indata++;
  }
  return crc;
}
