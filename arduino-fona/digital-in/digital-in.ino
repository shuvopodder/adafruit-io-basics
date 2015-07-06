#include <SoftwareSerial.h>
#include <Adafruit_FONA.h>

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
#define BUTTON 8

// replace xxxxxxxxx with your Adafruit IO key
#define AIO_KEY "xxxxxxxxx"
#define AIO_URL "http://io.adafruit.com/api/feeds/%s/data?X-AIO-Key=%s"

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// button state
int current = 0;
int last = 0;

void setup() {

  // set button pin as an input
  pinMode(BUTTON, INPUT_PULLUP);

  // Serial init
  Serial.begin(115200);
  Serial.println(F("Initializing....(May take a few seconds)"));

  // SoftwareSerial init
  fonaSS.begin(4800);

  // bail if we can't connect to the FONA
  if (! fona.begin(fonaSS)) {
    Serial.println(F("Couldn't find FONA"));
    while(1);
  }

  fona.enableGPRS(false);
  while(!fona.enableGPRS(true)) {
    Serial.println(F("gprs init failed... trying again"));
    delay(2000);
  }
  
  Serial.println(F("gprs enabled"));

}

void loop() {

  // grab the current state of the button
  current = digitalRead(BUTTON);

  // return if the value hasn't changed
  if(current == last)
    return;

  // send the proper value to AIO depending on button state
  if(current == LOW)
    sendData("button", "1");
  else
    sendData("button", "0");

  // save the button state
  last = current;

}

bool sendData(char *feed, char *value) {

  // init return values
  uint16_t statuscode;
  int16_t length;

  // allow for ~60 character feed name or key
  char url[150];
  // allow for ~60 character value
  char data[80];

  // pull together the url and post body
  sprintf(url, AIO_URL, feed, AIO_KEY);
  sprintf(data, "{\"value\": \"%s\"}", value);

  // print urls for debugging
  Serial.println(url);
  Serial.println(data);

  // send post
  if(! fona.HTTP_POST_start(url, F("application/json"), (uint8_t *)data, strlen(data), &statuscode, (uint16_t *)&length))
    return false;

  fona.HTTP_POST_end();

  // should return a HTTP 201
  return statuscode == 201;

}