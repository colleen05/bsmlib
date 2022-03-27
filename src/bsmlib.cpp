#include <bsmlib.hpp>

void bsmlib::Data::DeleteKey(std::string keyname) {
    keys.erase(keyname);
}

bool bsmlib::Data::KeyExists(std::string keyname) {
    return keys.count(keyname) > 0;
}

void bsmlib::Data::SetKey(std::string keyname, Key key) {
    keys.insert_or_assign(keyname, key);
}

void bsmlib::Data::SetInt(std::string keyname, int value) {
    SetKey(keyname, Key {
        BSM_KEYTYPE_INT,
        "",
        value,
        0.0f,
        std::vector<uint8_t>()
    });
}

void bsmlib::Data::SetFloat(std::string keyname, float value) {
    SetKey(keyname, Key {
        BSM_KEYTYPE_FLOAT,
        "",
        0,
        value,
        std::vector<uint8_t>()
    });
}

void bsmlib::Data::SetString(std::string keyname, std::string value) {
    SetKey(keyname, Key {
        BSM_KEYTYPE_STRING,
        value,
        0,
        0.0f,
        std::vector<uint8_t>()
    });
}

void bsmlib::Data::SetRaw(std::string keyname, std::vector<uint8_t> data) {
    SetKey(keyname, Key {
        BSM_KEYTYPE_RAW,
        "",
        0,
        0.0f,
        data
    });
}

bsmlib::Key bsmlib::Data::GetKey(std::string keyname) {
    return (keys.count(keyname) > 0) ? keys[keyname] : Key {
        BSM_KEYTYPE_NULL,
        "",
        0,
        0.0f,
        std::vector<uint8_t>()
    };
}

int bsmlib::Data::GetInt(std::string keyname) {
    if(keys.count(keyname) > 0) {
        if(keys[keyname].type == BSM_KEYTYPE_INT) {
            return keys[keyname].value_int;
        }
    }

    return 0;
}

float bsmlib::Data::GetFloat(std::string keyname) {
    if(keys.count(keyname) > 0) {
        if(keys[keyname].type == BSM_KEYTYPE_FLOAT) {
            return keys[keyname].value_float;
        }
    }

    return 0.0f;
}

std::string bsmlib::Data::GetString(std::string keyname) {
    if(keys.count(keyname) > 0) {
        if(keys[keyname].type == BSM_KEYTYPE_STRING) {
            return keys[keyname].value_string;
        }
    }

    return "";
}

std::vector<uint8_t> bsmlib::Data::GetRaw(std::string keyname) {
    if(keys.count(keyname) > 0) {
        if(keys[keyname].type == BSM_KEYTYPE_RAW) {
            return keys[keyname].data;
        }
    }

    return std::vector<uint8_t>();
}

bool bsmlib::Data::Load(std::string fname) {
    std::ifstream file;

    int data_region_start   = 0,
        keycount            = 0,
        filesize            = 0;

    std::vector<uint8_t> filedata;

    // Load file bytes
    file.open(fname, std::ios::binary);
    if(!file.is_open()) return false;

    file.unsetf(std::ios::skipws);

    file.seekg(0, std::ios::end);
    filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    filedata.reserve(filesize);
    filedata.insert(
        filedata.begin(),
        std::istream_iterator<uint8_t>(file),
        std::istream_iterator<uint8_t>()
    );

    // Check file validity
    if(filesize < 22) return false; // Impossibly small size
    if(filesize < (filedata[0] * 21 + 1)) return false; // Does not match header

    // Parse file
    data_region_start   = 1 + (filedata[0] * 21);
    keycount            = filedata[0];

    for(int i = 1; i < data_region_start; i += 21) {
        KeyType vtype   = (KeyType)(filedata[i + 16]);
        auto    keyname = std::string(filedata.begin() + i, filedata.begin() + i + 16);

        keyname.erase(std::find(keyname.begin(), keyname.end(), '\0'), keyname.end());

        if(vtype == BSM_KEYTYPE_INT) {
            SetInt(keyname,
                (filedata[i + 17 + 0] << 0)  +
                (filedata[i + 17 + 1] << 8)  +
                (filedata[i + 17 + 2] << 16) +
                (filedata[i + 17 + 3] << 24)
            );
                
            std::cout << "INFO: BSM: [" << fname << "] Key (Int)    \"" << keyname << "\": " << GetInt(keyname)<< "\n";
        } else if(vtype == BSM_KEYTYPE_FLOAT) {
            float value = 0.0f;

            uint32_t valbytes =
                (filedata[i + 17 + 0] << 0)  +
                (filedata[i + 17 + 1] << 8)  +
                (filedata[i + 17 + 2] << 16) +
                (filedata[i + 17 + 3] << 24) ;

            memcpy(&value, &valbytes, 4);

            SetFloat(keyname, value);

            std::cout << "INFO: BSM: [" << fname << "] Key (Float)  \"" << keyname << "\": " << GetFloat(keyname)<< "\n";
        } else if(vtype == BSM_KEYTYPE_STRING) {
            int data_offset = (filedata[i + 17 + 0]) + (filedata[i + 17 + 1] << 8);
            int data_size   = (filedata[i + 17 + 2]) + (filedata[i + 17 + 3] << 8);

            SetString(keyname, std::string (
                filedata.begin() + data_region_start + data_offset,
                filedata.begin() + data_region_start + data_offset + data_size
            ));

            std::cout << "INFO: BSM: [" << fname << "] Key (String) \"" << keyname << "\": \"" << GetString(keyname) << "\"\n";
        } else if(vtype == BSM_KEYTYPE_RAW) {
            int data_offset = (filedata[i + 17 + 0]) + (filedata[i + 17 + 1] << 8);
            int data_size   = (filedata[i + 17 + 2]) + (filedata[i + 17 + 3] << 8);

            SetRaw(keyname, std::vector<uint8_t> (
                filedata.begin() + data_region_start + data_offset,
                filedata.begin() + data_region_start + data_offset + data_size
            ));

            std::cout << "INFO: BSM: [" << fname << "] Key (Raw)    \"" << keyname << "\": " << data_size<< " Bytes \n";
        }
    }

    return true;
}

bool bsmlib::Data::Save(std::string fname) {
    std::ofstream file;

    std::vector<uint8_t> tableregion;
    std::vector<uint8_t> dataregion;

    tableregion.push_back((uint8_t)keys.size());

    for(auto &kp : keys) {
        auto keyname = kp.first;
        auto key = kp.second;

        // Pad out name to 16 bytes
        auto keyname_bytes = std::vector<uint8_t>(keyname.begin(), keyname.end());
        keyname_bytes.resize(16, 0);

        // Push name and type
        tableregion.insert(std::end(tableregion), keyname_bytes.begin(), keyname_bytes.end());
        tableregion.push_back((uint8_t)key.type);

        // Push value / dataregion markers
        if(key.type == BSM_KEYTYPE_INT){
            tableregion.push_back((uint8_t)((key.value_int & 0x000000FF) >> 0));
            tableregion.push_back((uint8_t)((key.value_int & 0x0000FF00) >> 8));
            tableregion.push_back((uint8_t)((key.value_int & 0x00FF0000) >> 16));
            tableregion.push_back((uint8_t)((key.value_int & 0xFF000000) >> 24));
        }else if(key.type == BSM_KEYTYPE_FLOAT) {
            uint32_t valbytes;

            memcpy(&valbytes, &key.value_float, 4);

            tableregion.push_back((uint8_t)((valbytes & 0x000000FF) >> 0));
            tableregion.push_back((uint8_t)((valbytes & 0x0000FF00) >> 8));
            tableregion.push_back((uint8_t)((valbytes & 0x00FF0000) >> 16));
            tableregion.push_back((uint8_t)((valbytes & 0xFF000000) >> 24));
        }else if(key.type == BSM_KEYTYPE_RAW) {
            tableregion.push_back((uint8_t)((dataregion.size() & 0x00FF) >> 0));
            tableregion.push_back((uint8_t)((dataregion.size() & 0xFF00) >> 8));

            tableregion.push_back((uint8_t)((key.data.size() & 0x00FF) >> 0));
            tableregion.push_back((uint8_t)((key.data.size() & 0xFF00) >> 8));

            dataregion.insert(dataregion.end(), key.data.begin(), key.data.end());
        }else if(key.type == BSM_KEYTYPE_STRING) {
            tableregion.push_back((uint8_t)((dataregion.size() & 0x00FF) >> 0));
            tableregion.push_back((uint8_t)((dataregion.size() & 0xFF00) >> 8));

            tableregion.push_back((uint8_t)((key.value_string.size() & 0x00FF) >> 0));
            tableregion.push_back((uint8_t)((key.value_string.size() & 0xFF00) >> 8));

            dataregion.insert(dataregion.end(), key.value_string.begin(), key.value_string.end());
        }
    }

    // Write bytes
    file.open(fname, std::ios::out | std::ios::binary);

    file.write((char *)&tableregion[0], tableregion.size());
    file.write((char *)&dataregion[0], dataregion.size());

    file.close();

    return true;
}

bsmlib::Data::Data(std::string fname) {
    keys = std::map<std::string, Key>();

    Load(fname);
}

bsmlib::Data::Data() {
    keys = std::map<std::string, Key>();
}