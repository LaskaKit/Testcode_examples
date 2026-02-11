#include "IP5306_I2C.h"

#define I2C_SDA   42
#define I2C_SCL   2
#define PIN_ON    47


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  while (!Serial)
    delay(10);     // will pause Zero, Leonardo, etc until serial console opens  
  Serial.println("enter setup");
  
  pinMode(PIN_ON, OUTPUT);      // Set EN pin for second stabilisator as output
  digitalWrite(PIN_ON, HIGH);   // Turn on the second stabilisator
  
  delay(500);
  // Bring up I2C at 100 kHz during bring-up (you can raise later)
  Wire.begin(I2C_SDA, I2C_SCL, 100000);

    delay(500);

  IP5306 ip5306(I2C_SDA,I2C_SCL);  //create instance
 
  //set battery voltage
  ip5306.set_battery_voltage(BATT_VOLTAGE_0);   //4.2V

  //Voltage Vout for charging
  ip5306.charger_under_voltage(VOUT_5);         //4.7V
  
  //set charging complete current
  ip5306.end_charge_current(CURRENT_400);       //400mA
  
  //set cutoff voltage
  ip5306.set_charging_stop_voltage(CUT_OFF_VOLTAGE_3);    // 4.2/4.305/4.35/4.395  V   
  
  //set light load shutdown time
  ip5306.set_light_load_shutdown_time(SHUTDOWN_64s);      //64s

  //enable low battery shutdown mode
  ip5306.low_battery_shutdown(ENABLE);

  //allow boost even after removing Vin
  ip5306.boost_after_vin(ENABLE);

  //allow auto power on after load detection
  ip5306.power_on_load(ENABLE);

  //enable boost mode
  ip5306.boost_mode(ENABLE);

  Serial.println("booster on");
  delay(10000);


  uint8_t read_reg=0;

  
  read_reg=ip5306.i2c_read(IP5306_ADDRESS,SYS_CTL0);
  Serial.println(read_reg);
  Serial.println("\n");

  delay(500);

}

void loop() {
  // put your main code here, to run repeatedly:

}
