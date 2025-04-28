/* Test code for Multifunctional soil probe ZTS-3000-TR-WS-N01, temperature and humidity RS485
 * 
 * Board:   LaskaKit ESPlan ESP32 LAN8720A RS485 PoE        https://www.laskakit.cz/laskakit-espink-esp32-e-paper-pcb-antenna/
 * Sensor: Multifunctional soil probe ZTS-3000-TR-WS-N01, 
 *          temperature and humidity RS485                  https://www.laskakit.cz/multifunkcni-pudni-sonda-zts-3000-tr-ws-n01--teplota-a-vlhkost-rs485/
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

//#define DEBUG

byte getMoiTemp[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02};

void setup() {
  Serial.begin(115200);
  Serial1.begin(4800, SERIAL_8N1, 36, 4);

}

void loop() {
  byte rcv[20];
  getData(getMoiTemp, sizeof(getMoiTemp), rcv);

#ifdef DEBUG
  Serial.print("RCV data: ");
  for (int ii=0; ii<9; ii++){
    char buffi[2];
    sprintf(buffi, "%02X", rcv[ii]);
    Serial.print(buffi);
    Serial.print(" "); 
  }
  Serial.println();
#endif

  float moi;
  moi = ((rcv[3] << 8) | rcv[4]) / 10.0;

  float temp;
  temp = ((rcv[5] << 8) | rcv[6]) / 10.0;

  Serial.print("Vlhkost: ");
  Serial.print(moi);
  Serial.print("% Teplota: ");
  Serial.print(temp);
  Serial.println("°C");
  delay(2000);
}

void getData(byte d[], int i, byte rcv[]){
  byte crc[2];
  byte dat[5];
  uint16_t crcc = calcCRC(d, i);
  uint8_t crc_l = (uint8_t)(crcc >> 8);
  uint8_t crc_h = (uint8_t)crcc;
  d[6] = crc_h;
  d[7] = crc_l;

  i = i + 2;

#ifdef DEBUG
  Serial.print("Send cmd: ");
  for (int ii=0; ii<i; ii++) {
    char buffo[2];
    sprintf(buffo, "%02X", d[ii]);
    Serial.print(buffo);
    Serial.print(" ");
  }
  Serial.println();
#endif

  if(Serial1.write(d, i) == 8){
    delay(50);
    if (Serial1.available()) {
      int y = 0;
      while(Serial1.available() > 0){
        rcv[y] = Serial1.read();
        y++;
      }
    }else{
      Serial.println("NO DATA");
      return;
    }
  }
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
