#include "esp_camera.h"
#include <HTTPClient.h>
#include <base64.h>
#include <WiFi.h>
#include <ArduinoJson.h>

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15 
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
//#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
//#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD
//#define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
//#define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"

// ===========================
// Enter your WiFi credentials
// ===========================
// const char* ssid = "wcida";
// const char* password = "R3dCidWcer0WMa";
const char* ssid = "FrontinoMesh";
const char* password = "ArepaGood2023&";

void startCameraServer();
void setupLedFlash(int pin);

// Configuración de la API del servidor web
const char* serverUrl = "http://192.168.0.100:8000/api/v1/fotos/"; // Reemplaza con la URL de tu servidor web
const char* serverUrlAccesos = "http://192.168.0.100:8000/api/v1/accesos/1/"; // Reemplaza con la URL de tu servidor web

const char* serverIP = "192.168.0.100";
const int serverPort = 8000;

// Configuración del sensor de movimiento
const int motionPin = 14; // Pin del sensor de movimiento en el ESP32

// Variables globales
bool motionDetected = false;

int foto_id = 4;

void setup() {

  Serial.begin(115200);
  while(!Serial);
  delay(1000);
  Serial.println();
  Serial.print("start!");

  Serial.setDebugOutput(true);
  Serial.println();


  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  if (digitalRead(motionPin) == HIGH) {
    if (!motionDetected) {
      Serial.println("Movimiento detectado.");
      captureAndSendPhoto();
      motionDetected = true;
      buscarValorPlaca();
    }
  } else {
    motionDetected = false;
  }
}


void captureAndSendPhoto() {

  camera_fb_t *fb = NULL;

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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Error al inicializar la cámara. Código de error: 0x%x", err);
    return;
  }

  // Captura de foto
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Error al capturar la foto.");
    return;
  }

  // Envío de la foto al servidor web
  WiFiClient client;
  HTTPClient http;

  Serial.print("Conectando al servidor web: ");
  Serial.println(serverUrl);

  if (http.begin(client, serverUrl)) {
    Serial.println("Enviando foto al servidor...");

    http.addHeader("Content-Type", "application/json");

    size_t size = fb->len;
    String buffer = base64::encode((uint8_t *) fb->buf, fb->len);
    
    String imgPayload = "{\"foto\" : \"data:@file/jpg;base64, " + buffer + "\"}";
    
    int httpResponseCode = http.POST(imgPayload);

    if (httpResponseCode == 201) {
      Serial.println("Foto enviada correctamente.");
      String response = http.getString();

      // Parse the response JSON
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, response);

      if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
      } else {
        // Extract the ID value from the response JSON
        int id = doc["id"].as<int>();

        // Print the ID value
        Serial.print("ID: ");
        Serial.println(id);
        foto_id = id;
      }
      
        
    } else {
      Serial.print("Error al enviar la foto. Código de respuesta HTTP: ");
      Serial.println(httpResponseCode);
      foto_id = 400;
    }
      
    http.end();
  } else {
    Serial.println("Error al establecer la conexión con el servidor web.");
  }

  // Liberar recursos de la cámara
  esp_camera_fb_return(fb);
  esp_camera_deinit();

}

void buscarValorPlaca(){
  Serial.print("Foto id es:");
  Serial.println(foto_id);

  // Envío de la foto al servidor web
  WiFiClient client;
  HTTPClient http;
  String endPoint =  "/api/v1/accesos/";
  

  String s1 = "/api/v1/accesos/";
  String s2 = String(foto_id);
  s2.concat('/');
  s1.concat(s2);

  http.begin(client, serverIP, serverPort, s1); // Cambia "/endpoint" por la ruta correcta del endpoint

  // Realiza la solicitud GET
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200 || httpResponseCode == 301) {
    String response = http.getString();

    // Analiza la respuesta JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("Error al analizar JSON: ");
      Serial.println(error.c_str());
    } else {
      // Accede a los valores del JSON
      String placa = doc["placa"].as<String>();
      String idX = doc["id"].as<String>();

      // Imprime los valores
      Serial.print("Valor de placa: ");
      Serial.println(placa);
      Serial.print("Valor de id: ");
      Serial.println(idX);
    }
  } else {
    Serial.print("Error en la solicitud. Código de respuesta: ");
    Serial.println(httpResponseCode);
  }

  http.end();

}



