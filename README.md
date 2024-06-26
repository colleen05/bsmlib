# bsmlib- A C++ library for loading and writing binary data to and from files.
**bsmlib** provides functions for loading, modifying, and saving BSM (**B**inary **Sm**all Data) formatted data.
To include this library, add files from the `include` directory to your include path or linker, and files from the `src` path to your source path.
To compile as a static library, check the documentation for your compiler (such as [here](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html) for GCC).

## :warning: :rotating_light: DISCONTINUED C++ PROJECT :rotating_light: :warning:
I'm no longer using C++ for personal projects, thus this project's development has been discontinued.
Check my GitHub or [**my website**](https://colleen05.me/) for projects coming in the future.

## Included demo
A demo application is available in the `./demo/` directory of this repository, which demonstrates some capabilities of bsmlib. To build this demo, execute `build_demo_linux64.sh` on Linux, or `build_demo_win64.bat` on Windows. This will create an executable in the `./demo/bin/` directory.
## File specifications
Supported types:
 - Integers (signed 32-bit)
 - Floats
 - Strings
 - Raw data

|  |Max size/length|
|--|--|
|Keys|255|
|Keyname|16
|Data region|131072 (128kb)
## Using with C++
After including the library with `#include <bsmlib.hpp>`, access all the bsmlib functions in the `bsmlib` namespace through the `bsmlib::Data` class.
### Setting and getting keys
```c++
#include <bsmlib.hpp>

int main() {
	std::vector<uint8_t> data_bytes = {0x02, 0x03, 0x05, 0x08, 0x0D, 0x15, 0x22, 0x37};
	
	bsmlib::Data my_data;

	my_data.SetInt("my_int", 2022);
	my_data.SetFloat("my_float", 3.14159f);
	my_data.SetString("my_string", "Hello, world!");
	my_data.SetRaw("my_raw", data_bytes);

	std::cout << "Int:        " << my_data.GetInt("my_int")         << std::endl;
	std::cout << "Float:      " << my_data.GetFloat("my_float")     << std::endl;
	std::cout << "String:     " << my_data.GetString("my_string")   << std::endl;
	std::cout << "Raw (size): " << my_data.GetRaw("my_raw").size()  << std::endl;
	
	return 0;
}
```
You may also use the `Get*(std::string keyname)` functions across different types. Using `GetString()` on an integer key will return the result of `std::to_string()` on that key's integer value.

**NOTE:** The raw bytes returned from `GetRaw()` on non-raw keys may not be in the same order as stored in files depending on your system. Usage of `GetRaw()` on non-raw keys is discouraged.
### Saving and loading files
Use `bsmlib::Data::Save(std::string filename)` to save to a file.
```c++
bsmlib::Data my_data;
// ...
my_data.Save("data.bsm");
```
Use `bsmlib::Data::Load(std::string filename, bool clearFirst = true)` to load from a file.
```c++
bsmlib::Data my_data;
my_data.Load("data.bsm");
```
OR, pass a filename as an `std::string` to the constructor to automatically load data from a file.
```c++
bsmlib::Data my_data("data.bsm");
```
You can also set the `clearFirst` argument to `false` if you would like to append data to the structure from a file, rather than loading the file data as a new structure.
### Deleting a key
Deleting a key is as simple as calling `bsmlib::Data::DeleteKey(std::string filename)`.
You may also use `bsmlib::Data::ClearKeys()` to delete every key in a structure.
### Testing if key exists
If you want to know if a key exists, call `bsmlib::Data::KeyExists(std::string filename)`. It will return `true` if a key exists by the given name, and `false` if it does not.
## BSM file structure
The `bsmlib.hpp` file contains a simple text markup of the layout of the file.
```
[1] - Key table size (# of keys)

[*] Key table { (21 bytes each)
    [16] - Name
    [1]  - Type (0 = Int, 1 = Float, 2 = String, 3 = Raw)
    [2]  - Data offset  /  Value low word
    [2]  - Data size    /  Value high hide
}

[*] - Data region
```
The first byte of the file signifies the number of keys stored.
For the given table size, each entry is **21 bytes** long. The **first 16 bytes** stores the name of the key, the **17th byte** stores the type, and the **18th to 21st bytes** store either the data region info or value depending on the keys type. **Integers and floats** get their data stored in the **last 4 bytes** of every table entry. **Strings and raws** use the **18th and 19th** bytes to store their data's offset from the beginning of the data region, and the **20th and 21st** bytes to store the length of their data.
**Note: All values are stored in little-endian format**.
