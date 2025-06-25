/*
* This example code is used for LaskaKit ESPD-3.5 ESP32 3.5 TFT ILI9488 Touch v3.0 https://www.laskakit.cz/laskakit-esp32-s3-devkit/
* with I2S decoder wit I2S DAC and amplifier MAX98357A on board
* ESPD-3.5 board reads stream from internet radio and playing on speaker.
*
* Board:    LaskaKit ESP32-S3-DEVKit:           https://www.laskakit.cz/laskakit-esp32-s3-devkit
* Module:   MAX98357 I2S mono amplifier 3W:     https://www.laskakit.cz/max98357-i2s-mono-zesilovac-3w/
* Speaker:  Speaker 3W 4Ω 40mm:             https://www.laskakit.cz/reproduktor-3w-4-40mm/
* laskakit.cz 2025
*
* Libraries:  Use last library version on https://github.com/schreibfaul1/ESP32-audioI2S
*             Do not use library from library manager, it will not compile
*/
#include "Arduino.h"
#include "WiFi.h"
#include "Audio.h"

#define I2S_LRC   39
#define I2S_DOUT  40
#define I2S_BCLK  41

Audio audio;
// Replace with your network credentials
const char* ssid = "xxxx";
const char* password = "xxxx";

void setup() {
      
    Serial.begin(115200);

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to WiFi ssid: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    int i = 0;
    int a = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        a++;
        i++;
        if (i == 10) {
            i = 0;
            Serial.println(".");
        } else {
            Serial.print("."); 
        }
    }
    Serial.println("Wi-Fi connected successfully");

    delay(1500);
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(30);
    //audio.connecttohost("http://www.wdr.de/wdrlive/media/einslive.m3u");
    audio.connecttohost("http://somafm.com/wma128/missioncontrol.asx"); //  asx
    //audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac"); //  128k aac
    //audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3"); //  128k mp3
}
void loop(){
    vTaskDelay(1);
    audio.loop();
    if(Serial.available()){ // put streamURL in serial monitor
        audio.stopSong();
        String r=Serial.readString(); r.trim();
        if(r.length()>5) audio.connecttohost(r.c_str());
        log_i("free heap=%i", ESP.getFreeHeap());
    }
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}