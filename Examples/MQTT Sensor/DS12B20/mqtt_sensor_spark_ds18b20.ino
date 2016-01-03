/*

    MQTT Sensor example v 0.1

    This sketch demonstrates how to keep the client connected
    using a non-blocking reconnect function. If the client loses
    its connection, it attempts to reconnect every 5 seconds
    without blocking the main loop.
    
    Home Assistant Component
    https://home-assistant.io/components/sensor.mqtt/
*/


#include "OneWire/OneWire.h"
#include "MQTT/MQTT.h"

//============================================================
// NODE VARIABLES
//============================================================

const String nodeName       = "spark_01";                       // name of this node
const String publishTopic   = "home/livingroom/temperature";    // the topic where you will publish the temperature
const String debugTopic     = "home/debug";                     // this topic is just for debugging
long _next_time_to_sample = millis();                           // currently time
const int loopInterval = 2000;                                  // define how many miliseconds until the nest reading


//============================================================
// SENSOR VARIABLES
//============================================================

// ONE WIRE LIB
OneWire one = OneWire(D0);                                      // the sensor will be connected in the digital port 0
uint8_t rom[8];
uint8_t resp[9];

// AVERAGE TEMPERATURE
const int temperatureManySamples = 5;                           // numbers of samples that will be taken in order to filter the changes
const float temperatureMinChange = 0.3;                         // the temperature will be uptadted if the change in the reading is bigger than this value
float temperatureSamples[temperatureManySamples];               // array that will contain the last readings   

int temperatureSamplesStep   = 0;                               // index of the currently temperature reading
float averageTemperature     = 0;                               // currently temperature average
float lastAverageTemperature = 0;                               // the last temoerature average sent 
float temperatureDifference  = 0.0;                             // the difference between the currently temperature and the last reading sent


//============================================================
// MQTT CONFIGURATION
//============================================================

// this function is used to receive messages of subscribed topics
void callback(char* topic, byte* payload, unsigned int length);

// put the IP address of your MQTT Broker, in our case the Raspberry Pi
byte server[] = { 192, 168, 1, 129 };

// creates the MQTT client that will handle te communication
MQTT client(server, 1883, callback);

// this function is used to receive messages of subscribed topics
// not going to be used in this case
void callback(char* topic, byte* payload, unsigned int length) {}



//============================================================
// SETUP
//============================================================

void setup()
{

    // start serial communication 
    // if your spark core is connected to your computer via USB you can use the Arduino IDE to monitor it
    Serial.begin(9600);

    Serial.println();
    Serial.print("Node ");
    Serial.print(nodeName);
    Serial.print("Starting... ");
    Serial.println();

    // onboard RGB LED will be used
    RGB.control(true);


    // setup of the DS18B20 object
    one.reset();
    one.write(0x33);
    one.read_bytes(rom, 8);
    Serial.print("rom is: ");

    // prints the address of the ID of the connected DS18B20
    for (int i = 0; i < 8; i++)
    {
        Serial.print(rom[i], HEX);
        Serial.print(" ");
    }

    // initialize array
    for ( int i = 0; i < temperatureManySamples; i++ )
    {
        temperatureSamples[i] = 0;
    }

    // wait for wifi to be ready
    while ( !WiFi.ready() )
    {
        // do nothing
        delay(250);
    }

    Serial.println();
    Serial.println("Wi-fi is connected!");
    Serial.println();
}

//============================================================
// LOOP
//============================================================

void loop()
{

    // check if the node is connected to the MQTT broker
    // only retry every 5 seconds, don't bash the broker
    if ( !client.isConnected() && ( millis() - lastRetry ) > 5000 )
    {

        // set last retry to now
        lastRetry = millis();

        // not connected, turn LED red
        RGB.color(255, 0, 0);

        // connect to MQTT broker
        if ( client.connect("spark_Carmos_mqtt") )
        {
            Serial.println();
            Serial.println("Connecting to the MQTT broker...");
            Serial.println();

            // connected, turn LED green
            RGB.color(0, 255, 0);

            client.publish( debugTopic, nodeName + " connected to MQTT broker" );
        }
    }

    else
    {   
        // keeps the node connected and "talking" to the broker
        client.loop();

        Serial.println();
        Serial.println("Connecting to the MQTT broker...");
        Serial.println();

        // inside your loop
        if ( millis() > _next_time_to_sample )
        {

            // TEMPERATURE READING

            one.reset();
            one.write(0x55);
            one.write_bytes(rom, 8);
            one.write(0x44);
            delay(10);
            one.reset();
            one.write(0x55);
            one.write_bytes(rom, 8);
            one.write(0xBE);
            one.read_bytes(resp, 9);
            byte MSB = resp[1];
            byte LSB = resp[0];

            float tempRead = ( (MSB << 8) | LSB ); //using two's compliment
            float temperature = tempRead / 16;


            // TEMPERATURE AVERAGE

            temperatureSamples[temperatureSamplesStep] = temperature;

            Serial.println();
            Serial.print( "Temperature: " );
            Serial.println();

            averageTemperature = 0;

            for ( int i = 0; i < temperatureManySamples; i++ )
            {
                averageTemperature += temperatureSamples[i];
            }

            // divided by the number of the samples to get the average
            averageTemperature = averageTemperature / temperatureManySamples;

            // this fuction return the difference between two valures always in a positive value
            temperatureDifference = fabs( lastAverageTemperature, averageTemperature );

            temperatureSamplesStep += 1;

            if ( temperatureSamplesStep >= temperatureManySamples )
            {
                temperatureSamplesStep = 0;
            }

            // Just update the temperature if the absolut difference is higher than the minimum and if is differente than the last sent temperature
            if ( temperatureDifference > temperatureMinChange && lastAverageTemperature != averageTemperature )
            {
                Serial.println();
                Serial.print( "Average: " );
                Serial.println( averageTemperature );
                Serial.print( "Last average: " );
                Serial.println( lastAverageTemperature );
                Serial.print( "Difference: " );
                temperatureDifference = fabs( lastAverageTemperature, averageTemperature );
                Serial.println( temperatureDifference );
                Serial.println();
                Serial.println( "==========================================" );
                String temp( averageTemperature, 2 );

                formatAndPublish( publishTopic, temp );

                lastAverageTemperature = averageTemperature;
            }

            else
            {
                Serial.println();
                Serial.print( "No significant change in temperature:" );
                Serial.println();
                Serial.println();
                Serial.print( "Average: " );
                Serial.println( averageTemperature );
                Serial.print( "Last average: " );
                Serial.println( lastAverageTemperature );
                Serial.print( "Difference: " );
                Serial.println( temperatureDifference );
                Serial.println();
                Serial.println( "==========================================" );
            }

            // do your sample
            _next_time_to_sample += loopInterval;
        }
    }
}


//============================================================
// EXTRA FUNCTIONS
//============================================================

void formatAndPublish( String topic, String value )
{
    if ( client.isConnected() )
    {
        byte bytebuffer[ value.length()+1 ]; 
        value.getBytes( bytebuffer, value.length()+1 );
        client.publish( topic, bytebuffer, value.length()+1, true ); 
    }    
}


float fabs( float value1, float value2 )
{

    if ( value1 > value2)
    {
        return value1 - value2;
    }

    else if ( value1 < value2 )
    {
        return value2 - value1;
    }

    else
    {
        return 0.0;
    }
}