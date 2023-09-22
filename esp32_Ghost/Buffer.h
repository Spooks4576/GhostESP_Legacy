#pragma once
#include <FS.h>
#include <SD.h>
#include <Arduino.h>

#define BUF_SIZE 3 * 1024
#define SNAP_LEN 2324
#define SD_CS 10

class Buffer {
public:

  Buffer() {
    bufA = (uint8_t*)malloc(BUF_SIZE);
    bufB = (uint8_t*)malloc(BUF_SIZE);
  }

  bool InitSD(String file_name) {
    pinMode(SD_CS, OUTPUT);

    delay(10);

    if (!SD.begin(SD_CS)) {
      Serial.println(F("Failed to mount SD Card"));
      return false;
    } else {
      Serial.println(F("Mounted SD Card"));
      CreatePcap(&SD, file_name);
      open();
    }
  }

  void CreatePcap(fs::FS* fs, String fn = "") {
    while (fs->exists(fileName)) {
      int i = 0;
      fileName = "/" + fn + "_" + (String)i + ".pcap";
      i++;
    }

    file = fs->open(fileName, FILE_WRITE);
    file.close();
  }

  void open() {
    bufSizeA = 0;
    bufSizeB = 0;

    bufSizeB = 0;

    writing = true;

    write(uint32_t(0xa1b2c3d4));  // magic number
    write(uint16_t(2));           // major version number
    write(uint16_t(4));           // minor version number
    write(int32_t(0));            // GMT to local correction
    write(uint32_t(0));           // accuracy of timestamps
    write(uint32_t(SNAP_LEN));    // max length of captured packets, in octets
    write(uint32_t(105));         // data link type
  }

  void addPacket(uint8_t* buf, uint32_t len) {
    if ((useA && bufSizeA + len >= BUF_SIZE && bufSizeB > 0) || (!useA && bufSizeB + len >= BUF_SIZE && bufSizeA > 0)) {
      return;
    }

    if (useA && bufSizeA + len + 16 >= BUF_SIZE && bufSizeB == 0) {
      useA = false;
    } else if (!useA && bufSizeB + len + 16 >= BUF_SIZE && bufSizeA == 0) {
      useA = true;
    }

    uint32_t microSeconds = micros();
    uint32_t seconds = (microSeconds / 1000) / 1000;

    microSeconds -= seconds * 1000 * 1000;
  }

  void save(fs::FS* fs) {

    if ((useA && bufSizeB == 0) || (!useA && bufSizeA == 0)) {
      return;
    }

    file = fs->open(fileName, FILE_APPEND);
    if (!file) {
      Serial.println(fileName + "'");
      return;
    }

    saving = true;

    uint32_t len;

    if (useA) {
      file.write(bufB, bufSizeB);
      len = bufSizeB;
      bufSizeB = 0;
    } else {
      file.write(bufA, bufSizeA);
      len = bufSizeA;
      bufSizeA = 0;
    }

    file.close();
    saving = false;
  }

  void forceSaveSerial() {
    uint32_t len = bufSizeA + bufSizeB;
    if (len == 0) return;

    saving = true;
    writing = false;

    if (useA) {
      if (bufSizeB > 0) {
        Serial1.write(bufB, bufSizeB);
        bufSizeB = 0;
      }
      if (bufSizeA > 0) {
        Serial1.write(bufA, bufSizeA);
        bufSizeA = 0;
      }
    } else {
      if (bufSizeA > 0) {
        Serial1.write(bufA, bufSizeA);
        bufSizeA = 0;
      }
      if (bufSizeB > 0) {
        Serial1.write(bufB, bufSizeB);
        bufSizeB = 0;
      }
    }

    saving = false;
    writing = true;
  }

  void forceSave(fs::FS* fs) {
    uint32_t len = bufSizeA + bufSizeB;
    if (len == 0) return;

    file = fs->open(fileName, FILE_APPEND);
    if (!file) {
      Serial.println(fileName + "'");
      return;
    }

    saving = true;
    writing = false;

    if (useA) {

      if (bufSizeB > 0) {
        file.write(bufB, bufSizeB);
        bufSizeB = 0;
      }

      if (bufSizeA > 0) {
        file.write(bufA, bufSizeA);
        bufSizeA = 0;
      }

    } else {

      if (bufSizeA > 0) {
        file.write(bufA, bufSizeA);
        bufSizeA = 0;
      }

      if (bufSizeB > 0) {
        file.write(bufB, bufSizeB);
        bufSizeB = 0;
      }
    }

    file.close();

    saving = false;
    writing = true;
  }

  void write(int32_t n) {
    uint8_t buf[4];
    buf[0] = n;
    buf[1] = n >> 8;
    buf[2] = n >> 16;
    buf[3] = n >> 24;
    write(buf, 4);
  }

  void write(uint32_t n) {
    uint8_t buf[4];
    buf[0] = n;
    buf[1] = n >> 8;
    buf[2] = n >> 16;
    buf[3] = n >> 24;
    write(buf, 4);
  }

  void write(uint16_t n) {
    uint8_t buf[2];
    buf[0] = n;
    buf[1] = n >> 8;
    write(buf, 2);
  }

  void write(uint8_t* buf, uint32_t len) {
    if (!writing) return;

    if (useA) {
      memcpy(&bufA[bufSizeA], buf, len);
      bufSizeA += len;
    } else {
      memcpy(&bufB[bufSizeB], buf, len);
      bufSizeB += len;
    }
  }

  uint8_t* bufA;
  uint8_t* bufB;

  uint32_t bufSizeA = 0;
  uint32_t bufSizeB = 0;
  bool writing = false;
  bool useA = true;
  bool saving = false;
  File file;
  String fileName = "/0.pcap";
};