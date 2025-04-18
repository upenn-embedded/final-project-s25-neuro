#ifndef MEDIBOUND_DEVICE_BLE_H
#define MEDIBOUND_DEVICE_BLE_H

#include "MediboundDevice.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

namespace Medibound {

// Service and Characteristic UUIDs
#define MEDIBOUND_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define COMMAND_CHARACTERISTIC_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define STATUS_CHARACTERISTIC_UUID    "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define DATA_CHARACTERISTIC_UUID      "beb5483e-36e1-4688-b7f5-ea07361b26aa"
#define MODE_CHARACTERISTIC_UUID      "beb5483e-36e1-4688-b7f5-ea07361b26ab"

class MediboundDeviceBLE : public MediboundDevice {
private:
    String _deviceName;
    BLEServer* _server;
    BLEService* _service;
    BLECharacteristic* _commandCharacteristic;
    BLECharacteristic* _statusCharacteristic;
    BLECharacteristic* _dataCharacteristic;
    BLECharacteristic* _modeCharacteristic;
    bool _isAuthenticated;
    TaskHandle_t _dataTask;
    static MediboundDeviceBLE* _instance;

    // Add ServerCallbacks class definition
    class ServerCallbacks : public BLEServerCallbacks {
    private:
        MediboundDeviceBLE* _device;

    public:
        ServerCallbacks(MediboundDeviceBLE* device) : _device(device) {}

        void onDisconnect(BLEServer* server) override {
            Serial.println("[BLE] Client disconnected");
            
            // Stop continuous data collection if running
            if (_device->_dataTask != nullptr) {
                Serial.println("[BLE] Stopping data collection task due to disconnect");
                vTaskDelete(_device->_dataTask);
                _device->_dataTask = nullptr;
            }

            // Reset authentication
            _device->_isAuthenticated = false;
            Serial.println("[BLE] Authentication reset");

            // Reset status to idle
            _device->updateStatus("idle");

            // Reset data to initial state
            _device->setInitialData();

            // Restart advertising
            server->startAdvertising();
            Serial.println("[BLE] Restarted advertising");
        }
    };

public:
    MediboundDeviceBLE(const char* apiKey, const char* secretKey, const char* deviceId, 
                       const char* deviceName, DeviceMode mode)
        : MediboundDevice(apiKey, secretKey, deviceId, mode),
          _deviceName(deviceName), _server(nullptr), _service(nullptr),
          _commandCharacteristic(nullptr), _statusCharacteristic(nullptr),
          _dataCharacteristic(nullptr), _modeCharacteristic(nullptr),
          _isAuthenticated(false), _dataTask(nullptr) {
        _instance = this;
        Serial.println("[BLE] Creating new MediboundDeviceBLE instance");
        Serial.printf("[BLE] Device ID: %s\n", deviceId);
        Serial.printf("[BLE] Device Name: %s\n", deviceName);
        Serial.printf("[BLE] Mode: %s\n", mode == DeviceMode::STATIC ? "static" : "continuous");
        initializeBLE();
    }

    ~MediboundDeviceBLE() {
        if (_dataTask != nullptr) {
            Serial.println("[BLE] Cleaning up data collection task");
            vTaskDelete(_dataTask);
            _dataTask = nullptr;
        }
    }

    void setBatteryLevel(uint8_t level) override {
        Serial.printf("[BLE] Setting battery level: %d%%\n", level);
        // TODO: Implement battery level characteristic
    }

    void setData(const std::vector<MediboundVariable>& variables) override {
        if (!_isAuthenticated) {
            Serial.println("[BLE] Attempt to send data while not authenticated");
            return;
        }

        Serial.println("[BLE] Preparing to send data");
        
        // Manually construct JSON string
        String jsonString = "{\"data\":[";
        
        for (size_t i = 0; i < variables.size(); i++) {
            const auto& var = variables[i];
            String name = var.getName();
            
            jsonString += "{\"name\":\"" + name + "\",";
            
            switch (var.getType()) {
                case VariableType::NUMBER_ARRAY: {
                    jsonString += "\"values\":[";
                    const auto& values = var.getNumberArrayValue();
                    for (size_t j = 0; j < values.size(); j++) {
                        jsonString += String(values[j], 6); // 6 decimal places for floats
                        if (j < values.size() - 1) {
                            jsonString += ",";
                        }
                    }
                    jsonString += "]";
                    break;
                }
                case VariableType::STRING: {
                    String strValue = var.getStringValue();
                    jsonString += "\"value\":\"" + strValue + "\"";
                    break;
                }
                case VariableType::NUMBER:
                default: {
                    jsonString += "\"value\":" + String(var.getNumberValue(), 6);
                    break;
                }
            }
            
            jsonString += "}";
            if (i < variables.size() - 1) {
                jsonString += ",";
            }
        }
        
        jsonString += "]}";
        
        Serial.printf("[BLE] Sending JSON data: %s\n", jsonString.c_str());

        // Send data in chunks if needed (BLE has packet size limitations)
        const size_t maxChunkSize = 512;  // Maximum size for BLE packet
        size_t remaining = jsonString.length();
        size_t offset = 0;

        while (remaining > 0) {
            size_t chunkSize = min(remaining, maxChunkSize);
            String chunk = jsonString.substring(offset, offset + chunkSize);
            
            _dataCharacteristic->setValue((uint8_t*)chunk.c_str(), chunk.length());
            _dataCharacteristic->notify();
            
            remaining -= chunkSize;
            offset += chunkSize;

            if (remaining > 0) {
                // Small delay between chunks if sending multiple
                delay(20);
            }
        }

        Serial.println("[BLE] Data sent successfully");
    }

    

private:
    static void dataCollectionTask(void* parameter) {
        MediboundDeviceBLE* device = (MediboundDeviceBLE*)parameter;
        Serial.println("[BLE] Data collection task started");
        while (true) {
            Serial.println("[BLE] Triggering data collection callback");
            if (device->_onStartCallback) {
                device->_onStartCallback();
            }
            vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second delay
        }
    }

    void initializeBLE() {
        Serial.println("[BLE] Initializing BLE device");
        BLEDevice::init(_deviceName.c_str());

        // Create server and set callbacks
        _server = BLEDevice::createServer();
        _server->setCallbacks(new ServerCallbacks(this));
        Serial.println("[BLE] BLE Server created with disconnect handling");
        
        // Set manufacturer data
        BLEAdvertisementData advData;
        String mfgData = "bm" + String(_deviceId);
        advData.setManufacturerData(mfgData.c_str());
        Serial.printf("[BLE] Setting manufacturer data: %s\n", mfgData.c_str());
        
        _service = _server->createService(MEDIBOUND_SERVICE_UUID);
        Serial.printf("[BLE] Service created with UUID: %s\n", MEDIBOUND_SERVICE_UUID);

        // Create characteristics
        setupCharacteristics();

        // Start the service
        _service->start();
        Serial.println("[BLE] Service started");

        // Start advertising
        BLEAdvertising* advertising = BLEDevice::getAdvertising();
        advertising->setAdvertisementData(advData);
        advertising->start();
        Serial.println("[BLE] Started advertising");

        // Set initial data
        setInitialData();
        updateStatus("idle");
        updateMode(_mode == DeviceMode::STATIC ? "static" : "continuous");
    }

    void setupCharacteristics() {
        Serial.println("[BLE] Setting up characteristics");

        // Command characteristic (notify + write)
        _commandCharacteristic = _service->createCharacteristic(
            COMMAND_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
        );
        _commandCharacteristic->addDescriptor(new BLE2902());
        _commandCharacteristic->setCallbacks(new CommandCallbacks(this));
        Serial.println("[BLE] Command characteristic created");

        // Status characteristic (notify)
        _statusCharacteristic = _service->createCharacteristic(
            STATUS_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
        );
        _statusCharacteristic->addDescriptor(new BLE2902());
        Serial.println("[BLE] Status characteristic created");

        // Data characteristic (notify)
        _dataCharacteristic = _service->createCharacteristic(
            DATA_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
        );
        _dataCharacteristic->addDescriptor(new BLE2902());
        Serial.println("[BLE] Data characteristic created");

        // Mode characteristic (notify + read)
        _modeCharacteristic = _service->createCharacteristic(
            MODE_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ
        );
        _modeCharacteristic->addDescriptor(new BLE2902());
        

        Serial.printf("[BLE] Mode characteristic created");
    }

    void setInitialData() {
        String initialJson = "{\"key\":\"" + encryptApiKey() + "\"}";
        Serial.println("[BLE] Setting initial data");
        _dataCharacteristic->setValue((uint8_t*)initialJson.c_str(), initialJson.length());
        _dataCharacteristic->notify();
        Serial.println("[BLE] Initial data sent");
    }

    void updateStatus(const char* status) {
        Serial.printf("[BLE] Updating status to: %s\n", status);
        _statusCharacteristic->setValue((uint8_t*)status, strlen(status));
        _statusCharacteristic->notify();
    }

    void updateMode(const char* mode) {
        Serial.printf("[BLE] Updating mode to: %s\n", mode);
        if (_modeCharacteristic != nullptr) {
            _modeCharacteristic->setValue((uint8_t*)mode, strlen(mode));
            _modeCharacteristic->notify();
        } else {
            Serial.println("[BLE] Mode characteristic not initialized");
        }
    }

    void setMode(DeviceMode mode) override {
        MediboundDevice::setMode(mode);
        const char* modeStr = mode == DeviceMode::STATIC ? "static" : "continuous";
        updateMode(modeStr);
    }

    class CommandCallbacks : public BLECharacteristicCallbacks {
    private:
        MediboundDeviceBLE* _device;

    public:
        CommandCallbacks(MediboundDeviceBLE* device) : _device(device) {}

        void onWrite(BLECharacteristic* characteristic) override {
            String command = characteristic->getValue();
            Serial.printf("[BLE] Received command: %s\n", command.c_str());

            if (command.startsWith("auth ")) {
                String receivedKey = command.substring(5);
                Serial.println("[BLE] Processing authentication");
                if (receivedKey == _device->_apiKey) {
                    _device->_isAuthenticated = true;
                    _device->updateStatus("ready");
                    Serial.println("[BLE] Authentication successful");
                } else {
                    Serial.println("[BLE] Authentication failed - invalid key");
                }
            } else if (_device->_isAuthenticated) {
                if (command == "start") {
                    Serial.println("[BLE] Start command received");
                    _device->updateStatus("running");
                    if (_device->getMode() == DeviceMode::STATIC) {
                        Serial.println("[BLE] Running in static mode");
                        if (_device->_onStartCallback) {
                            _device->_onStartCallback();
                        }
                        _device->updateStatus("ready");
                    } else {
                        Serial.println("[BLE] Running in continuous mode");
                        if (_device->_dataTask == nullptr) {
                            Serial.println("[BLE] Creating data collection task");
                            xTaskCreate(
                                dataCollectionTask,
                                "DataCollection",
                                4096,
                                _device,
                                1,
                                &_device->_dataTask
                            );
                        }
                    }
                } else if (command == "stop" && _device->getMode() == DeviceMode::CONTINUOUS) {
                    Serial.println("[BLE] Stop command received");
                    if (_device->_dataTask != nullptr) {
                        Serial.println("[BLE] Stopping data collection task");
                        vTaskDelete(_device->_dataTask);
                        _device->_dataTask = nullptr;
                    }
                    _device->updateStatus("ready");
                } else {
                    Serial.printf("[BLE] Unknown command received: %s\n", command.c_str());
                }
            } else {
                Serial.println("[BLE] Command rejected - not authenticated");
            }
        }
    };
};

// Initialize static member
MediboundDeviceBLE* MediboundDeviceBLE::_instance = nullptr;

} // namespace Medibound

#endif // MEDIBOUND_DEVICE_BLE_H 