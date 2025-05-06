#define CAMERA_MODEL_AI_THINKER 
#include "esp_camera.h"
#include "camera_pins.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <Wire.h>


//defineren van verschillende variabelen die verder worden gebruikt
#define FRAME_SIZE FRAMESIZE_QVGA 
#define WIDTH 320 // pixels van camera
#define HEIGHT 240 // pixels van camera
#define BLOCK_SIZE 10
#define W (WIDTH / BLOCK_SIZE) 
#define H (HEIGHT / BLOCK_SIZE)
#define BLOCK_DIFF_THRESHOLD 0.2
//define the number of blocks above which an image is changed 
#define IMAGE_DIFF_THRESHOLD 0.2 //wordt later gebruikt om bewegign te detecteren
#define DEBUG 1

/*Gegevens om met wifi te verbiden*/
const char* ssid     = "SSID"; //SSID
const char* password = "PASSWORD"; //Wifi wachtwoord

const char* serverName = "http://esp32-counter.000webhostapp.com/post-esp-data.php"; //defineren van serverName, dit is de uiteindelijke eindwebsite

String apiKeyValue = "tPmAT5Ab3j7F9"; //apiKey voor koppeling beveilingen van de koppeling naar de database, als deze niet klopt zal er geen HTTP POST plaatsvinden

/*
*/
uint16_t prev_frame[H][W] = { 0 }; //vorige frame, hiermee word het huidige frame vergeleken
uint16_t current_frame[H][W] = { 0 }; //huidige frame
int list[2]={0,0} ;  
int counter = 0 ; //de variable counter, is later het totaal aantal personen, wordt naar databae gestuurd

/* defineren van functies die worden gebruikt
 */
bool setup_camera(framesize_t);
bool capture_still();
int motion_detect();
void update_frame();
void print_frame(uint16_t frame[H][W]);
bool direction_detection ();
int freq(uint16_t frame[H][W],uint16_t a);


/* setup voert setup_camera uit en verbind met wifi.
 */
void setup() {
    Serial.begin(115200);
    Serial.println(setup_camera(FRAME_SIZE) ? "OK" : "ERR INIT");
    pinMode(12, OUTPUT);  // initialize the GPIO12 pin as an output
    pinMode(4, OUTPUT);  // initialize the GPIO4 pin as an output

    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

/* loop is de hoofd functie die elke seconde wordt uitgevoerd
   */
 
void loop() {
    /* Controlen of de camera wel een foto kan maken, als dit niet zo is komt er een foutmelding */
    if (!capture_still()) {
        Serial.println("Failed capture");      
        return;
    }
//Het tellen van de personen op basis van richting
   switch (motion_detect()){ //aanroepen van de functie motion_detect
    case 0 : //als motion_detect 0 is wordt list[1] 0
      Serial.println("");
      list[1]=0;
      digitalWrite(12, LOW);
      digitalWrite(4, LOW);
      break;  
    case 1 :  //persoon komt van de linker kant, list[1] wordt 1
      list[1]= 1;
      digitalWrite(12, HIGH);
      digitalWrite(4, LOW);
      break; 
    case -1 : //persoon komt van de recht kant, list [1] word -1
      list[1]= -1 ;
      digitalWrite(12, LOW);
      digitalWrite(4, HIGH);
      break;       
   }
   
    if ((list[0]==0) && (list[1]==1)){
       counter= counter-1 ; // als list[1] 1 is komt er een persoon bij 
    }else if ((list[0]==0) && (list[1]==-1)){
        counter=counter+1 ; // als list[1] -1 gaat er een persoon af
       }
    if (counter < 0) { //als counter een mingetal wordt, wordt counter weer 0 anders komen er mingetallen in de database.
      counter = 0;
    }     
    Serial.print(counter) ; 
    list[0]= list[1]; //the current value will be the preview value
    Serial.println("    personen in het lokaal");
    update_frame();
    request(); //roept de request functie aan, dit is de functie voor de HTTP POST   
} 

/* request voert de http POST naar de database uit.
   */
void request() {
        //Controleren of er wifi verbinding is
      if(WiFi.status()== WL_CONNECTED){
        HTTPClient http;
        
        http.begin(serverName); //eerder genoemde site
        
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        //Voorbereiden van de HTTP post, de apikey en de variabele counter wordt steeds gestuurd
        String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + counter + "";
        Serial.print("httpRequestData: "); //Print de http post is de veriale monitor ter controle van de programeur
        Serial.println(httpRequestData);
    //Verstuur HTTP post
      int httpResponseCode = http.POST(httpRequestData);
  //controleren of de HTTP post echt is verstuurd
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    
}


//de setup van de camera
bool setup_camera(framesize_t frameSize) {
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
    config.pixel_format = PIXFORMAT_GRAYSCALE;
    config.frame_size = frameSize;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    bool ok = esp_camera_init(&config) == ESP_OK;

    sensor_t *sensor = esp_camera_sensor_get();
    sensor->set_framesize(sensor, frameSize);

    return ok;
}

//Maak een foto en verdeel de foto is blokken van 10 bij 10 pixels
bool capture_still() {
    camera_fb_t *frame_buffer = esp_camera_fb_get();

    if (!frame_buffer)
        return false;

   
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            current_frame[y][x] = 0;


    //Foto wordt gedownsampeld
    for (uint32_t i = 0; i < WIDTH * HEIGHT; i++) {
        const uint16_t x = i % WIDTH;
        const uint16_t y = floor(i / WIDTH);
        const uint8_t block_x = floor(x / BLOCK_SIZE);
        const uint8_t block_y = floor(y / BLOCK_SIZE);
        const uint8_t pixel = frame_buffer->buf[i];
        const uint16_t current = current_frame[block_y][block_x];

        current_frame[block_y][block_x] += pixel;
    }
    
    // gemiddele pixel in block (rescale)
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++)
            current_frame[y][x] /= BLOCK_SIZE * BLOCK_SIZE;

    return true;
}


//functie om de richting te bapalen
bool direction_detection(uint16_t frame[H][W]){ 
  // initialiseren van beide richtingen
  uint16_t direc_left[H][W] = { 0 };
  uint16_t direc_right[H][W] = { 0 };
  
  for (int y = 0; y < H ; y++) {
      for (int x = 0; x < W; x++){
        if(x < W/2 ) {                   
          direc_left[y][x] = frame[y][x];
          direc_right[y][x] = { 0 };
        } 
        else {
           direc_right[y][x] = frame[y][x];
           direc_left[y][x] = { 0 };
        }
      } 
      }
      Serial.println("....................");
  if (freq(direc_right,{99}) > freq(direc_left,{99})) {

    return false;
    } else if (freq(direc_right,{99}) < freq(direc_left,{99})) { 

      return true; 
    } 
}



// detecteren van beweging door het verschil te bepalen van het aantal blokken
int motion_detect(){
    uint16_t changes = {0}; //kijken hoeveel blokken er zijn verandered
    const uint16_t blocks = (WIDTH * HEIGHT) / (BLOCK_SIZE * BLOCK_SIZE); //aantal blokken in de foto
    uint16_t direc[H][W] = { 0 };
    for (int y = 0; y < H; y++)
       { for (int x = 0; x < W; x++)
       {direc[y][x] = {0};}}
    //vergelijk de blokken van de huidige frame met de vorige frame en bereken de delta
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            float current = current_frame[y][x]; 
            float prev = prev_frame[y][x];
            float delta = abs(current - prev) / prev; //bereken delta
    
            if (delta >= BLOCK_DIFF_THRESHOLD) { //als delta groter of gelijk is aan BLOCK_DIFF_THRESHOLD (0.2) dan is er beweging gedetecteerd, nu moet 20% van de de huidige en vorige frame verschillen om beweging te detecteren 
                changes += 1;   
                direc[y][x] = {99} ;
 #endif               
            }
    }
    }
     if ((1.0 * changes / blocks) > IMAGE_DIFF_THRESHOLD){
      if  (direction_detection(direc)) {
          return 1;  // 1 betekent dat iemand van link komt
      } else {
        return -1 ; //-1 betekent dat iemand van recht komt
      }  
    } else {
      return 0; //0 is geen beweging 
    }
}





 //kopieer huidige frame naar de vorige
void update_frame() { 
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            prev_frame[y][x] = current_frame[y][x];
        }
    }
}


int freq(uint16_t matrix[H][W],uint16_t a) {
   int freq = 0 ; 
   for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            if (matrix[y][x] == a) {
              freq = freq +1 ;
            }
        }
    }
    return freq ;

}


void print_frame(uint16_t frame[H][W]) {
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            Serial.print(frame[y][x]);
            Serial.print('\t');
        }

        Serial.println();
    }
}
