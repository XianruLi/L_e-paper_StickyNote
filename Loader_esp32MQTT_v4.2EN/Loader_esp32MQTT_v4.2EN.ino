/*
 * 远程便利贴 - 英文测试版 
 * 目的：验证微雪 4.2寸 V2 屏幕能否点亮，以及 MQTT 能否接收
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <GxEPD2_BW.h>
// #include <Adafruit_GFX.h> // 必须引用这个
#include <SPI.h>

// ================= 1. 引脚定义 (微雪驱动板) =================
#define EPD_SCK  13
#define EPD_DIN  14
#define EPD_CS   15
#define EPD_BUSY 25
#define EPD_RST  26
#define EPD_DC   27

// ================= 2. 屏幕实例化 (针对 V2 版本) =================

GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));
// ================= 3. 网络配置 =================
const char* ssid        = "HUAWEI-401";          
const char* password    = "ailai123456789";      
const char* mqtt_server = "broker.emqx.io";
const char* mqtt_topic  = "XianruLi_sticky_note"; 

WiFiClient espClient;
PubSubClient client(espClient);

// ================= 4. 屏幕刷新 (使用英文自带字体) =================
void updateScreen(String msg) {
  Serial.println(">>> 开始刷新屏幕");
  
  // 强制指定 SPI 引脚
  SPI.end();
  SPI.begin(EPD_SCK, -1, EPD_DIN, EPD_CS);

  display.init(115200);
  
  // --- 英文显示设置 ---
  display.setRotation(0); // 0或1，根据需要旋转
  display.setTextColor(GxEPD_BLACK); // 设置黑色文字
  
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE); // 白底

    // 画框
    display.drawRect(5, 5, 390, 290, GxEPD_BLACK);
    display.drawLine(5, 40, 395, 40, GxEPD_BLACK);

    // 标题
    display.setTextSize(2); // 字号放大
    display.setCursor(10, 15);
    display.print("Note (English Only)");

    // 正文
    display.setTextSize(2); // 字体大小: 1=小, 2=中, 3=大
    display.setCursor(10, 60);
    // 简单的英文自动换行依靠 GFX 库自带功能
    display.print(msg); 
    
    // 底部签名
    display.setTextSize(1);
    display.setCursor(250, 280);
    display.print("Device: ESP32 V2");

  } while (display.nextPage());
  
  display.powerOff(); 
  Serial.println("<<< 刷新完成");
}

// ================= 5. MQTT 回调 =================
void callback(char* topic, byte* payload, unsigned int length) {
  String incoming = "";
  Serial.print("收到消息: ");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  Serial.println(incoming);
  updateScreen(incoming);
}

// ================= 6. 连接与主程序 =================
void setup_wifi() {
  delay(10);
  Serial.print("连接 WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("连接 MQTT...");
    String clientId = "ESP32-Note-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("成功!");
      client.subscribe(mqtt_topic);
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  SPI.begin(EPD_SCK, -1, EPD_DIN, EPD_CS); // 初始化 SPI

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // 开机测试
  updateScreen("System Ready.\nWaiting for msg...");
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
}