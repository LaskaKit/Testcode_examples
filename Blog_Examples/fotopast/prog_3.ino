#include <WiFiClientSecure.h>
#include <Wire.h>
#include "esp_camera.h"
#include "wifikeys.h"
#include "SSD1306.h"
#include "Base64.h"

#define PWDN_GPIO_NUM   	-1
#define RESET_GPIO_NUM  	-1
#define XCLK_GPIO_NUM   	4
#define SIOD_GPIO_NUM   	18
#define SIOC_GPIO_NUM   	23

#define Y9_GPIO_NUM     	36
#define Y8_GPIO_NUM     	37
#define Y7_GPIO_NUM     	38
#define Y6_GPIO_NUM     	39
#define Y5_GPIO_NUM     	35
#define Y4_GPIO_NUM     	14
#define Y3_GPIO_NUM     	13
#define Y2_GPIO_NUM     	34
#define VSYNC_GPIO_NUM  	5
#define HREF_GPIO_NUM   	27
#define PCLK_GPIO_NUM   	25

#define I2C_SDA         	21
#define I2C_SCL         	22

#define SSD1306_MODE_TYPE   0   // 0 : GEOMETRY_128_64  // 1: GEOMETRY_128_32
#define SSD1306_ADDRESS 	0x3c

#define AS312_PIN       	19

// Promenne pro pouzite při odesilani dat na server
const char* myDomain = "script.google.com";
String myScript = "/macros/s/xxxxxxxxxxx/exec";	//Sem se místo xxx vlozi kod google skriptu
String myFilename = "filename=capture.jpg";
String mimeType = "&mimetype=image/jpeg";
String myImage = "&data=";

int waitingTime = 30000;  // Doba pro odeslani obrazku na server, pokud bude pomale pripojeni, lze v pripade problemu s prenosem zvetsit

SSD1306 oled(SSD1306_ADDRESS, I2C_SDA, I2C_SCL, (OLEDDISPLAY_GEOMETRY)SSD1306_MODE_TYPE);

bool deviceProbe(uint8_t addr){  // Testuje, zda je pritomno I2C zarizeni na dane adrese
    Wire.beginTransmission(addr);
    return Wire.endTransmission() == 0;
}

bool setupDisplay(){  // Nastaveni displeje
    if (!deviceProbe(SSD1306_ADDRESS))return false;
    oled.init();
    return true;
}

bool setupCamera(){  // Nastaveni kamery
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    if (psramFound()) {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return false;
    }

    sensor_t *s = esp_camera_sensor_get();
    if (s->id.PID == OV3660_PID) {
        s->set_vflip(s, 1);
        s->set_brightness(s, 1);
        s->set_saturation(s, -2);
    }

    s->set_framesize(s, FRAMESIZE_QVGA);

    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);

    return true;
}

void setupNetwork(){  // Nastaveni a pripojeni k WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
}

bool saveCapturedImage() {  // Vyfoceni a odeslani obrazku na server
    Serial.println("Connect to " + String(myDomain));
    WiFiClientSecure client;

    if (client.connect(myDomain, 443)) {
        Serial.println("Connection successful");

        camera_fb_t * fb = NULL;
        fb = esp_camera_fb_get();
        if(!fb) {
            Serial.println("Camera capture failed");
            return false;
        }

        char *input = (char *)fb->buf;
        char output[base64_enc_len(3)];
        String imageFile = "";
        for (int i=0;i<fb->len;i++) {
            base64_encode(output, (input++), 3);
            if (i%3==0) imageFile += urlencode(String(output));
        }
        String Data = myFilename+mimeType+myImage;

        esp_camera_fb_return(fb);

        Serial.println("Send a captured image to Google Drive.");

        client.println("POST " + myScript + " HTTP/1.1");
        client.println("Host: " + String(myDomain));
        client.println("Content-Length: " + String(Data.length()+imageFile.length()));
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.println();

        client.print(Data);
        int Index;
        for (Index = 0; Index < imageFile.length(); Index = Index+1000) {
            client.print(imageFile.substring(Index, Index+1000));
        }

        Serial.println("Waiting for response.");
        long int StartTime=millis();
        while (!client.available()) {
            Serial.print(".");
            delay(100);
            if ((StartTime+waitingTime) < millis()) {
                Serial.println();
                Serial.println("No response.");
                //If you have no response, maybe need a greater value of waitingTime
                break;
            }
        }
        Serial.println();

        if (client.find("302")){
            Serial.println("Data send OK.");
        }else{
            Serial.println("Data send FAIL!");
            return false;
        }
    } else {
        Serial.println("Connected to " + String(myDomain) + " failed.");
    }
    client.stop();

    return true;
}

// Prevod binarniho obrazku na textovou variantu pro posilani na webove stranky
//https://github.com/zenmanenergy/ESP8266-Arduino-Examples/
String urlencode(String str){
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
        if (c == ' '){
            encodedString+= '+';
        } else if (isalnum(c)){
            encodedString+=c;
        } else{
            code1=(c & 0xf)+'0';
            if ((c & 0xf) >9){
                code1=(c & 0xf) - 10 + 'A';
            }
            c=(c>>4)&0xf;
            code0=c+'0';
            if (c > 9){
                code0=c - 10 + 'A';
            }
            code2='\0';
            encodedString+='%';
            encodedString+=code0;
            encodedString+=code1;
            //encodedString+=code2;
        }
        yield();
    }
    return encodedString;
}

void displayText(String text, int d){  // Zobrazeni textu na displeji
    int len = text.length();
    oled.clear();
    oled.drawString( oled.getWidth() / 2, oled.getHeight() / 2 - len, text);
    oled.display();

    if (d > 0){
        delay(d);
        oled.clear();
        oled.display();
    }
}

void setup(){
    Serial.begin(115200);
    Wire.begin(I2C_SDA, I2C_SCL);

    bool statusd, statusc;
    statusd = setupDisplay(); // Nastaveni displeje
    Serial.print("setupDisplay status "); (statusd)? Serial.println("OK") : Serial.println("Fail");

    statusc = setupCamera();  // Nastaveni kamery
    Serial.print("setupCamera status "); (statusc)? Serial.println("OK") : Serial.println("Fail");

    if (!statusd || !statusc) {  // Pokud se vyskytla chyba v nastaveni, zrestartuje se ESP32
        Serial.println("Initializing error, restarting...");
        delay(10000); esp_restart();
    }

    oled.setFont(ArialMT_Plain_16);
    oled.setTextAlignment(TEXT_ALIGN_CENTER);

    displayText("Starting...", 0);

    setupNetwork();  // Nastaveni a pripojeni k WiFi

    Serial.println("Ready...");
    displayText("Ready...", 1000);
}

void loop(){
    if(digitalRead(AS312_PIN)){  // Pokud je detekovan pohyb
        Serial.println("Camera capture and send...");
        displayText("Capture and send", 0);

        // Odesle se obrazek na Google
        if (saveCapturedImage()){  // Pokud odeslani obrazku probehlo v poradku
            displayText("Send OK", 1000);
        }else{
            displayText("Send FAIL!", 1000);
        }
    }
}
