/* I dedicate any and all copyright interest in this software to the
 * public domain. I make this dedication for the benefit of the public at
 * large and to the detriment of my heirs and successors. I intend this
 * dedication to be an overt act of relinquishment in perpetuity of all
 * present and future rights to this software under copyright law.
 */

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

#include "emulator.hpp"

std::vector<uint8_t> read_file(const char *path) {
    std::ifstream file(path, std::ios::binary);
    if(!file) {
        std::cerr << "Could not open " << path << std::endl;
        exit(74);
    }
    file.seekg(0, std::ios::end);
    auto file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> data(file_size);
    file.read((char*) &data[0], file_size);
    return data;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <rom-file>" << std::endl;
        return 1;
    }
    auto bin = read_file(argv[1]);
    chip8::Emulator chip8_emu(bin);
    int status = chip8_emu.run();
    return status;
}
