#include <PortentaUWBShield.h>


/**
 * This example demonstrates UWB in-band data transfer during ranging.
 * 
 * Overview:
 * - Establishes a UWB Session between two devices.
 * - Sends custom data payloads piggybacked on ranging packets.
 * - This example acts as the TRANSMITTER (TX).
 * 
 * Features:
 * - Manual Data Send: We update the buffer and trigger sendData() in the loop.
 * - Latency Measurement: A GPIO pulse is generated on PIN 3 upon successful transmission.
 *   Connect a Logic Analyzer to PIN 3 to measure the exact time the packet leaves the stack.
 */

// MAC addresses of the devices
uint8_t devAddr[] = {0x11, 0x11};
uint8_t destination[] = {0x22, 0x22};
UWBMacAddress srcAddr(UWBMacAddress::Size::SHORT, devAddr);
UWBMacAddress dstAddr(UWBMacAddress::Size::SHORT, destination);

UWBInBandDataTx* dataSession;

// Counter for demo
uint32_t dataCounter = 0;
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 1000;

// Ranging callback
void rangingHandler(UWBRangingData& rangingData) {
  if (rangingData.measureType() == (uint8_t)uwb::MeasurementType::TWO_WAY) {
    RangingMeasures twr = rangingData.twoWayRangingMeasure();
    for (int j = 0; j < rangingData.available(); j++) {
      if (twr[j].status == 0 && twr[j].distance != 0xFFFF) {
        Serial.print("Distance: ");
        Serial.print(twr[j].distance);
        Serial.println(" cm");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

#if defined(ARDUINO_PORTENTA_C33)
  pinMode(LEDR, OUTPUT);
  digitalWrite(LEDR, LOW);
#endif

  // Setup Latency Measurement Pin
  // Connect a Scope/Logic Analyzer to D3
  pinMode(3, OUTPUT); 
  digitalWrite(3, LOW);

  Serial.println("=== UWB In-Band Data TX Example ===");
  Serial.println("Initializing UWB...");

  UWB.registerRangingCallback(rangingHandler);
  // Using INFO level for cleaner output. Use UWB_DEBUG_LEVEL for troubleshooting.
  UWB.begin(Serial, uwb::LogLevel::UWB_INFO_LEVEL);

  while (UWB.state() != 0) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" Done!");

  Serial.println("Creating TX session...");
  dataSession = new UWBInBandDataTx(0x11223344, srcAddr, dstAddr, 12);
  UWBSessionManager.addSession(*dataSession);
  dataSession->init();
  dataSession->start();

  Serial.println("Session started. Firmware will auto-send data from global buffer during ranging.");
  Serial.println("");
}

void loop() {
  unsigned long currentTime = millis();

  // Update the global buffer every second
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = currentTime;

    // Create message
    String message = "Hello #" + String(dataCounter);

    // Send the data explicitly!
    // Firmware does NOT auto-send. We must trigger it.
    uwb::Status status = dataSession->sendData((uint8_t*)message.c_str(), message.length() + 1);

    if (status == uwb::Status::SUCCESS) {
        // Latency Measurement Pulse
        // Pulse Pin 3 HIGH for 50us to satisfy logic analyzer trigger
        digitalWrite(3, HIGH);
        delayMicroseconds(50);
        digitalWrite(3, LOW);

        Serial.print("Sent: '");
    } else {
        Serial.print("Send FAILED: '");
    }
    Serial.print(message);
    Serial.println("'");

    dataCounter++;

#if defined(ARDUINO_PORTENTA_C33)
    digitalWrite(LEDR, !digitalRead(LEDR));
#endif
  }

  delay(10);
}