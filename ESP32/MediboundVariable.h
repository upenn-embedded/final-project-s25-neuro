#ifndef MEDIBOUND_VARIABLE_H
#define MEDIBOUND_VARIABLE_H

#include <vector>
#include <string>
#include <Arduino.h>

namespace Medibound {

enum class VariableType {
    NUMBER,
    NUMBER_ARRAY,
    STRING
};

class MediboundVariable {
private:
    String _name;
    VariableType _type;
    float _numberValue;
    std::vector<float> _numberArrayValue;
    String _stringValue;

public:
    // Constructor for number
    MediboundVariable(const char* name, float value) 
        : _name(name), _type(VariableType::NUMBER), _numberValue(value) {}

    // Constructor for string
    MediboundVariable(const char* name, const char* value) 
        : _name(name), _type(VariableType::STRING), _stringValue(value) {}

    // Constructor for number array
    MediboundVariable(const char* name, const std::vector<float>& values) 
        : _name(name), _type(VariableType::NUMBER_ARRAY), _numberArrayValue(values) {}

    // Get the name of the variable
    String getName() const { return _name; }

    // Get the type of the variable
    VariableType getType() const { return _type; }

    // Get the number value (for single values)
    float getNumberValue() const { 
        if (_type != VariableType::NUMBER) {
            return 0.0f;
        }
        return _numberValue; 
    }

    // Get the string value
    String getStringValue() const { 
        if (_type != VariableType::STRING) {
            return String("");
        }
        return _stringValue; 
    }

    // Get the number array value
    const std::vector<float>& getNumberArrayValue() const { 
        return _numberArrayValue; 
    }
};

} // namespace Medibound

#endif // MEDIBOUND_VARIABLE_H 