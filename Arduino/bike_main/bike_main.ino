#include <board.h>
#include <BreakoutSDK.h>
#include <stdio.h>

/** Change this to your device purpose */
static const char *device_purpose = "Dev-Kit";
/** Change this to your key for the SIM card inserted in this device
    You can find your PSK under the Breakout SDK tab of your Narrowband SIM detail at
    https://www.twilio.com/console/wireless/sims
*/

static const char *psk_key = "8a9ab0d5dce70526c242de133b7e315e";
int delay_interval = 1 * 1000;
static unsigned long last_send = 0;
/** This is the Breakout SDK top API */
Breakout *breakout = &Breakout::getInstance();

#define LOOP_INTERVAL (1 * 1000)
#define SEND_INTERVAL (10 * 60 * 1000)
#define BUTTON_PIN  38

enum state {
  IS_IDLE,
  CHECKED_OUT,
  CHECK_IN,
  RETURN
};

void setup() {
  // Feel free to change the log verbosity. E.g. from most critical to most verbose:
  //   - errors: L_ALERT, L_CRIT, L_ERR, L_ISSUE
  //   - warnings: L_WARN, L_NOTICE
  //   - information & debug: L_INFO, L_DB, L_DBG, L_MEM
  // When logging, the additional L_CLI level ensure that the output will always be visible, no matter the set level.
  owl_log_set_level(L_INFO);
  LOG(L_WARN, "Arduino setup() starting up\r\n");

  // Set the Breakout SDK parameters
  breakout->setPurpose(device_purpose);
  breakout->setPSKKey(psk_key);
  breakout->setPollingInterval(10 * 60);  // Optional, by default set to 10 minutes

  // Powering the modem and starting up the SDK
  LOG(L_WARN, "Powering on module and registering...");
  breakout->powerModuleOn();

  LOG(L_WARN, "... done powering on and registering.\r\n");
  LOG(L_WARN, "Arduino loop() starting up\r\n");
}

/**
   This is just a simple example to send a command and write out the status to the console.
*/

void sendCommand(const char * command) {
  if (breakout->sendTextCommand(command) == COMMAND_STATUS_OK) {
    LOG(L_INFO, "Tx-Command [%s]\r\n", command);
  } else {
    LOG(L_INFO, "Tx-Command ERROR\r\n");
  }
}
void scanRFID() {

}
/*
   @param RFID: RFID value from scanner
*/
bool userCheckIn() {
  //POLL server for valid RFID login...

  //if ok then "unlock"

  //else do nothing...
}

void sendLocation() {
  gnss_data_t data;
  breakout->getGNSSData(&data);

  if (data.valid && ((last_send == 0) || (millis() - last_send >= SEND_INTERVAL))) {
    last_send = millis();
    if (data.valid) {
      char commandText[512];
      snprintf(commandText, 512, "Current Position:  %d %7.5f %s  %d %7.5f %s\r\n", data.position.latitude_degrees,
               data.position.latitude_minutes, data.position.is_north ? "N" : "S", data.position.longitude_degrees,
               data.position.longitude_minutes, data.position.is_west ? "W" : "E");
      sendCommand(commandText);
    }
  }
}

void get_command_server(char *command, int size = 140) {
  if (breakout->hasWaitingCommand()) {
    // Check if there is a command waiting and log it on the USB serial
    size_t commandLen = 0;
    bool isBinary = false;
    // Read a command
    command_status_code_e code = breakout->receiveCommand(140, command, &commandLen, &isBinary);
    switch (code) {
      case COMMAND_STATUS_OK:
        LOG(L_INFO, "Rx-Command [%.*s]\r\n", commandLen, command);
        break;
      case COMMAND_STATUS_ERROR:
        LOG(L_INFO, "Rx-Command ERROR\r\n");
        break;
      case COMMAND_STATUS_BUFFER_TOO_SMALL:
        LOG(L_INFO, "Rx-Command BUFFER_TOO_SMALL\r\n");
        break;
      case COMMAND_STATUS_NO_COMMAND_WAITING:
        LOG(L_INFO, "Rx-Command NO_COMMAND_WAITING\r\n");
        break;
      default:
        LOG(L_INFO, "Rx-Command ERROR %d\r\n", code);
    }
  }
}

void sendBikeStatus() {

}

bool checkButton(){
  static bool pressed=true;
  int buttonState = digitalRead(BUTTON_PIN);
  
  if (!pressed && buttonState){
    return true;
    char message[] = "You Pressed The Button";
//    sendCommand(message);
  } else if (pressed && !buttonState) {
    LOG(L_INFO, "Button is not currently pressed\r\n");
    return false;
  }
}

void bike_main() {
  state bike_state = IS_IDLE;
  switch (bike_state) {
    case IS_IDLE:
      sendLocation();//if appropriate amount of time has passed
      scanRFID();//this will be more
      //might need to poll for RFID...
      break;

    case CHECK_IN:
      if (userCheckIn()) {
        bike_state = CHECKED_OUT;
        delay_interval = delay_interval / 2;
      } else {
        bike_state = IS_IDLE;
      }
      break;

    case CHECKED_OUT: //The user has successfully "checked_out" a bike
      if(checkButton()){
        //get stats? how long left on the checkout? maybe some twilio implementation?
      }else{
        sendLocation();
      }
      break;

    case RETURN://do necessary return procedure
      //send to twilio that user is done
      char returnMessage[512];
      gnss_data_t data;
      breakout->getGNSSData(&data);
      snprintf(returnMessage, 512, "User: %d\nLat: %7.5f\nLon: %7.5f \nTime: \n",);
      breakout->sendTextCommand(returnMessage);
      bike_state = IS_IDLE;
      break;
  }
}

void loop() {
  bike_main();
  breakout->spin();
  
  delay(delay_interval);
}
