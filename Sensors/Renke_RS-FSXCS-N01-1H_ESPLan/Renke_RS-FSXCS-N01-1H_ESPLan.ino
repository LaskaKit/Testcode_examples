//   
/* Test code for Renke RS-FSXCS-N01-1H Ultrasonic wind speed and direction, temperature, humidity sensor
 *           with LaskaKit ESPlan ESP32 LAN8720A RS485 PoE
 * 
 * Board:   LaskaKit ESPlan ESP32 LAN8720A RS485 PoE                        https://www.laskakit.cz/laskakit-esplan-esp32-lan8720a-max485-poe
 * Sensor:  Renke RS-FSXCS-N01-1H Ultrasonic wind speed and direction, 
 *          temperature, humidity sensor                                    ttps://www.laskakit.cz/renke-rs-fsxcs-n01-1h-ultrazvukovy-senzor-rychlosti-a-smeru-vetru-teploty--vlhkosti/ 
 * 
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */
#define DEBUG
#define SENSOR_ADDR 0x01

byte get_all_data[8] = {SENSOR_ADDR, 0x03, 0x01, 0xF4, 0x00, 0x0F};

void setup() {
  Serial.begin(115200);
  Serial1.begin(4800, SERIAL_8N1, 36, 4);
}

void loop() {
  byte rcvd[40];
  int c = RS485_transaction(get_all_data, sizeof(get_all_data), rcvd);

  if (c == 0){
    Serial.println("Nebyla prijata zadna data!");
  }else if (c == -2){
    Serial.println("Neplatny kontrolni soucet!");
  }else if (c == -1){
    Serial.println("Nebyla prijata zadna data!");
  }else{

#ifdef DEBUG
    debug_print(rcvd, c, 0);
#endif

    float wspd;
    wspd = ((rcvd[3] << 8) | rcvd[4]) / 10.0;

    float wforce;
    wforce = ((rcvd[5] << 8) | rcvd[6]) / 10.0;

    int wdir;
    wdir = (rcvd[7] << 8) | rcvd[8];
    //wdir = rcvd[7];

    int wdir2;
    wdir2 = (rcvd[9] << 8) | rcvd[10];
    //wdir2 = rcvd[8];

    float humidity;
    humidity = ((rcvd[11] << 8) | rcvd[12]) / 10.0;

    float temp;
    temp = ((rcvd[13] << 8) | rcvd[14]) / 10.0;

    float noise;
    noise = ((rcvd[15] << 8) | rcvd[16]) / 10.0;

    float pm25;
    pm25 = ((rcvd[17] << 8) | rcvd[18]) / 10.0;

    float pm10;
    pm10 = ((rcvd[19] << 8) | rcvd[20]) / 10.0;

    float pressure;
    pressure = ((rcvd[21] << 8) | rcvd[22]) / 10.0;

    float lux20W_h;
    lux20W_h = ((rcvd[23] << 8) | rcvd[24]) / 10.0;

    float lux20W_l;
    lux20W_l = ((rcvd[25] << 8) | rcvd[26]) / 10.0;

    float lux20W;
    lux20W = ((rcvd[27] << 8) | rcvd[28]) / 10.0;

    float rainfall;
    rainfall = ((rcvd[29] << 8) | rcvd[30]) / 10.0;

    float radiation;
    radiation = ((rcvd[31] << 8) | rcvd[32]) / 10.0;

    Serial.printf("Rychlost větru: %.1fm/s\r\n", wspd);
    Serial.printf("Síla větru: %.1f\r\n", wforce);
    Serial.printf("Směr větru(gear): %d\r\n", wdir);
    Serial.printf("Směr větru(angle): %d°\r\n", wdir2);
    Serial.printf("Vlhkost: %.1f%%RH\r\n", humidity);
    Serial.printf("Teplota: %.1f°C\r\n", temp);
    Serial.println("-----------------");  // nasleduji veliciny podle konkretni vybavenosti senzoru. podle vybavenosti lze nepouzivane hodnoty vynechat
    Serial.printf("Hluk: %.1f\r\n", noise);  // pokud neni vybaven senzorem hluku, vraci nejnizsi hodnotu 30.0
    Serial.printf("PM2.5: %.1fμg/m3\r\n", pm25);
    Serial.printf("PM10: %.1fμg/m3\r\n", pm10);
    Serial.printf("Tlak: %.1fkPa\r\n", pressure);
    Serial.printf("Lux 20W High: %.1f\r\n", lux20W_h);
    Serial.printf("Lux 20W Low: %.1f\r\n", lux20W_l);
    Serial.printf("Lux 20W: %.1fLux\r\n", lux20W);
    Serial.printf("Srážky: %.1fmm\r\n", lux20W_h);
    Serial.println();
  }
  delay(1000);
}

int RS485_transaction(byte d[], int i, byte rcv[]){
  byte crc[2];

  uint16_t crcc = calcCRC(d, i-2);
  uint8_t crc_l = (uint8_t)(crcc >> 8);
  uint8_t crc_h = (uint8_t)crcc;
  d[6] = crc_h;
  d[7] = crc_l;

#ifdef DEBUG
  debug_print(d, i, 1);
#endif

  if(Serial1.write(d, i) == 8){
      int y = 0;
      while(Serial1.available() > 0){
        rcv[y] = Serial1.read();
        y++;
      }

      if (y == 0) return -1;

      if (y >= 8){
        if (((((uint8_t)rcv[y-1] << 8) | (uint8_t)rcv[y-2]) != calcCRC(rcv, y-2))) return -2;
      }else{
        return -3;
      }

      return y;
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

void debug_print(byte data[], int i, int dir){
  if (dir){
    Serial.print("Send cmd: ");
  }else{
    Serial.printf("Rcv data: (%dBytes) ", i);
  }

  for (int ii=0; ii<i; ii++) {
    char buffo[2];
    sprintf(buffo, "%02X", data[ii]);
    Serial.print(buffo);
    Serial.print(" ");
  }
  Serial.println();
}
