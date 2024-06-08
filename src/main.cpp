#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <AsyncWebSocket.h>
#include <HTTPClient.h>

const char* ssid = "ssid";
const char* password = "password";

const char* thingspeak_server = "http://api.thingspeak.com/update";
String apiKey = "THINGSPEAK_API_TOKEN";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

int lcdColumns = 16;
int lcdRows = 2;
int buttonPin = 19;
int gas_sensor = 34;
int co_sensor = 35;
float m = -0.353;
float c = 0.711;
float R0 = 23.30;
float m1 = -0.67;
float c1 = 1.34;
float R01 = 5.80;
int last_state = HIGH;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

float calculatePPM(int sensorValue, float R0, float m, float c);

void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Real-time Sensor Readings</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f3f3f3;
        }

        .container {
            max-width: 600px;
            margin: 20px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }

        h1 {
            text-align: center;
            color: #333;
        }

        .sensor-reading {
            margin-bottom: 20px;
            padding: 10px;
            background-color: #f9f9f9;
            border-radius: 4px;
            box-shadow: 0 0 5px rgba(0, 0, 0, 0.1);
        }

        p {
            margin: 5px 0;
            color: #666;
        }

        .status {
            display: block;
            margin-top: 5px;
        }

        .good {
            color: green;
        }

        .bad {
            color: red;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Real-time Sensor Readings</h1>
        <div class="sensor-reading" id="gas">Gas: </div>
        <div class="sensor-reading" id="co">CO: </div>
    </div>

    <script>
        var socket = new WebSocket("ws://" + window.location.host + "/ws");
        socket.onmessage = function(event) {
            var data = event.data.split(",");
            var gasValue = parseFloat(data[0]);
            var coValue = parseFloat(data[1]);

            document.getElementById("gas").innerHTML = "Gas: " + gasValue + " ppm" + "<span class='" + (gasValue > 30 ? 'bad' : 'good') + " status'>" + "Air quality: " + (gasValue > 30 ? 'Bad' : 'Good') + "</span>";
            document.getElementById("co").innerHTML = "CO: " + coValue + " ppm" + "<span class='" + (coValue > 3 ? 'bad' : 'good') + " status'>" + "Air quality: " + (coValue > 3 ? 'Bad' : 'Good') + "</span>";
        };
    </script>
</body>
</html>
)rawliteral";

void setup()
{
    Serial.begin(9600);

    lcd.init();                    
    lcd.backlight();

    pinMode(gas_sensor, INPUT);
    pinMode(co_sensor, INPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // route for web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send_P(200, "text/html", index_html);
    });

    // websocket event handling
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.begin();
}

void loop()
{
    // WiFiClient client = server.available(); // listen for incoming clients

    float ppm_gas = calculatePPM(analogRead(gas_sensor), R0, m, c);
    float ppm_co = calculatePPM(analogRead(co_sensor), R01, m1, c1);
    int button_state = digitalRead(buttonPin);

    if (button_state == LOW)
    {
        if (last_state == HIGH)
            lcd.clear();

        lcd.setCursor(0, 0);

        lcd.print("Gas: ");
        lcd.print(ppm_gas);

        lcd.setCursor(0,1);
        lcd.print("CO: ");
        lcd.print(ppm_co);
    }
    else
    {
        if (last_state == LOW)
            lcd.clear();
        lcd.setCursor(0, 0);
        if (ppm_gas > 30) 
            lcd.print("Gas level: Bad");
        else
            lcd.print("Gas level: Good");

        lcd.setCursor(0,1);
        if (ppm_co > 3.2) 
            lcd.print("CO level: Bad");
        else
            lcd.print("CO level: Good");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        String url = thingspeak_server;
        url += "?api_key=" + apiKey;
        url += "&field1=" + String(ppm_gas);
        url += "&field2=" + String(ppm_co);

        http.begin(url);
        int httpCode = http.GET();

        if (httpCode > 0) {
            Serial.printf("ThingSpeak responded with code: %d\n", httpCode);
            String payload = http.getString();
            Serial.println(payload);
        } else {
            Serial.printf("Error on HTTP request: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    } else {
        Serial.println("WiFi not connected");
    }

    // send sensor data over socket
    ws.textAll(String(ppm_gas) + "," + String(ppm_co));

    last_state = button_state;

    delay(30000);
}

// calculate concentration of gas in parts per million
// params: raw sensor reading, resistance of the sensor in clean air, slope of the calibration curve, intercept of the calibration curve
float calculatePPM(int sensorValue, float R0, float m, float c)
{
    float sensor_volt = sensorValue * (3.3 / 4096.0);
    float RS_gas = ((3.3 * 10.0) / sensor_volt) - 10.0;
    float ratio = RS_gas / R0;
    double ppm_log = (log10(ratio) - c) / m;
    double ppm = pow(10, ppm_log);
    return ppm;
}
