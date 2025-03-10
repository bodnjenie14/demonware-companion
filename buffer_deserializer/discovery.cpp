/************************************************************************************
_____________________________DEMONWARE COMPANION______________________________******
***********************************************************************************
**
** - Name        : byteBuffer structure Discovery
** - Description : automated code to read and discover data types in buffer  
**
** - Author      : Hosseinpourziyaie
** - Started on  : 2022-08-25     | Ended on : 2022-08-28
** - Note        : new code shows data values in front of struct members as comments
**
**
** [Copyright � Hosseinpourziyaie 2022] <hosseinpourziyaie@gmail.com>
**
************************************************************************************/

#include "byte_buffer.h"
#include "utilities.h"
#include <format>
#include <fstream>
#include <cstring>

//#define DUMP_BINARY_DATA

unsigned int Discovery_getDataTypeSize(char type)
{
    switch (type)
    {
    case BD_BB_BOOL_TYPE: case BD_BB_SIGNED_CHAR8_TYPE: case BD_BB_UNSIGNED_CHAR8_TYPE:
        return 1;
    case BD_BB_SIGNED_INTEGER16_TYPE: case BD_BB_UNSIGNED_INTEGER16_TYPE:
        return 2;
    case BD_BB_SIGNED_INTEGER32_TYPE: case BD_BB_UNSIGNED_INTEGER32_TYPE: case BD_BB_FLOAT32_TYPE:
        return 4;
    case BD_BB_SIGNED_INTEGER64_TYPE: case BD_BB_UNSIGNED_INTEGER64_TYPE: case BD_BB_FLOAT64_TYPE:
        return 8;
    default:
        return 0; // corrupted or unknown data type
    }
}

std::string Discovery_getDataValueString(char type, uint64_t value)
{
    switch (type)
    {
    case BD_BB_BOOL_TYPE:
        return std::to_string((bool)value); break;
    case BD_BB_SIGNED_CHAR8_TYPE:
        return std::to_string((char)value); break;
    case BD_BB_UNSIGNED_CHAR8_TYPE:
        return std::to_string((unsigned char)value); break;
    case BD_BB_SIGNED_INTEGER16_TYPE:
        return std::to_string((short)value); break;
    case BD_BB_UNSIGNED_INTEGER16_TYPE:
        return std::to_string((unsigned short)value); break;
    case BD_BB_SIGNED_INTEGER32_TYPE:
        return std::to_string((int)value); break;
    case BD_BB_UNSIGNED_INTEGER32_TYPE:
        return std::to_string((unsigned int)value); break;
    case BD_BB_SIGNED_INTEGER64_TYPE:
        return std::to_string((long long)value); break;
    case BD_BB_UNSIGNED_INTEGER64_TYPE:
        return std::to_string((unsigned long long)value); break;
    case BD_BB_FLOAT32_TYPE:
        return std::to_string((float)value); break;
    case BD_BB_FLOAT64_TYPE:
        return std::to_string((double)value); break;
    default:
        return std::to_string(value); break;
    }
}

void ByteBuffer_StructureDiscovery(const char* filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filepath << std::endl;
        return;
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(fileSize);
    if (fileSize > 0) {
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    }

    try {
        std::cout << "Parsing buffer of size: " << fileSize << " bytes\n";
        std::cout << "Structure contents:\n";
        std::cout << "----------------------------------------\n";

        size_t offset = 0;
        while (offset < buffer.size()) {
            std::string fieldName;
            while (offset < buffer.size() && buffer[offset] != 0) {
                if (isprint(buffer[offset])) {  // Only add printable characters
                    fieldName += static_cast<char>(buffer[offset]);
                }
                offset++;
            }
            
            offset++;

            if (!fieldName.empty()) {
                if (offset + sizeof(uint32_t) <= buffer.size()) {
                    uint32_t value;
                    memcpy(&value, &buffer[offset], sizeof(uint32_t));
                    std::cout << fieldName << ": " << value << "\n";
                    offset += sizeof(uint32_t);
                } else {
                    std::cout << fieldName << ": <no value>\n";
                }
            }

            while (offset < buffer.size() && !isprint(buffer[offset]) && buffer[offset] != 0) {
                offset++;
            }
        }
        
        std::cout << "----------------------------------------\n";
    }
    catch (const std::exception& e) {
        std::cout << "Error parsing buffer: " << e.what() << std::endl;
    }
}