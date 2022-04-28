#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "PubSubClient.h"


#define ssid "KimHung"  //Thay bằng wifi nhà bạn
#define password "mottoichin"
// Thông tin về MQTT Broker
#define mqtt_server "io.adafruit.com" // Thay bằng thông tin của bạn
#define mqtt_user "kimhungtdblla24"    //Thay bằng user va pass của bạn
#define mqtt_pwd "aio_UDSA14XHRvdTduRIlQnWGDlGVVah"
#define stopic "kimhungtdblla24/feeds/da-tkll-rp"
#define ptopic "kimhungtdblla24/feeds/da-tkll-rq"
const uint16_t mqtt_port = 1883; //Port của CloudMQTT

WiFiClient espClient;
PubSubClient client(espClient);

char cmdJsonRec[127]; 
String text;

DynamicJsonDocument cmd(1024); 
DynamicJsonDocument cmdRec(1024); 

char flagWaitResValue = 0;

#define INIT      0
#define PARSE_REQ 1
#define SEND_CMD  2
#define WAIT_RES  3
#define SEND_RES  4
char statusProcess = INIT;

String cmdTerminal = "!OK!";

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(9600);
  // hàm thực hiện chức năng kết nối Wifi và in ra địa chỉ IP của ESP8266
  setup_wifi();
  // cài đặt server là broker.mqtt-dashboard.com và lắng nghe client ở port 1883
  client.setServer(mqtt_server, mqtt_port);
  // gọi hàm callback để thực hiện các chức năng publish/subcribe
  client.setCallback(callback);
  // gọi hàm reconnect() để thực hiện kết nối lại với server khi bị mất kết nối
  reconnect();
}

void setup_wifi() {
  delay(10);
  // Serial.println();
  // Serial.print("Connecting to ");
  // Serial.println(ssid);
  // kết nối đến mạng Wifi
  WiFi.begin(ssid, password);
  // in ra dấu . nếu chưa kết nối được đến mạng Wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }
  // in ra thông báo đã kết nối và địa chỉ IP của ESP8266
  // Serial.println("");
  // Serial.println("WiFi connected");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {
  //in ra tên của topic và nội dung nhận được từ kênh MQTT lens đã publish
  // Serial.print("Message arrived [");
  // Serial.print(topic);
  // Serial.print("] ");
  for (int i = 0; i < length; i++) {
    cmdJsonRec[i] = (char)payload[i];
    // Serial.print(cmdJsonRec[i]);
  }
  // Serial.println();
  deserializeJson(cmd, (char*)cmdJsonRec);

  // processRequest();
  statusProcess = PARSE_REQ;

}

void processRequest()
{
  DynamicJsonDocument resp(1024); 
  
  String cmdText ;
  switch(statusProcess)
  {
    case INIT:
      flagWaitResValue = 0 ;
      break;
    case PARSE_REQ:
        cmdTerminal = "!";
        cmdTerminal += (String)cmd["cmd"];
        cmdTerminal += "-";
        cmdTerminal += (String)cmd["status"];
        if((int)cmd["paras"] >= 1 && ((String)cmd["status"] != "sch"))
            cmdTerminal += "-";
        // Parse the command
        // {"cmd":"set","check":0,"status":"normal","paras":1,"valueTime":[23,3,25,5]}
        // {cmd:"set", check:0 , status:"manual", paras:1, color:"red" }
        if((String)cmd["cmd"] == "set"){
          if((String)cmd["status"] == "normal")
          {
            cmdTerminal += (String)cmd["valueTime"];
          }
          if((String)cmd["status"] == "manual")
          {
            cmdTerminal += (String)cmd["valueColor"];
          }
        }
        //{cmd:"start", check:0 , status:"manual", paras:1, valueColor:"red" };
        if((String)cmd["cmd"] == "start"){
          if((String)cmd["status"] == "manual")
          {
            cmdTerminal += (String)cmd["valueColor"];
          }
        }
        // //{cmd:"add", check:0 , status:"sch", paras:1, valueSCH:valueAdd }
        // if((String)cmd["cmd"] == "add"){
        //   if((String)cmd["status"] == "sch")
        //   {
        //     cmdTerminal += (String)cmd["valueSCH"];
        //   }
        // }
        // //{cmd:"edit", check:0 , status:"sch", paras:1, valueSCH:valueAdd };
        // if((String)cmd["cmd"] == "edit"){
        //   if((String)cmd["status"] == "sch")
        //   {
        //     cmdTerminal += (String)cmd["valueSCH"];
        //   }
        // }
        // {cmd:"delete", check:0 , status:"sch", paras:num, index:indexSCH_Delete };
        if((String)cmd["cmd"] == "delete"){
          if((String)cmd["status"] == "sch")
          {
            cmdTerminal += (String)cmd["index"];
          }
        }
        // {cmd:"update", check:0 , status:"sch", paras:i}
        if((String)cmd["cmd"] == "update"){
          if((String)cmd["status"] == "sch")
          {
            cmdTerminal += "-";
            cmdTerminal += (String)cmd["paras"];
          }
        }
        ////////////////////////////////////////////////////////////////////////////
        cmdTerminal += "!";
        statusProcess = SEND_CMD ;
      break;
    case SEND_CMD:
      Serial.println(cmdTerminal);
      statusProcess = WAIT_RES ;
      break;
    case WAIT_RES:
      if(Serial.available()) { // nếu có dữ liệu gửi đến
      text = Serial.readStringUntil('\n');

      // Xac nhan da Update
      if(text == "!OK!")
      {
        text = "";
        flagWaitResValue = 0;
        statusProcess = SEND_RES ;  
        break;
      }
      // add-sch // edit-sch
      if(text == "!OK1!")
      {
        text = "";

        cmdTerminal = (String)cmd["valueSCH"];
        // cmdTerminal = "[0,\"12:30\",\"16:30\",35,3,30,3]";
        Serial.println(cmdTerminal);
        
        statusProcess = WAIT_RES ;  
        break;
      }

      deserializeJson(cmdRec, text);
      if((String)cmdRec["cmd"] == "update"){
        flagWaitResValue = 1 ;
        statusProcess = SEND_RES ;  
      }
      }
    break;
    case SEND_RES:
    if((String)cmd["check"] == "0") {
      resp["cmd"] = "check";
      resp["cmdCheck"] = cmd["cmd"];
      resp["status"] = cmd["status"];
      resp["check"] = 1;
    /////////////////////////////
    if(flagWaitResValue == 1)
    {
        //{"cmd":"update","check":0,"status":"status","paras":1,"valueStatus":"normal"}
      if((String)cmdRec["cmd"] == "update"){
          if((String)cmdRec["status"] == "status")
          {
            resp["valueStatus"] = cmdRec["valueStatus"];
          }
        }
    //{"cmd":"update","check":0,"status":"normal","paras":1,"valueTime":[40,3,35,3]}
      if((String)cmdRec["cmd"] == "update"){
          if((String)cmdRec["status"] == "normal")
          {
            resp["valueTime"] = cmdRec["valueTime"];
          }
        }
      //{"cmd":"update","check":0,"status":"sch","paras":1,"valueSch":[4,"12:30","14:30",40,3,35,3]}
      if((String)cmdRec["cmd"] == "update"){
          if((String)cmdRec["status"] == "sch")
          {
            resp["check"] = (int)cmdRec["check"];
            resp["valueSch"] = cmdRec["valueSch"];
          }
        }
    }
    ////////////////////////////////////////////////////////////////
      serializeJson(resp, cmdText);
      const char* cmdResp = cmdText.c_str();
      client.publish(ptopic, cmdResp);
      statusProcess = INIT;
  }
    break;
    default :
      statusProcess = INIT;
  } 
}

void sendCmdToWeb()
{
  DynamicJsonDocument reqCmd(1024); 
  if(flagWaitResValue == 0)
  {
    if(Serial.available()) { // nếu có dữ liệu gửi đến
        text = Serial.readStringUntil('\n');
        reqCmd["cmd"]="update";
        reqCmd["check"]=0;

        deserializeJson(cmdRec, text);
        reqCmd["status"]=cmdRec["status"];
        reqCmd["paras"]=cmdRec["paras"];
        // {"status":"status","paras":1,"valueStatus":"normal"}
        if((String)cmdRec["status"] == "status")
          reqCmd["valueStatus"]=cmdRec["valueStatus"];

        if((String)cmdRec["status"] == "normal")
          reqCmd["valueTime"]=cmdRec["valueTime"];

        if((String)cmdRec["status"] == "manual")
        {
          reqCmd["valueStatus"]=cmdRec["valueStatus"];
          reqCmd["valueColor"]=cmdRec["valueColor"];
        }

        if((String)cmdRec["status"] == "slow")
          reqCmd["valueStatus"]=cmdRec["valueStatus"];

        if((String)cmdRec["status"] == "sch")
        {
          reqCmd["valueMode"]=cmdRec["valueMode"];
          reqCmd["valueSch"]=cmdRec["valueSch"];
        }

        text="";
        serializeJson(reqCmd, text);
        const char* cmdReq = text.c_str();
        client.publish(ptopic, cmdReq);
      }

  }
}

void reconnect() {
  // lặp cho đến khi được kết nối trở lại
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266", mqtt_user, mqtt_pwd)) {
      Serial.println("connected");
      // publish gói tin "Connected!" đến topic ESP8266/connection/board
      client.publish(ptopic, "Connected!");
      // đăng kí nhận gói tin tại topic ESP8266/LED_GPIO2/status
      client.subscribe(stopic);
    } else {
      // in ra màn hình trạng thái của client khi không kết nối được với MQTT broker
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // delay 5s trước khi thử lại
      delay(5000);
    }
  }
}
void sendMessage(char * msg)
{
  client.publish(ptopic, msg);
}
void loop() {
  // kiểm tra nếu ESP8266 chưa kết nối được thì sẽ thực hiện kết nối lại
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  processRequest();
  //  if(Serial.available()) { // nếu có dữ liệu gửi đến
  //       text = Serial.readStringUntil('\n');
  //       Serial.print(text);
  //  }

  sendCmdToWeb();

  delay(10);
}
