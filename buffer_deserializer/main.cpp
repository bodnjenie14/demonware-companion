/************************************************************************************
_____________________________DEMONWARE COMPANION______________________________******
***********************************************************************************
**
** - Name        : byteBuffer Deserializer Main
** - Description : starting point to application
**
** - Author      : Hosseinpourziyaie
** - Started on  : 2022-08-25     | Ended on : 2022-08-28
** - Note        : This console application has a creative drag n drop feature
**
**
** [Copyright © Hosseinpourziyaie 2022] <hosseinpourziyaie@gmail.com>
**
************************************************************************************/

#include "byte_buffer.h"
#include "utilities.h"
#include "discovery.h"
#include <conio.h>
#include <filesystem>
#include <fstream>

struct MarketplaceInventoryItem
{
    uint64_t m_userID;
    char m_accountType[16];
    uint32_t m_itemId;
    uint32_t m_itemQuantity;
    uint32_t m_itemXp;
    char m_itemData[64];
    uint32_t m_expireDateTime;
    int64_t m_expiryDuration;
    uint16_t m_collisionField;
    uint32_t m_modDateTime;
    uint8_t m_customSourceType;
}; // size: 128 

void bdMarketplaceInventory(const char* file/*, MarketplaceInventoryItem* output*/)
{
    auto reader = new bdByteBufferReader(read_binary_file(file));

    while (reader->current_byte < reader->buffer.size())
    {
        //auto output = new MarketplaceInventoryItem{};
        MarketplaceInventoryItem output = {};

        reader->read_uint64(&output.m_userID);
        reader->read_string(output.m_accountType, 16);
        std::cout << "m_userID: " << output.m_userID << "(" << output.m_accountType << ")" << std::endl;

        reader->read_uint32(&output.m_itemId);
        reader->read_uint32(&output.m_itemQuantity);
        reader->read_uint32(&output.m_itemXp);
        std::cout << "m_itemId: " << output.m_itemId << "(0x" << std::hex << std::uppercase << output.m_itemId << std::dec << ")" << " --  m_itemQuantity: " << output.m_itemQuantity << " --  m_itemXp: " << output.m_itemXp << std::endl;

        std::vector<unsigned char> m_itemData; int length;
        reader->read_blob(&m_itemData, &length);
        if (length)std::cout << "m_itemData: 0x" << std::hex << static_cast<int>(m_itemData.at(0)) << std::dec << std::endl;

        reader->read_uint32(&output.m_expireDateTime);
        reader->read_int64(&output.m_expiryDuration);
        std::cout << "m_expireDateTime: " << output.m_expireDateTime << " --  m_expiryDuration: " << output.m_expiryDuration << std::endl;

        reader->read_uint16(&output.m_collisionField);
        reader->read_uint32(&output.m_modDateTime);
        std::cout << "m_collisionField: " << output.m_collisionField << " --  m_modDateTime: " << output.m_modDateTime << std::endl;
        std::cout << "m_modDateTime: " << convert_unix_time(output.m_modDateTime) << std::endl;


        std::cout << "----------------------------------------------------------------------------------------------------------------" << std::endl;
    }
}

void bdMarketingComms(const char* file)
{
    auto reader = new bdByteBufferReader(read_binary_file(file));

    while (reader->current_byte < reader->buffer.size())
    {
        unsigned __int64 m_messageID; std::string m_languageCode;
        reader->read_uint64(&m_messageID);
        std::cout << "m_messageID: " << m_messageID << std::endl;

        reader->read_string(&m_languageCode);
        std::cout << "m_languageCode: <" << m_languageCode << ">" << std::endl;

        std::vector<unsigned char> m_content; int length;
        reader->read_blob(&m_content, &length);
        std::string m_content_str(m_content.begin(), m_content.end());
        std::cout << "m_content: " << m_content_str << std::endl;

        std::vector<unsigned char> m_metadata;
        reader->read_blob(&m_metadata, &length);
        std::string m_metadata_str(m_metadata.begin(), m_metadata.end());
        std::cout << "m_metadata: " << m_metadata_str << std::endl;

        std::cout << std::endl << "----------------------------------------------------------------------------------------------------------------" << std::endl;
    }
}

void ProcessBatchFolder(const std::string& folderPath) {
    namespace fs = std::filesystem;

    if (!fs::exists(folderPath)) {
        fs::create_directory(folderPath);
        std::cout << "Batch folder created at: " << folderPath << std::endl;
    }

    const std::string outputFolder = "./batch_output";
    if (!fs::exists(outputFolder)) {
        fs::create_directory(outputFolder);
        std::cout << "Created output folder: " << outputFolder << std::endl;
    }

    std::cout << "Processing all payloads in folder: " << folderPath << std::endl;

    size_t fileCount = 0;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            ++fileCount;
            std::string filePath = entry.path().string();
            std::string outputFileName = outputFolder + "/" + entry.path().stem().string() + ".txt";

            std::cout << "Found file: " << filePath << " -> Output: " << outputFileName << std::endl;

            std::ofstream outputFile(outputFileName);
            if (!outputFile.is_open()) {
                std::cout << "Error: Unable to create output file: " << outputFileName << std::endl;
                continue;
            }

            std::streambuf* originalCoutBuffer = std::cout.rdbuf();
            std::cout.rdbuf(outputFile.rdbuf());

            try {
                ByteBuffer_StructureDiscovery(filePath.c_str());
            }
            catch (const std::exception& ex) {
                std::cout << "Error processing file: " << ex.what() << std::endl;
            }

            std::cout.rdbuf(originalCoutBuffer);
            outputFile.close();
        }
        else {
            std::cout << "Skipping non-regular file: " << entry.path() << std::endl;
        }
    }

    if (fileCount == 0) {
        std::cout << "No files found in the folder: " << folderPath << std::endl;
    }
    else {
        std::cout << "Batch processing complete. Processed " << fileCount << " files. Output saved in " << outputFolder << std::endl;
    }

    std::cout << "Processing complete. Press any key to close the program..." << std::endl;
    _getch(); 
}

void ShowProgramOptions(char* file) {
    std::cout << "  1- Perform data structure Discovery" << std::endl;
    std::cout << "  2- Deserialize bdMarketingComms buffer" << std::endl;
    std::cout << "  3- Process all payloads in /batch folder and save output in /batch_output" << std::endl;

    int input;
    while (!(std::cin >> input) || (input < 1 || 4 < input)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Re-enter the number: ";
    }

    switch (input) {
    case 1:
        ByteBuffer_StructureDiscovery(file);
        break;
    case 2:
        bdMarketingComms(file);
        break;
    case 3:
        ProcessBatchFolder("./batch"); 
        break;
    default:
        std::cout << "Invalid FunctionID!" << std::endl;
        break;
    }
}

int main(int argc, char* argv[]) {
    namespace fs = std::filesystem;

    if (!fs::exists("./batch")) {
        fs::create_directory("./batch");
        std::cout << "Batch folder created at: ./batch" << std::endl;
    }

    std::cout << "Demonware Companion: ByteBuffer Tool (by Hosseinpourziyaie)" << std::endl
        << "Drag and drop a file or use batch folder!" << std::endl;

    if (argc == 1) {
        std::cout << "No file provided. Choose an option to proceed:" << std::endl;
        ShowProgramOptions(nullptr); 
    }
    else if (argc == 2) {
        if (is_file_exists(argv[1])) {
            ShowProgramOptions(argv[1]); 
        }
        else {
            std::cout << "Error: File not found." << std::endl;
        }
    }
    else {
        std::cout << "Please input one file at a time." << std::endl;
    }

    return 0;
}

