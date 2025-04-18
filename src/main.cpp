#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <AsyncWebSocket.h>
#include <HTTPClient.h>

// network configs
const char* ssid = "qwerty";
const char* password = "22222222";

// thingspeak configs
const char* thingspeak_server = "http://api.thingspeak.com/update";
String apiKey = "G06HMFEEHU6NTOHJ";

// web server configs
AsyncWebServer server(80); // port 80
AsyncWebSocket ws("/ws"); // web socket enpoint

bool is_first_run = true;

// pins
int button_pin = 19;
int gas_sensor = 34;
int co_sensor = 35;

// calibration constraints for the sensors
float m = -0.353;
float c = 0.711;
float R0 = 23.30;
float m1 = -0.67;
float c1 = 1.34;
float R01 = 5.80;

unsigned long lastThingSpeakTime = 0; // to store the last time data was sent to ThingSpeak
const unsigned long thingspeakInterval = 900000; // every 30 minutes

// button last state
int last_state = HIGH;

// lcd object
LiquidCrystal_I2C lcd(0x27, 16, 2);  

// function to calculate gas concentration (Parts Per Million)
float calculatePPM(int sensorValue, float R0, float m, float c);

// web socket event handler
void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {}

// HTML content (stored in flash memory)
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
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <div class="container">
        <h1>Real-time Sensor Readings</h1>
        <div class="sensor-reading" id="gas">Gas: </div>
        <div class="sensor-reading" id="co">CO: </div>
        <canvas id="gasChart" width="400" height="200"></canvas>
        <canvas id="coChart" width="400" height="200"></canvas>
    </div>
    <script>
        var socket = new WebSocket("ws://" + window.location.host + "/ws");
        socket.onmessage = function(event) {
            var data = event.data.split(",");
            var gasValue = parseFloat(data[0]);
            var coValue = parseFloat(data[1]);

            document.getElementById("gas").innerHTML = "Gas: " + gasValue + " ppm" + "<span class='" + (gasValue > 1200 ? 'bad' : 'good') + " status'>" + "Air quality: " + (gasValue > 30 ? 'Bad' : 'Good') + "</span>";
            document.getElementById("co").innerHTML = "CO: " + coValue + " ppm" + "<span class='" + (coValue > 50 ? 'bad' : 'good') + " status'>" + "Air quality: " + (coValue > 3 ? 'Bad' : 'Good') + "</span>";
        };

        async function fetchData(field) {
            const response = await fetch(`https://api.thingspeak.com/channels/2572875/fields/${field}.json?api_key=EVUOCDYFKA4Q19RU&results=10`);
            const data = await response.json();
            return data.feeds.map(feed => ({
                value: feed[`field${field}`],
                timestamp: new Date(feed.created_at)
            }));
        }

        async function renderCharts() {
            const gasData = await fetchData(1);
            const coData = await fetchData(2);

            const gasLabels = gasData.map(entry => entry.timestamp.toLocaleDateString());
            const coLabels = coData.map(entry => entry.timestamp.toLocaleDateString());

            new Chart(document.getElementById('gasChart'), {
                type: 'line',
                data: {
                    labels: gasLabels,
                    datasets: [{
                        label: 'Gas Sensor Data',
                        data: gasData.map(entry => entry.value),
                        borderColor: 'rgba(75, 192, 192, 1)',
                        borderWidth: 2,
                        fill: false
                    }]
                },
                options: {
                    scales: {
                        x: {
                            title: {
                                display: true,
                                text: 'Date'
                            }
                        },
                        y: {
                            title: {
                                display: true,
                                text: 'Gas (ppm)'
                            }
                        }
                    }
                }
            });

            new Chart(document.getElementById('coChart'), {
                type: 'line',
                data: {
                    labels: coLabels,
                    datasets: [{
                        label: 'CO Sensor Data',
                        data: coData.map(entry => entry.value),
                        borderColor: 'rgba(255, 99, 132, 1)',
                        borderWidth: 2,
                        fill: false
                    }]
                },
                options: {
                    scales: {
                        x: {
                            title: {
                                display: true,
                                text: 'Date'
                            }
                        },
                        y: {
                            title: {
                                display: true,
                                text: 'CO (ppm)'
                            }
                        }
                    }
                }
            });
        }

        renderCharts();
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
    pinMode(button_pin, INPUT_PULLUP);

    // connect to wifi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) // wait till wifi connect
    {
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); // get local ip of web server

    // route for web page (for GET requests)
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request){
        request->send_P(200, "text/html", index_html);
    });

    // web socket event handling
    ws.onEvent(onWsEvent); // set web socket event handler function
    server.addHandler(&ws); // add web socket handler to server

    server.begin();
}

void loop()
{
    // read analog data
    float ppm_gas = calculatePPM(analogRead(gas_sensor), R0, m, c);
    float ppm_co = calculatePPM(analogRead(co_sensor), R01, m1, c1);
    int button_state = digitalRead(button_pin);

    // button to change to the gas data screen on lcd
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
    else // display bad or good result instead
    {
        if (last_state == LOW)
            lcd.clear();
        lcd.setCursor(0, 0);
        if (ppm_gas > 1200) 
            lcd.print("Gas: Bad");
        else
            lcd.print("Gas: Good");

        lcd.setCursor(0, 1);
        if (ppm_co > 50) 
            lcd.print("CO: Bad");
        else
            lcd.print("CO: Good");
    }

    // Check if it's time to send data to ThingSpeak
    if (millis() - lastThingSpeakTime >= thingspeakInterval || is_first_run) 
    {
        lastThingSpeakTime = millis();
        is_first_run = false;

        // sending data to thingspeak server
        if (WiFi.status() == WL_CONNECTED) // check if is connected to wifi network
        {
            // http client object and url of thingspeak server
            HTTPClient http;
            String url = thingspeak_server;

            // concatenate url with appropriate url variables
            url += "?api_key=" + apiKey;
            url += "&field1=" + String(ppm_gas);
            url += "&field2=" + String(ppm_co);

            // begin sending http GET request to thingspeak
            http.begin(url);
            int httpCode = http.GET();

            // check http status code
            if (httpCode > 0)
            {
                Serial.printf("ThingSpeak responded with code: %d\n", httpCode);
                String payload = http.getString(); // response data from thingspeak
                Serial.println(payload);
            }
            else // http request error
            {
                Serial.printf("Error on HTTP request: %s\n", http.errorToString(httpCode).c_str());
            }

            http.end(); // end http request
        }
        else
        {
            Serial.println("WiFi not connected");
        }
    }

    // send sensor data over socket
    ws.textAll(String(ppm_gas) + "," + String(ppm_co));

    last_state = button_state;

    delay(1500); // every 1.5 seconds
}

// calculate concentration of gas in parts per million
// params: raw sensor reading, resistance of the sensor in clean air, slope of the calibration curve, intercept of the calibration curve
float calculatePPM(int sensorValue, float R0, float m, float c)
{
    float sensor_volt = sensorValue * (3.3 / 4096.0); // convert to voltage value
    float RS_gas = ((3.3 * 10.0) / sensor_volt) - 10.0; // sensor resistance
    float ratio = RS_gas / R0; // ratio of sensor resistance to baseline resistance
    double ppm_log = (log10(ratio) - c) / m; // logarithm of PPM value using calibration curve
    double ppm = pow(10, ppm_log); // convert logarithm back to PPm value
    return ppm;
}
