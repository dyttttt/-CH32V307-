#include <WiFi.h>
#include <WebSocketsServer.h>

#define WARM_YELLOW_PIN 12
#define PINK_PIN 13
#define SKY_BLUE_PIN 14


const char* ssid = "ESP32-Control";      
const char* password = "12345678";       


WebSocketsServer webSocket(81);
void setup() {
  pinMode(WARM_YELLOW_PIN, OUTPUT);
  pinMode(PINK_PIN, OUTPUT);
  pinMode(SKY_BLUE_PIN, OUTPUT);
  Serial.begin(115200);


  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); 
  
  WiFi.begin(ssid, password);
  Serial.println("正在连接热点...");
  
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 15000) { 
    delay(500);
    Serial.print(".");
  }
 

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi连接成功");
    Serial.print("IP地址: ");
    Serial.println(WiFi.localIP()); 
    
    // ↓↓↓ 修改3：启动WebSocket服务 ↓↓↓
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("WebSocket服务已启动,端口:81");
    // ↑↑↑ 修改3结束 ↑↑↑
  } else {
    Serial.println("\n连接失败,请检查:");
    Serial.println("1. 热点是否开启 2. 密码是否正确 3. 手机是否关闭了流量共享");
  }
}

void loop() {
  webSocket.loop();
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = (char*)payload;
    message.trim();
    
    if (message == "yellow") {
      digitalWrite(WARM_YELLOW_PIN, !digitalRead(WARM_YELLOW_PIN));
      webSocket.sendTXT(num, "yellow_status:" + String(digitalRead(WARM_YELLOW_PIN)));
      Serial.println("黄灯状态切换");
    }
    else if (message == "pink") {
      digitalWrite(PINK_PIN, !digitalRead(PINK_PIN));
      webSocket.sendTXT(num, "pink_status:" + String(digitalRead(PINK_PIN)));
      Serial.println("粉灯状态切换");
    }
    else if (message == "blue") {
      digitalWrite(SKY_BLUE_PIN, !digitalRead(SKY_BLUE_PIN));
      webSocket.sendTXT(num, "blue_status:" + String(digitalRead(SKY_BLUE_PIN)));
      Serial.println("蓝灯状态切换");
    }
    else {
      webSocket.sendTXT(num, "error:invalid_cmd");
      Serial.println("收到未知指令: " + message);
    }
  }
}
