#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>

std::string byteNotation(unsigned char byte)
{
    std::stringstream ss;
    ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << "res2h <filename>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream fileIn(argv[1], std::ios::binary);
    if (!fileIn) {
        std::cout << "Failed to open file: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    std::filesystem::path fs = argv[1];
    std::string fileName = fs.filename().u8string();
    std::string extension = fs.extension().u8string();
    extension = extension.substr(1);
    while (fileName.back() != '.')
        fileName.pop_back();
    fileName.pop_back();

    for (char& c : fileName)
        if (c == ' ' || c == '-' || c == '.')
            c = '_';

    std::string id = fileName + "_" + extension;
    std::cout << id << std::endl;

    std::ofstream fileOut( id + ".h");
    if (!fileOut)
    {
        std::cout << "Failed to create out file" << std::endl;
        fileIn.close();
        return EXIT_FAILURE;
    }

    fileOut << "#ifndef RESOURCE_" << id << "_H\n#define RESOURCE_" << id << "_H\n\n";
    fileOut << "#include <memory>\n#include <initializer_list>\n\n";
    fileOut << "void " << id << "(std::unique_ptr<unsigned char[]>& bytes_out, size_t& count_out)\n{\n";
    fileOut << "    std::initializer_list<unsigned char> values =\n    {\n";

    unsigned char byte = 0;
    uint8_t row = 0;
    bool isFirst = true;
    fileOut << "        ";
    while (fileIn.read(reinterpret_cast<char*>(&byte), sizeof(byte)))
	{
        if (!isFirst)
        {
            fileOut << ", ";
            if (++row == 16)
            {
                row = 0;
                fileOut << std::endl << "        ";
            }
        }
        else
            isFirst = false;

		fileOut << byteNotation(byte);
	}

    fileOut << "\n    };\n\n";
    fileOut << "    const size_t size = values.size();\n";
    fileOut << "    bytes_out = std::make_unique<unsigned char[]>(size);\n";
    fileOut << "    std::copy(values.begin(), values.end(), bytes_out.get());\n";
    fileOut << "    count_out = size;\n";
    fileOut << "}\n\n#endif // !" << id;

    fileOut.close();
    fileIn.close();
    return 0;
}