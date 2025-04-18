#ifndef MEDIBOUND_DEVICE_WIFI_H
#define MEDIBOUND_DEVICE_WIFI_H

#include "MediboundDevice.h"
#include <WiFi.h>
#include <HTTPClient.h>

namespace Medibound {

class MediboundDeviceWiFi : public MediboundDevice {
private:
    String _ssid;
    String _password;
    HTTPClient _http;
    bool _isConnected;

public:
    MediboundDeviceWiFi(const char* apiKey, const char* deviceId, DeviceMode mode,
                        const char* ssid, const char* password)
        : MediboundDevice(apiKey, deviceId, mode),
          _ssid(ssid), _password(password), _isConnected(false) {}

    void setBatteryLevel(uint8_t level) override;
    void setData(const String& data) override;

private:
    void connectToWiFi();
    bool ensureWiFiConnection();
};

} // namespace Medibound

#endif // MEDIBOUND_DEVICE_WIFI_H 