#include <PortentaUWBShield.h>


/**
 * This example demonstrates UWB in-band data reception during ranging.
 * 
 * Overview:
 * - Establishes a UWB Session between two devices.
 * - Receives data payloads piggybacked on ranging packets.
 * - This example acts as the RECEIVER (RX).
 * 
 * Features:
 * - Raw Buffer Parsing: Explicitly parses the raw DataPacket buffer to extract Sequence, Size, and Payload.
 * - Latency Measurement: A GPIO pulse is generated on PIN 3 immediately upon data reception.
 *   Connect a Logic Analyzer to PIN 3 to measure the exact time the packet arrives at the app layer.
 */

// MAC addresses of the devices
uint8_t devAddr[] = {0x22, 0x22};
uint8_t destination[] = {0x11, 0x11};
UWBMacAddress srcAddr(UWBMacAddress::Size::SHORT, devAddr);
UWBMacAddress dstAddr(UWBMacAddress::Size::SHORT, destination);

UWBInBandDataRx* dataSession;

// Statistics
uint32_t packetsReceived = 0;

// Data reception callback - read from global buffer!
void dataRxHandler(uwb::DataPacket& rxData) {
  // Latency Measurement Pulse
  // Pulse Pin 3 HIGH for 50us to satisfy logic analyzer trigger
  digitalWrite(3, HIGH);
  delayMicroseconds(50);
  digitalWrite(3, LOW);

  packetsReceived++;

  // The UWB HAL provides a raw buffer pointer cast to DataPacket&.
  // However, the internal memory layout differs from the DataPacket struct definition.
  // We parse the raw bytes using known offsets to extract the header and payload.
  
  uint8_t* rawData = (uint8_t*)&rxData;
  
  // Parse Header
  // Offset 14-15: Sequence Number
  // Offset 16-17: Actual Data Size
  // Offset 18...: Inline Payload Data
  uint16_t sequenceNumber = *((uint16_t*)&rawData[14]);
  uint16_t actualSize = *((uint16_t*)&rawData[16]);
  uint8_t* dataPayload = &rawData[18];

  Serial.print("RX Seq: ");
  Serial.print(sequenceNumber);
  Serial.print(" | Size: ");
  Serial.print(actualSize);
  
  // Sanity check size before printing
  if (actualSize > 0 && actualSize < 120) {
      Serial.print(" | Data: '");
      for (uint16_t i = 0; i < actualSize; i++) {
        char c = (char)dataPayload[i]; 
        if (c == '\0') break; // Stop at null terminator if string
        if (isPrintable(c)) {
            Serial.print(c);
        } else {
            Serial.print(".");
        }
      }
      Serial.println("'");
  } else {
      Serial.println(" | (Empty/Invalid)");
  }

#if defined(ARDUINO_PORTENTA_C33)
  digitalWrite(LEDG, !digitalRead(LEDG));
#endif
}

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
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, LOW);
#endif


  // Setup Latency Measurement Pin
  // Connect a Scope/Logic Analyzer to D3
  pinMode(3, OUTPUT); 
  digitalWrite(3, LOW);

  Serial.println("=== UWB In-Band Data RX Example ===");
  Serial.println("Initializing UWB...");

  UWB.registerDataRxCallback(dataRxHandler);
  UWB.registerRangingCallback(rangingHandler);
  // Using INFO level for cleaner output. Use UWB_DEBUG_LEVEL for troubleshooting.
  UWB.begin(Serial, uwb::LogLevel::UWB_INFO_LEVEL);

  while (UWB.state() != 0) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" Done!");

  Serial.println("Creating RX session...");
  dataSession = new UWBInBandDataRx(0x11223344, srcAddr, dstAddr, 12);
  UWBSessionManager.addSession(*dataSession);
  dataSession->init();
  dataSession->start();

  Serial.println("Session started. Firmware will auto-write to global buffer during ranging.");
  Serial.println("");
}

void loop() {
  static unsigned long lastStatusTime = 0;
  unsigned long currentTime = millis();

  // Status every 10 seconds
  if (currentTime - lastStatusTime >= 10000) {
    lastStatusTime = currentTime;
    Serial.print("Listening... (Packets: ");
    Serial.print(packetsReceived);
    Serial.println(")");

#if defined(ARDUINO_PORTENTA_C33)
    digitalWrite(LEDB, !digitalRead(LEDB));
#endif
  }

  delay(100);
}