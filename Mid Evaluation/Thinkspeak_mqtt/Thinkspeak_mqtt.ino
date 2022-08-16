#include <SimpleDHT.h>      
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// Ensure correct credentials to connect to your WiFi Network.
char ssid[] = "Redmi 5A";
char pass[] = "k.pallavi";

// Ensure that the credentials here allow you to publish and subscribe to the ThingSpeak channel.
#define channelID 1699233
const char mqttUserName[] = "MDMRJiAyNgklABEvLTQPDC8"; 
const char clientID[] = "MDMRJiAyNgklABEvLTQPDC8";
const char mqttPass[] = "FtyV6ffot5FRq8uBHKuM3ue2";

#define mqttPort 1883
WiFiClient client;

int pinDHT11 = 0;
SimpleDHT11 dht11(pinDHT11);
byte temp = 0;      //Stores temperature value
const char* server = "mqtt3.thingspeak.com";
int status = WL_IDLE_STATUS; 
long lastPublishMillis = 0;
int connectionDelay = 1;
int updateInterval = 20;
PubSubClient mqttClient( client );

void connectWifi()
{
    Serial.print(F("Connecting to "));
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(F("."));
    }
    Serial.println();
    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
}

void mqttConnect() 
{
    // Loop until connected.
    while ( !mqttClient.connected() )
    {
        // Connect to the MQTT broker.
        if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) ) 
        {
            Serial.print( "MQTT to " );
            Serial.print( server );
            Serial.print (" at port ");
            Serial.print( mqttPort );
            Serial.println( " successful." );
        } 
        else 
        {
            Serial.print( "MQTT connection failed, rc = " );
            Serial.print( mqttClient.state() );
            Serial.println( " Will try again in a few seconds" );
            delay( connectionDelay*1000 );
        }
    }
}

void setup() 
{
    Serial.begin( 115200 );
    // Delay to allow serial monitor to come up.
    delay(500);
    dht11.read(&temp, NULL, NULL);
     
    // Connect to Wi-Fi network.
    connectWifi();
  
    // Configure the MQTT client
    mqttClient.setServer( server, mqttPort ); 
  
    // Set the MQTT message handler function.
    mqttClient.setCallback( mqttSubscriptionCallback );
  
    // Set the buffer to handle the returned JSON. NOTE: A buffer overflow of the message buffer will result in your callback not being invoked.
    mqttClient.setBufferSize( 2048 );
}

// Function to handle messages from MQTT subscription.
void mqttSubscriptionCallback( char* topic, byte* payload, unsigned int length ) 
{
    // Print the details of the message that was received to the serial monitor.
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) 
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// Subscribe to ThingSpeak channel for updates.
void mqttSubscribe( long subChannelID )
{
    String myTopic = "channels/"+String( subChannelID )+"/subscribe/fields/field1";
    String myTopic1 = "channels/"+String( subChannelID )+"/subscribe/fields/field2";
    String myTopic2 = "channels/"+String( subChannelID )+"/subscribe/fields/field3";
    String myTopic3 = "channels/"+String( subChannelID )+"/subscribe/fields/field4";
    mqttClient.subscribe(myTopic.c_str());
    mqttClient.subscribe(myTopic1.c_str());
    mqttClient.subscribe(myTopic2.c_str());
    mqttClient.subscribe(myTopic3.c_str());
}

// Publish messages to a ThingSpeak channel.
void mqttPublish(long pubChannelID, String message) 
{
    String topicString ="channels/" + String( pubChannelID ) + "/publish";
    mqttClient.publish( topicString.c_str(), message.c_str() );
}

void loop() 
{
    
    // Reconnect to WiFi if it gets disconnected.
    if (WiFi.status() != WL_CONNECTED) 
    {
        connectWifi();
    }

    Serial.println("DHTxx test!");
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (t==0) 
    {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    //float t = random(25, 27);
    Serial.print("Temprature = ");
    Serial.print((int)temp);
    Serial.println("C");

    float ecg = random(2, 10);
    ecg = ecg/10;
    Serial.print("ECG Values = ");
    Serial.print(ecg);
    Serial.println(" ");
    
    float hp = random(70, 72);
    Serial.print("Heart pulse = ");
    Serial.print(hp);
    Serial.println(" ");
    
    float bp = random(100, 102);
    Serial.print("Blood Pressre = ");
    Serial.print(bp);
    Serial.println(" ");
    Serial.println(" ");
    Serial.println("----------------------------------------------------- ");
            
    // Connect if MQTT client is not connected and resubscribe to channel updates.
    if (!mqttClient.connected()) 
    {
        mqttConnect(); 
        mqttSubscribe( channelID );
    }
  
    // Call the loop to maintain connection to the server.
    mqttClient.loop(); 
  
    // Update ThingSpeak channel periodically. The update results in the message to the subscriber.
    if ( (millis() - lastPublishMillis) > updateInterval*1000) 
    {
        mqttPublish( channelID, (String("field1=")+String(t)) );
        delay(2000);
        mqttPublish( channelID, (String("field2=")+String(ecg)) );
        delay(2000);
        mqttPublish( channelID, (String("field3=")+String(hp)) );
        delay(2000);
        mqttPublish( channelID, (String("field4=")+String(bp)) );
        lastPublishMillis = millis();
    }
    delay(500);
}
