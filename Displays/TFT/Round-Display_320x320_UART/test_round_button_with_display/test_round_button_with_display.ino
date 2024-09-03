Stream *port;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 4, 5);

  if (initDisplayModule(&Serial1)){
    Serial.print("Init OK. Version ");
    Serial.println(getVer());
  }else{
    Serial.println("Init FAIL!");
  }

  delay(2000);
}

void loop() {
  if (Serial1.available()) {
    String buff = Serial1.readStringUntil('\r');
    printHex(buff);
  }
  //Serial.println();
  //delay(1000);
}

bool initDisplayModule(Stream *p){
  port = p;
  port->setTimeout(50);
  char initConn[5] = {0xAA, 0x01, 0x01, 0x01};
  String r = sendComm(initConn, sizeof(initConn));
  //Serial.println(r[0], HEX);
  
  if (r[0] == 0xFE){
    return true;
  }else{
    return false;
  }
}

String getVer(){
  char readVersion[5] = {0xAA, 0x06, 0x01, 0x00};
  return sendComm(readVersion, sizeof(readVersion)).substring(3);
}

String sendComm(char* c, int l){
  String buff;
  String comm;
  String datain;
  for (int i = 1; i < l-1; i++){
    comm += c[i];
  }
  char crccomm = crcCalc(comm);
  c[l-1] = crccomm;
  
  if(port->write(c, l) == 5){
    delay(50);
    if (port->available()) {
      buff = port->readStringUntil('\r');
      //printHex(buff);

      String respdata = buff.substring(1, 4);
      //printHex(respdata);
      //Serial.println(getRespCode(respdata));  

      if(buff.length() > 5){
        datain = buff.substring(5, buff.length());
        datain = datain.substring(0, datain.length()-1);
        //printHex(datain);
        
        char crc = crcCalc(datain);
        //if (crc < 16) Serial.print(0);
        //Serial.println(crc, HEX);
      }else{
        datain = respdata.substring(2);
      }
    }
  }
  return datain;
}

String getRespCode(String s){
  String rcode;
  String retcode = s.substring(2);

  switch (retcode[0]) {
    case 0xFE:
      rcode = "NORMAL";
      break;
    case 0xFD:
      rcode = "CHECKSUM_ERROR";
      break;
    case 0xFC:
      rcode = "NOT_SUPPORT";
      break;
    case 0xFB:
      rcode = "BUSY";
      break;
    case 0xFA:
      rcode = "SYSTEM_IS_STARTING_OR_NOT_CONNECT";
      break;
    default:
      rcode = "UNKNOWN";
      break;
  }

  return rcode;
}

void printHex(String s){
  for (int i = 0; i < s.length(); i++){
    if (s[i] < 16) Serial.print(0);
    Serial.print(s[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

char crcCalc(String d){
  char crc;
  for (int i = 0; i < d.length(); i++){
    crc += d[i];
  }
  crc = crc^0xFF;
  return crc;
}
