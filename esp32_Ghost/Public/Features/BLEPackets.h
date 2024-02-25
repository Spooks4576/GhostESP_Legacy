#ifdef SUPPORTS_BLE

const char *generateUUID() {
  static char uuid[37];  // 36 characters for UUID and 1 for the null terminator
  const char *hexChars = "0123456789ABCDEF";

  // UUID format: 8-4-4-4-12 (hex characters)
  sprintf(uuid, "%c%c%c%c%c%c%c%c-%c%c%c%c-%c%c%c%c-%c%c%c%c-%c%c%c%c%c%c%c%c%c%c%c%c",
          hexChars[random(16)], hexChars[random(16)], hexChars[random(16)], hexChars[random(16)],
          hexChars[random(16)], hexChars[random(16)], hexChars[random(16)], hexChars[random(16)],
          hexChars[random(16)], hexChars[random(16)], hexChars[random(16)], hexChars[random(16)],
          hexChars[random(16)], hexChars[random(16)], hexChars[random(16)], hexChars[random(16)],
          hexChars[random(16)], hexChars[random(16)], hexChars[random(16)], hexChars[random(16)],
          hexChars[random(16)], hexChars[random(16)], hexChars[random(16)], hexChars[random(16)],
          hexChars[random(16)], hexChars[random(16)], hexChars[random(16)], hexChars[random(16)],
          hexChars[random(16)], hexChars[random(16)]);

  return uuid;
}


struct SamsungTestBLEData {
  NimBLEAdvertisementData advertisementData;
  NimBLEAdvertisementData scanResponse;
};



NimBLEAdvertisementData GetAppleAdvertismentData() {
  NimBLEAdvertisementData randomAdvertisementData = NimBLEAdvertisementData();
  uint8_t packet[17];
  uint8_t size = 17;
  uint8_t i = 0;

  packet[i++] = size - 1;  // Packet Length
  packet[i++] = 0xFF;      // Packet Type (Manufacturer Specific)
  packet[i++] = 0x4C;      // Packet Company ID (Apple, Inc.)
  packet[i++] = 0x00;      // ...
  packet[i++] = 0x0F;      // Type
  packet[i++] = 0x05;      // Length
  packet[i++] = 0xC1;      // Action Flags
  esp_fill_random(&packet[i], 3);

  randomAdvertisementData.addData(std::string((char *)packet, 17));
  return randomAdvertisementData;
}


SamsungTestBLEData GetSamsungTestBLE() {
  NimBLEAdvertisementData advertisementData = NimBLEAdvertisementData();
  NimBLEAdvertisementData scanResponse = NimBLEAdvertisementData();

  // Split your specific packet data into advertisement and scan response data
  uint8_t advertisementPacket[] = {
    0x02, 0x01, 0x18, 0x1B, 0xFF, 0x75, 0x00, 0x42, 0x09, 0x81, 0x02, 0x14,
    0x15, 0x03, 0x21, 0x01, 0x09, 0xEF, 0x0C, 0x01, 0x47, 0x06, 0x3C, 0x94, 0x8E,
    0x00, 0x00, 0x00, 0x00, 0xC7, 0x00
  };
  uint8_t scanResponsePacket[] = {
    0x10, 0xFF, 0x75, 0x00, 0x00, 0x63, 0x50, 0x8D, 0xB1, 0x17, 0x40, 0x46,
    0x64, 0x64, 0x00, 0x01, 0x04
  };

  // Add the packet data to the advertisement data and scan response
  advertisementData.addData(std::string((char *)advertisementPacket, sizeof(advertisementPacket)));
  scanResponse.addData(std::string((char *)scanResponsePacket, sizeof(scanResponsePacket)));

  // Return both advertisement data and scan response
  return { advertisementData, scanResponse };
}




#endif