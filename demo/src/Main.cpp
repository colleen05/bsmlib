#include <bsmlib.hpp>
#include <iomanip>

int main() {
    // Create bsm data object
    bsmlib::Data data;
    
    // Set some keys
    data.SetInt("test_int", 1234);
    data.SetFloat("test_float", 3.1415f);
    data.SetString("test_string", "Hello, world!");
    data.SetRaw("test_raw", {0x00, 0x01, 0x01, 0x02, 0x03, 0x05, 0x08, 0x0D});

    // Write data to file
    data.Save("test.bsm");

    // Load data into new object
    bsmlib::Data data_in("test.bsm");

    // Get values with .Get*()
    std::cout << "--- Read data with bsmlib::Data::Get*() ---" << std::endl;
    std::cout << "test.bsm::test_int    = " << std::to_string(data_in.GetInt("test_int"))        << std::endl;
    std::cout << "test.bsm::test_float  = " << std::to_string(data_in.GetFloat("test_float"))    << std::endl;
    std::cout << "test.bsm::test_string = " <<                data_in.GetString("test_string")   << std::endl;
    std::cout << "test.bsm::test_raw    = " << std::to_string(data_in.GetRaw("test_raw").size()) << " bytes." << std::endl;
    std::cout << std::endl;

    // Get values with iterator
    std::cout << "--- Read data with iterator ---" << std::endl;

    for(auto &p : data_in.keys) {
        auto &keyname = p.first;
        auto &keyvalue = p.second;

        switch(keyvalue.type) {
            case bsmlib::KeyType::Integer:
                std::cout << "FOUND: (int) test.bsm::" << keyname << " = " << std::to_string(keyvalue.value_int) << std::endl;
                break;
            case bsmlib::KeyType::Float:
                std::cout << "FOUND: (float) test.bsm::" << keyname << " = " << std::to_string(keyvalue.value_float) << std::endl;
                break;
            case bsmlib::KeyType::String:
                std::cout << "FOUND: (string) test.bsm::" << keyname << " = " << keyvalue.value_string << std::endl;
                break;
            case bsmlib::KeyType::Raw:
                std::cout << "FOUND: (raw) test.bsm::" << keyname << " = " << std::to_string(keyvalue.data.size()) << " bytes." << std::endl;
                break;
        }
    }

    std::cout << std::endl;

    // Print bytes of raw value, just for good measure.
    auto rawdata = data_in.GetRaw("test_raw");

    std::cout << "--- Raw bytes for test.bsm::test_raw ---" << std::endl;
    std::cout << "(" << rawdata.size() << " bytes):" << std::endl;

    for(auto &b : rawdata) {
        auto *s = (char *)malloc(5);
        sprintf(s, "0x%02X ", b);
        std::cout << s;
        free(s);
    }

    std::cout << std::endl;

    return 0;
}