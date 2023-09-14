// ExpandedCastMessageSerializer.hpp

#ifndef EXPANDED_CAST_MESSAGE_SERIALIZER_HPP
#define EXPANDED_CAST_MESSAGE_SERIALIZER_HPP

#include <stdint.h>
#include <string.h>

namespace ExpandedCastMessageSerializer {

const uint8_t MAX_STRING_SIZE = 64;
const uint8_t MAX_BINARY_PAYLOAD_SIZE = 64;

enum DeserializationResult {
  DESERIALIZATION_SUCCESS = 0,
  INVALID_FORMAT = 1,
  BUFFER_UNDERFLOW = 2
};

enum ProtocolVersion {
  CASTV2_1_0 = 0
};

enum PayloadType {
  STRING = 0,
  BINARY = 1
};

struct CastMessage {
  ProtocolVersion protocol_version;
  char source_id[MAX_STRING_SIZE];
  char destination_id[MAX_STRING_SIZE];
  char namespace_[MAX_STRING_SIZE];
  PayloadType payload_type;
  char payload_utf8[MAX_STRING_SIZE];
  uint8_t payload_binary[MAX_BINARY_PAYLOAD_SIZE];
  uint8_t payload_binary_size;  // Actual size of binary payload
};

enum SerializationResult {
  SUCCESS = 0,
  BUFFER_OVERFLOW = 1
};

inline DeserializationResult deserialize(const uint8_t* buffer, uint16_t length, char* payloadUtf8) {
  uint16_t index = 4;  // Start after the 4-byte length field

  Serial.println("Initial length: " + String(length));

  int payloadStart = -1;
  int braceCount = 0;

  for (; index < length; index++) {
    if (buffer[index] == '{' && payloadStart == -1) {
      payloadStart = index;
      braceCount = 1;
    } else if (buffer[index] == '{') {
      braceCount++;
    } else if (buffer[index] == '}') {
      braceCount--;
    }

    if (payloadStart != -1 && braceCount == 0) {
      break;
    }
  }

  if (payloadStart != -1 && braceCount == 0) {
    int payloadLength = index - payloadStart + 1;

    Serial.println("Identified JSON payload from index " + String(payloadStart) + " to " + String(index));
    Serial.println("Payload length (after identification): " + String(payloadLength));

    // Copy the identified payload
    memcpy(payloadUtf8, &buffer[payloadStart], payloadLength);
    payloadUtf8[payloadLength] = '\0';  // Null-terminate

    return ExpandedCastMessageSerializer::DESERIALIZATION_SUCCESS;
  } else {
    // Ensure null-termination before using strstr
    char tempBuffer[length - 4 + 1];
    memcpy(tempBuffer, &buffer[4], length - 4);
    tempBuffer[length - 4] = '\0';

    const char* sessionIdKey = "\"sessionId\":\"";
    const char* sessionIdStart = strstr((const char*)buffer + payloadStart, sessionIdKey);

    if (sessionIdStart) {
      sessionIdStart += strlen(sessionIdKey);  // Move to the start of the sessionId value
      const char* sessionIdEnd = strchr(sessionIdStart, '\"');

      if (sessionIdEnd) {
        int sessionIdLength = sessionIdEnd - sessionIdStart;
        memcpy(payloadUtf8, sessionIdStart, sessionIdLength);
        payloadUtf8[sessionIdLength] = '\0';  // Null-terminate

        Serial.println("Extracted sessionId: " + String(payloadUtf8));
        return ExpandedCastMessageSerializer::DESERIALIZATION_SUCCESS;  // Indicate that only part of the message was deserialized
      } else {
        Serial.println("Failed to find the end of the sessionId value.");
      }
    } else {
      int distanceFromStart = sessionIdStart - (const char*)buffer;
      Serial.println("Failed to find sessionId key. Printing first 400 characters of buffer for inspection:");
      Serial.println(String((const char*)buffer, 500));
    }
  }

  Serial.println("Failed to identify complete JSON payload or sessionId within buffer");
  return ExpandedCastMessageSerializer::INVALID_FORMAT;
}

inline SerializationResult serialize(const CastMessage& message, uint8_t* buffer, uint16_t& index, uint16_t bufferSize) {
  
  // Helper function to serialize a number as Varint
  auto serializeVarint = [&](uint32_t value) -> uint16_t {
      uint16_t writtenBytes = 0;
      do {
          uint8_t byte = value & 0x7F;
          value >>= 7;
          if (value) byte |= 0x80; // Set the top bit if there are more bytes to come
          if (index + 1 > bufferSize) return 0; // Buffer overflow
          buffer[index++] = byte;
          writtenBytes++;
      } while (value);
      return writtenBytes;
  };

  // Helper function to add strings
  auto addString = [&](const char* str, uint8_t fieldNumber) {
      uint8_t length = strlen(str);
      if (index + length + 2 > bufferSize) return BUFFER_OVERFLOW;  // +2 for field tag and length
      buffer[index++] = (fieldNumber << 3) | 2;                     // Length-delimited
      if (serializeVarint(length) == 0) return BUFFER_OVERFLOW;
      for (uint8_t i = 0; i < length; ++i) {
          buffer[index++] = str[i];
      }
      return SUCCESS;
  };

  // Protocol Version
  if (index + 2 > bufferSize) return BUFFER_OVERFLOW;  // +2 for field tag and value
  buffer[index++] = 0x08;                              // Field 1, Varint
  buffer[index++] = message.protocol_version;

  // Source ID
  if (addString(message.source_id, 2) != SUCCESS) return BUFFER_OVERFLOW;

  // Destination ID
  if (addString(message.destination_id, 3) != SUCCESS) return BUFFER_OVERFLOW;

  // Namespace
  if (addString(message.namespace_, 4) != SUCCESS) return BUFFER_OVERFLOW;

  // Payload Type
  if (index + 2 > bufferSize) return BUFFER_OVERFLOW;  // +2 for field tag and value
  buffer[index++] = 0x28;                              // Field 5, Varint
  buffer[index++] = message.payload_type;

  // Actual Payload (string or binary)
  if (message.payload_type == STRING) {
      if (addString(message.payload_utf8, 6) != SUCCESS) return BUFFER_OVERFLOW;
  } else {                                                                             // BINARY
      if (index + message.payload_binary_size + 2 > bufferSize) return BUFFER_OVERFLOW;  // +2 for field tag and length
      buffer[index++] = (7 << 3) | 2;                                                    // Field 7, Length-delimited
      if (serializeVarint(message.payload_binary_size) == 0) return BUFFER_OVERFLOW;
      for (uint8_t i = 0; i < message.payload_binary_size; ++i) {
          buffer[index++] = message.payload_binary[i];
      }
  }

  return SUCCESS;
}

}  // namespace ExpandedCastMessageSerializer

#endif  // EXPANDED_CAST_MESSAGE_SERIALIZER_HPP