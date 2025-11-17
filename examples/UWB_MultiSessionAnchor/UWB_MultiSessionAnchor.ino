#include <PortentaUWBShield.h>

/**
 * Multi-Session Anchor Demo
 * 
 * This demo shows how to setup the Arduino Portenta C33 with UWB Shield as an 
 * anchor that can simultaneously track multiple tags using different UWB sessions.
 * 
 * The anchor runs two concurrent sessions:
 * - Session 1 (0x111111): Tracks Tag1 (MAC 0x2222) using preamble code 10
 * - Session 2 (0x222222): Tracks Tag2 (MAC 0x4444) using preamble code 11
 * 
 * Different preamble codes prevent interference between sessions, allowing
 * concurrent distance measurements to multiple tags.
 * 
 * It expects two counterpart Stella devices setup as Multi-Session Tags
 * 
 */

// handler for ranging notifications
void rangingHandler(UWBRangingData &rangingData) {
  Serial.print("GOT RANGING DATA - Session: 0x");
  Serial.print(rangingData.sessionHandle(), HEX);
  Serial.print(" - Type: ");
  Serial.println(rangingData.measureType());
  
  if(rangingData.measureType()==(uint8_t)uwb::MeasurementType::TWO_WAY)
  {
    RangingMeasures twr=rangingData.twoWayRangingMeasure();
    for(int j=0;j<rangingData.available();j++)
    {
      if(twr[j].status==0 && twr[j].distance!=0xFFFF)
      {
        Serial.print("Distance: ");
        Serial.println(twr[j].distance);  
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);

#if defined(ARDUINO_PORTENTA_C33)
  /* Only the Portenta C33 has an RGB LED. */
  pinMode(LEDR, OUTPUT);
  digitalWrite(LEDR, LOW);
#endif

  // register the ranging notification handler before starting
  UWB.registerRangingCallback(rangingHandler);
  
  UWB.begin(); //start the UWB stack, use Serial for the log output
  Serial.println("Starting UWB ...");
  
  //wait until the stack is initialised
  while(UWB.state()!=0)
    delay(10);

  // ============ SESSION 1 SETUP ============
  Serial.println("Starting session 1 ...");
  
  //define MAC addresses for session 1
  uint8_t anchor1Addr[] = {0x11, 0x11};
  uint8_t tag1Addr[] = {0x22, 0x22};
  UWBMacAddress anchor1Mac(UWBMacAddress::Size::SHORT, anchor1Addr);
  UWBMacAddress tag1Mac(UWBMacAddress::Size::SHORT, tag1Addr);
  
  //setup session 1 with ID 0x111111, using preamble code 10
  UWBMultiSessionAnchor session1(0x111111, anchor1Mac, tag1Mac, 10);
  
  //add session 1 to the session manager
  UWBSessionManager.addSession(session1);
  
  //prepare and start session 1
  session1.init();
  session1.start();

  // ============ SESSION 2 SETUP ============
  Serial.println("Starting session 2 ...");
  
  //define MAC addresses for session 2
  uint8_t anchor2Addr[] = {0x33, 0x33};
  uint8_t tag2Addr[] = {0x44, 0x44};
  UWBMacAddress anchor2Mac(UWBMacAddress::Size::SHORT, anchor2Addr);
  UWBMacAddress tag2Mac(UWBMacAddress::Size::SHORT, tag2Addr);
  
  //setup session 2 with ID 0x222222, using preamble code 11 (different from session 1!)
  UWBMultiSessionAnchor session2(0x222222, anchor2Mac, tag2Mac, 11);
  
  //add session 2 to the session manager
  UWBSessionManager.addSession(session2);
  
  //prepare and start session 2
  session2.init();
  session2.start();
  
  Serial.println("Multi-session anchor ready!");
}

void loop()
{
#if defined(ARDUINO_PORTENTA_C33)
  /* Only the Portenta C33 has an RGB LED. */
  digitalWrite(LEDR, !digitalRead(LEDR));
#endif
  delay(1000);
}