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

void Header(int argc, char* argv[]);
void CS(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    if (argc != 2 && (argc != 3 && (argv[2] != "-cs"))) {
        std::cout << "Usage: " << "res2h <filename> (-cs)" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 3)
        CS(argc, argv);
    else
        Header(argc, argv);

    return 0;
}

void Header(int argc, char* argv[])
{
    std::ifstream fileIn(argv[1], std::ios::binary);
    if (!fileIn) {
        std::cout << "Failed to open file: " << argv[1] << std::endl;
        exit(EXIT_FAILURE);
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

    std::ofstream fileOut(id + ".h");
    if (!fileOut)
    {
        std::cout << "Failed to create out file" << std::endl;
        fileIn.close();
        exit( EXIT_FAILURE);
    }

    fileOut << "#ifndef RESOURCE_" << id << "_H\n#define RESOURCE_" << id << "_H\n\n";
    fileOut << "#include <memory>\n#include <initializer_list>\n\n";
    fileOut << "namespace res\n{\n";
    fileOut << "    void " << id << "(std::unique_ptr<unsigned char[]>& bytes_out, size_t& count_out)\n    {\n";
    fileOut << "        std::initializer_list<unsigned char> values =\n        {\n";

    unsigned char byte = 0;
    uint8_t row = 0;
    bool isFirst = true;
    fileOut << "            ";
    while (fileIn.read(reinterpret_cast<char*>(&byte), sizeof(byte)))
    {
        if (!isFirst)
        {
            fileOut << ", ";
            if (++row == 16)
            {
                row = 0;
                fileOut << std::endl << "            ";
            }
        }
        else
            isFirst = false;

        fileOut << byteNotation(byte);
    }

    fileOut << "\n        };\n\n";
    fileOut << "        const size_t size = values.size();\n";
    fileOut << "        bytes_out = std::make_unique<unsigned char[]>(size);\n";
    fileOut << "        std::copy(values.begin(), values.end(), bytes_out.get());\n";
    fileOut << "        count_out = size;\n";
    fileOut << "    }\n";
    fileOut << "}\n\n#endif // !" << id;

    fileOut.close();
    fileIn.close();
}

void CS(int argc, char* argv[])
{
    std::ifstream fileIn(argv[1], std::ios::binary);
    if (!fileIn) {
        std::cout << "Failed to open file: " << argv[1] << std::endl;
        exit(EXIT_FAILURE);
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

    std::ofstream fileOut(id + ".cs");
    if (!fileOut)
    {
        std::cout << "Failed to create out file" << std::endl;
        fileIn.close();
        exit(EXIT_FAILURE);
    }

    fileOut << "namespace Resource\n{\n";
    fileOut << "    internal static class " << id << "\n    {\n";
    fileOut << "        public static void Data(out byte[] data)\n        {\n";
    fileOut << "            data = new byte[]\n            {\n";

    unsigned char byte = 0;
    uint8_t row = 0;
    bool isFirst = true;
    fileOut << "                ";
    while (fileIn.read(reinterpret_cast<char*>(&byte), sizeof(byte)))
    {
        if (!isFirst)
        {
            fileOut << ", ";
            if (++row == 16)
            {
                row = 0;
                fileOut << std::endl << "                ";
            }
        }
        else
            isFirst = false;

        fileOut << byteNotation(byte);
    }

    fileOut << "\n            };\n";
    fileOut << "        }\n";
    fileOut << "    }\n";
    fileOut << "}";

    fileOut.close();
    fileIn.close();
}