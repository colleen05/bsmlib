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
            std::map<std::string, Key> keys;

            void DeleteKey(std::string keyname);

            bool KeyExists (std::string keyname);

            void SetKey     (std::string keyname, Key key);
            void SetInt     (std::string keyname, int value);
            void SetFloat   (std::string keyname, float value);
            void SetString  (std::string keyname, std::string value);
            void SetRaw     (std::string keyname, std::vector<uint8_t> data);

            Key         GetKey      (std::string keyname);
            int         GetInt      (std::string keyname);
            float       GetFloat    (std::string keyname);
            std::string GetString   (std::string keyname);

            std::vector<uint8_t> GetRaw(std::string keyname);

            bool Load(std::string fname);
            bool Save(std::string fname);

            Data();
            Data(std::string fname);
    };
}