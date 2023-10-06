#ifdef SUPPORTS_BLE

const char* generateUUID() {
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
        hexChars[random(16)], hexChars[random(16)]
    );

    return uuid;
}



NimBLEAdvertisementData GetAppleAdvertismentData() {
  NimBLEAdvertisementData randomAdvertisementData = NimBLEAdvertisementData();
  uint8_t packet[17];
  uint8_t size = 17;
  uint8_t i = 0;

  packet[i++] = size - 1;    // Packet Length
  packet[i++] = 0xFF;        // Packet Type (Manufacturer Specific)
  packet[i++] = 0x4C;        // Packet Company ID (Apple, Inc.)
  packet[i++] = 0x00;        // ...
  packet[i++] = 0x0F;  // Type
  packet[i++] = 0x05;                        // Length
  packet[i++] = 0xC1;                        // Action Flags
  esp_fill_random(&packet[i], 3);

  randomAdvertisementData.addData(std::string((char *)packet, 17));
  return randomAdvertisementData;
}


NimBLEAdvertisementData GetSamsungTestBLE() {
    NimBLEAdvertisementData samsungAdvertisementData = NimBLEAdvertisementData();
    uint8_t packet[29]; 
    uint8_t i = 0;

    // ...

    // Manufacturer Specific Data
    packet[i++] = 0x07;  // Length (example, including the data type byte and data bytes)
    packet[i++] = 0xFF;  // Manufacturer specific data type value

    // Samsung's Company ID (0x0075)
    packet[i++] = 0x75;  
    packet[i++] = 0x00;  
    
    // Additional manufacturer specific data bytes
    // The structure and meaning of these bytes would depend on Samsung's specific implementation
    packet[i++] = rand() % 256; // Example data
    packet[i++] = rand() % 256; // Example data
    packet[i++] = rand() % 256; // Example data

    // ...

    samsungAdvertisementData.addData(std::string((char *)packet, sizeof(packet)));
    return samsungAdvertisementData;
}




#endif