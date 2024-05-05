#include <WiFi.h>
#include <esp_wifi.h>

int probeCount = 0;

void setRandomMACAddress() {
  uint8_t newMACAddress[6];

  // Generate the first byte of the MAC address
  newMACAddress[0] = esp_random() & 0xFE; // Ensure the first byte is even

  // Generate the second byte of the MAC address
  uint8_t secondByte = esp_random() & 0xFF;
  if (secondByte == 0x26 || secondByte == 0xAE) {
    newMACAddress[1] = secondByte;
  } else {
    newMACAddress[1] = 0x2A; // Use a default value of 0x2A if the random byte is not 0x26 or 0xAE
  }

  // Generate the remaining bytes of the MAC address
  for (int i = 2; i < 6; i++) {
    newMACAddress[i] = esp_random() & 0xFF;
  }

  esp_wifi_set_mac(WIFI_IF_STA, newMACAddress);

  Serial.print("New MAC Address: ");
  for (int i = 0; i < 6; i++) {
    if (newMACAddress[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(newMACAddress[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
}

void loop() {
  // Set the SSID and channel for the probe request
  String ssid = "YourSSID";
  uint8_t channel = 1;

  // Create the probe request packet
  uint8_t probe_req[64] = {
    0x40, 0x00, // Frame Control
    0x00, 0x00, // Duration
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination address (broadcast)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source address (random)
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // BSSID (broadcast)
    0x00, 0x00, // Sequence Control
    0x00, 0x00, // SSID parameter set (length will be set later)
    0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24, // Supported rates
    0x32, 0x04, 0x30, 0x48, 0x60, 0x6c // Extended supported rates
  };

  // Set the SSID in the probe request packet
  probe_req[24] = ssid.length();
  memcpy(&probe_req[25], ssid.c_str(), ssid.length());

  // Send the probe request packet
  esp_wifi_80211_tx(WIFI_IF_STA, probe_req, sizeof(probe_req), false);

  probeCount++;

  if (probeCount >= 5) {
    setRandomMACAddress();
    probeCount = 0;
  }

  delay(1000); // Delay between probe requests
}
