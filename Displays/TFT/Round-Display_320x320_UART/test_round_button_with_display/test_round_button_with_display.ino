char readVersion[5] = {0xAA, 0x06, 0x01, 0x00};

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 4, 5);
  Serial1.setTimeout(50);
}

void loop() {
  String buff;
  String comm;
  for (int i = 1; i < sizeof(readVersion)-1; i++){
    comm += readVersion[i];
  }
  char crccomm = crcCalc(comm);
  readVersion[sizeof(readVersion)-1] = crccomm;
  
  if(Serial1.write(readVersion, sizeof(readVersion)) == 5){
    delay(50);
    if (Serial1.available()) {
      buff = Serial1.readStringUntil('\r').substring(5);
      Serial.println(buff);
      for (int i = 0; i < buff.length(); i++){
        Serial.print(buff[i], HEX);
      }
      Serial.println();
      //Serial.write(Serial1.read());
  
      String datain = buff.substring(1);
      datain = datain.substring(0, datain.length() - 1);
  
      for (int i = 0; i < datain.length(); i++){
        if (datain[i] < 16) Serial.print(0);
        Serial.print(datain[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
  
      char crc = crcCalc(datain);
      if (crc < 16) Serial.print(0);
      Serial.println(crc, HEX);
    }

  }

  delay(1000);
}

char crcCalc(String d){
  char crc;
  for (int i = 0; i < d.length(); i++){
    crc += d[i];
  }
  crc = crc^0xFF;
  return crc;
}
