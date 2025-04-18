#ifndef MEDIBOUND_DEVICE_H
#define MEDIBOUND_DEVICE_H

#include <Arduino.h>
#include <functional>
#include <vector>
#include <stdexcept>
#include "MediboundVariable.h"

namespace Medibound {

enum class DeviceMode {
    STATIC,
    CONTINUOUS
};

enum class DeviceStatus {
    IDLE,
    READY,
    RUNNING
};

class MediboundDevice {
protected:
    String _apiKey;
    String _secretKey;
    String _deviceId;
    DeviceMode _mode;
    DeviceStatus _status;
    std::function<void()> _onStartCallback;
    std::vector<MediboundVariable> _variables;
    
public:
    MediboundDevice(const char* apiKey, const char* secretKey, const char* deviceId, DeviceMode mode) 
        : _apiKey(apiKey), _secretKey(secretKey), _deviceId(deviceId), _mode(mode), _status(DeviceStatus::IDLE) {}
    
    virtual ~MediboundDevice() {}

    // Set the battery level (0-100)
    virtual void setBatteryLevel(uint8_t level) {
        throw std::runtime_error("setBatteryLevel not implemented");
    }

    // Set the callback function for when the device starts
    virtual void onStart(std::function<void()> callback) {
        _onStartCallback = callback;
    }

    // Set the data using a list of variables
    virtual void setData(const std::vector<MediboundVariable>& variables) {
        _variables = variables;
    }

    // Set the device mode
    virtual void setMode(DeviceMode mode) {
        _mode = mode;
    }

    // Get the current device mode
    DeviceMode getMode() const {
        return _mode;
    }

    // Get the device ID
    const String& getDeviceId() const {
        return _deviceId;
    }

    // Get the current status
    DeviceStatus getStatus() const {
        return _status;
    }

protected:
    // Helper function to convert variables to JSON string
    String variablesToJson() const {
        String json = "{";
        bool first = true;

        for (const auto& var : _variables) {
            if (!first) {
                json += ",";
            }
            first = false;

            json += "\"" + var.getName() + "\":";
            
            switch (var.getType()) {
                case VariableType::NUMBER:
                    json += String(var.getNumberValue());
                    break;
                    
                case VariableType::NUMBER_ARRAY: {
                    json += "[";
                    const auto& values = var.getNumberArrayValue();
                    for (size_t i = 0; i < values.size(); ++i) {
                        if (i > 0) json += ",";
                        json += String(values[i]);
                    }
                    json += "]";
                    break;
                }
                
                case VariableType::STRING:
                    json += "\"" + var.getStringValue() + "\"";
                    break;
            }
        }

        json += "}";
        return json;
    }

    // Helper function to encrypt API key with secret key (simple XOR for now)
    String encryptApiKey() const {
        String encrypted = "";
        for (size_t i = 0; i < _apiKey.length(); i++) {
            int xorVal = _apiKey[i] ^ _secretKey[i % _secretKey.length()];
            if (xorVal < 16) encrypted += "0";  // pad with leading 0 if needed
            encrypted += String(xorVal, HEX);
        }
        encrypted.toLowerCase();  // make it consistent
        Serial.println("Encrypted API Key: " + encrypted);
        return encrypted;
    }
};

} // namespace Medibound

#endif // MEDIBOUND_DEVICE_H 