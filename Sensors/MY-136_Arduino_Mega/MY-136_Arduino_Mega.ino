// Arduino MEGA + MY-136

byte readAddr[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
byte readBr[] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA};

void setup(){
  Serial.begin(115200);
  Serial1.begin(9600);
  delay(1000);
  Serial.println("Starting...");
}

void loop(){
  Serial.println("Read data");
  getData(readAddr, 8);
  Serial.println("..............................");
  delay(1000);
}

void getData(byte d[], int i){
  byte rcv[7];
  byte crc[2];
  byte dat[5];

  Serial.print("Send cmd: ");
  for (int ii=0; ii<i; ii++) {
    char buff[2];
    sprintf(buff, "%02X", d[ii]);
    Serial.print(buff);
    Serial.print(" ");
  }
  Serial.println();
  
  if(Serial1.write(d, i) == 8){
    if (Serial1.available()) {
      Serial.print("RCV data: ");
      for(uint8_t y=0; y<7; y++){
        rcv[y] = Serial1.read();
        if (y == 5) crc[0] = rcv[y];
        if (y == 6) crc[1] = rcv[y];

        char buff[2];
        sprintf(buff, "%02X", rcv[y]);
        Serial.print(buff);
        Serial.print(" ");
      }
    }else{
      Serial.println("NO DATA");
      return;
    }
  }

  for (int i=0; i<(sizeof rcv)-2; i++) {
    dat[i] = rcv[i];
  }
  
  if ((((uint16_t)crc[1] << 8) | (uint16_t)crc[0]) != calcCRC(dat, sizeof(dat))){
    Serial.print(" >> ");
    Serial.print(crc[1], HEX);
    Serial.print(" ");
    Serial.print(crc[0], HEX);
    Serial.print(" = ");
    Serial.println("CRC FAIL!");
    return;
  }

  Serial.println();
  Serial.print("DATA: ");
  for (int i=0; i<sizeof dat; i++) {
    char buff[2];
    sprintf(buff, "%02X", dat[i]);
    Serial.print(buff);
    Serial.print(" ");
  }
  Serial.println();
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
