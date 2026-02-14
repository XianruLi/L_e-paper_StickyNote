/*
 * 远程中文便利贴 - 终极优雅版
 * 核心修复：font_data.cpp (C++) 取代 font_data.c (C)
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <GxEPD2_BW.h>
#include <SPI.h>
#include <U8g2_for_Adafruit_GFX.h> // 只引用桥梁库

// --- 引用本地字体 ---
// 因为 font_data.cpp 已经是 C++ 了，所以这里直接 extern 就行，不需要特殊处理
extern const uint8_t u8g2_font_wqy16_t_gb2312[]; 


// ================= 1. 引脚定义 =================
#define EPD_SCK  13
#define EPD_DIN  14
#define EPD_CS   15
#define EPD_BUSY 25
#define EPD_RST  26
#define EPD_DC   27

// ================= 2. 实例化 =================
GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts; 

// ================= 3. 网络配置 =================
const char* ssid        = "HUAWEI-401";          
const char* password    = "ailai123456789";      
const char* mqtt_server = "broker.emqx.io";
const char* mqtt_topic  = "XianruLi_sticky_note"; 

WiFiClient espClient;
PubSubClient client(espClient);

// ================= 4. 屏幕刷新 =================
void updateScreen(String msg) {
  Serial.println(">>> 开始刷新屏幕");
  
  SPI.end();
  SPI.begin(EPD_SCK, -1, EPD_DIN, EPD_CS);

  display.init(115200);
  
  u8g2Fonts.begin(display); 
  u8g2Fonts.setFontMode(1);                 
  u8g2Fonts.setFontDirection(0);            
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  
  // 使用本地引用的字体
  u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312); 

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE); 
    
    // 框画
    display.drawRect(5, 5, 390, 290, GxEPD_BLACK);
    display.drawLine(5, 35, 395, 35, GxEPD_BLACK);
    
    // 标题
    u8g2Fonts.setFont(u8g2_font_wqy16_t_gb2312);
    u8g2Fonts.setCursor(15, 25); 
    u8g2Fonts.print("来自李贤儒的留言:");
    
// 2. 正文 (智能换行对齐版)
    // ===================================
    int leftMargin = 20;  // 左边距
    int currentY = 55;    // 起始 Y 坐标
    int lineHeight = 20;  // 行高 (16号字体建议设20，留点空隙)
    
    u8g2Fonts.setCursor(leftMargin, currentY);

    // 逐字打印，遇到换行符就手动归位
    for (int i = 0; i < msg.length(); i++) {
      char c = msg[i];
      
      if (c == '\n') {
        // 如果是换行符：
        currentY += lineHeight;            // 1. Y 往下移
        u8g2Fonts.setCursor(leftMargin, currentY); // 2. X 强行回到 20 (而不是 0)
      } else {
        // 如果是普通文字：
        u8g2Fonts.print(c);
      }
    }
    

  } while (display.nextPage());
  
  display.powerOff(); 
  Serial.println("<<< 刷新完成");
}

void callback(char* topic, byte* payload, unsigned int length) {
  String incoming = "";
  Serial.print("收到消息: ");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  Serial.println(incoming);
  updateScreen(incoming);
}

void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32-Note-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      client.subscribe(mqtt_topic);
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin(EPD_SCK, -1, EPD_DIN, EPD_CS);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  updateScreen("系统启动成功！\n等待中文消息...");
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}