#pragma once

#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <stdint.h>
#include <fstream>
#include <cstring>

/*
BSM File structure

[1] - Key table size (# of keys)

[*] Key table {
    [16] - Name
    [1]  - Type (0 = Int, 1 = Float, 2 = String, 3 = Raw)
    [2]  - Data offset  /  Value low word
    [2]  - Data size    /  Value high hide
}

[*] - Data region
*/

namespace bsmlib {
    enum KeyType {
        BSM_KEYTYPE_INT = 0,
        BSM_KEYTYPE_FLOAT,
        BSM_KEYTYPE_STRING,
        BSM_KEYTYPE_RAW,
        BSM_KEYTYPE_NULL
    };

    struct Key {
        KeyType      type;
        std::string  value_string;
        int          value_int;
        float        value_float;

        std::vector<uint8_t> data;
    };

    class Data {
        public:
            std::map<std::string, Key> keys;    // Map of keynames to values

            void ClearKeys();                       // Clear all keys in structure
            void DeleteKey(std::string keyname);    // Delete key by name

            bool KeyExists (std::string keyname);   // Returns true if key exists in structure

            void SetKey     (std::string keyname, Key key);                     // Set key using Key struct
            void SetInt     (std::string keyname, int value);                   // Set integer by name and value
            void SetFloat   (std::string keyname, float value);                 // Set float by name and value
            void SetString  (std::string keyname, std::string value);           // Set string by name and value
            void SetRaw     (std::string keyname, std::vector<uint8_t> data);   // Set raw by name and value

            Key         GetKey      (std::string keyname);  // Get Key structure of key by name
            int         GetInt      (std::string keyname);  // Get integer value of key by name
            float       GetFloat    (std::string keyname);  // Get float value of key by name
            std::string GetString   (std::string keyname);  // Get string value of key by name

            std::vector<uint8_t> GetRaw(std::string keyname);   // Get raw bytes of key

            bool Load(std::string fname, bool clearFirst = true);   // Load file by name. clearFirst = call ClearKeys() automatically.
            bool Save(std::string fname);                           // Save structure to file

            Data();                     // Default constructor
            Data(std::string fname);    // Constructs structure and loads file
    };
}