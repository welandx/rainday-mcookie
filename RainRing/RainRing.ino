/*************网络连接**************/
#define SSID        "HP" //改为你的热点名称, 不要有中文
#define PASSWORD    "88888888"//改为你的WiFi密码Wi-
#define DEVICEID    "562203353" //OneNet上的设备ID
#define HOST_NAME   "api.heclouds.com"
#define HOST_PORT   (80)
String apiKey = "K6LyVQuXzSb19S7z026neCXxfAY=";//与你的设备绑定的APIKey
/*************传感器与蜂鸣器*********/
#define PIR_PIN D6
#define buzzer_pin D8 //定义蜂鸣器驱动引脚
#define buzzer_fre 400 //定义蜂鸣器输出频率
/*****************************************/
#include <Wire.h>                                  //调用库  
#include <ESP8266.h>    
#include <SoftwareSerial.h>
#define EspSerial mySerial
#define UARTSPEED  9600
SoftwareSerial mySerial(2, 3); /* RX:D3, TX:D2 */
ESP8266 wifi(&EspSerial);
//ESP8266 wifi(Serial1);                                      //定义一个ESP8266（wifi）的对象
unsigned long net_time1 = millis();                          //数据上传服务器时间
String postString;                                //用于存储发送数据的字符串
void setup(void)     //初始化函数  
{       
  //初始化串口波特率  
    Wire.begin();
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
    Serial.print(F("setup begin\r\n"));
    delay(100);
  pinMode(PIR_PIN, INPUT);   //PIR传感器接口
  pinMode(buzzer_pin, OUTPUT);   //蜂鸣器接口

  WifiInit(EspSerial, UARTSPEED);

  Serial.print(F("FW Version:"));
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print(F("to station + softap ok\r\n"));
  } else {
    Serial.print(F("to station + softap err\r\n"));
  }

  if (wifi.joinAP(SSID, PASSWORD)) {
    Serial.print(F("Join AP success\r\n"));

    Serial.print(F("IP:"));
    Serial.println( wifi.getLocalIP().c_str());
  } else {
    Serial.print(F("Join AP failure\r\n"));
  }

  if (wifi.disableMUX()) {
    Serial.print(F("single ok\r\n"));
  } else {
    Serial.print(F("single err\r\n"));
  }

  Serial.print(F("setup end\r\n"));
    
  
}

  void loop()

{

   uint8_t buffer[560] = {0};

 

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) //创建tcp连接

  {

    Serial.print(F("create tcpok\r\n"));

  }

  else

  {

    Serial.print(F("create tcperr\r\n"));

  }

 

  postString = "GET /devices/";      //根据数据类型修改格式，此处为GET类型，意思是向onenet发送一个消息，告诉他你想做什么，此处为get，即为http get

  postString += DEVICEID;  //设备ID号

  postString +="/datapoints?datastream_id=datastreams&limit=1 "; //数据流选择为datastreams,且限制获取的数据数目为1

  postString += "HTTP/1.1";

  postString += "\r\n";

  postString += "api-key:";

  postString += apiKey; //产品的API key值

  postString += "\r\n";

  postString +="Host:api.heclouds.com\r\n";

  postString +="Connection:close\r\n\r\n";

  //此处connection:close在调试程序时，发现是断开了长连接，所以此后如果还有发送或接受数据，就需要再建立tcp长连接

  const char *postArray =postString.c_str();    //用指针指向数据

  /*json 要求输入的是char *类型。而如果你的是 string类型，那么需要通过toCharArray()的函数进行转换，而不能使用 c_str(),因为 c_str（）返回的是 const类型*/

  wifi.send((const uint8_t*)postArray,strlen(postArray)); //发送数据

  Serial.println(postArray);

  Serial.println(F("Send Success!"));

  postArray = NULL;

  uint32_t len = wifi.recv(buffer,sizeof(buffer), 10000);//接受从WiFi回复的信息至数组 buffer中，注：10000--timeout，为设置的响应时间！并返回信息的长度

 

 if (len > 0)

  {

    Serial.print("Received:[");

    for (uint32_t i = 0; i < len; i++) {

      Serial.print((char)buffer[i]);



    }

    Serial.print("]\r\n");//在串口打印收到的信息，转换为char类型

  }


  delay(200);
  if (((char)buffer[270]== '1'||(char)buffer[271]=='1'||(char)buffer[272]=='1'||(char)buffer[273]=='1'||(char)buffer[274]=='1')&&digitalRead(PIR_PIN))  //用于判断value='1' （注意，必须是'1',不能是1，因为类型不一样）与人体红外发生变化
  {  
    
    tone(buzzer_pin, buzzer_fre);   //驱动蜂鸣器
    delay(100);   
    }
    else{
     noTone(buzzer_pin);//蜂鸣器不响


}

}

