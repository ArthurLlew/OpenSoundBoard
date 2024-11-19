// File streams
#include <fstream>
// C++ strings & regex
#include <string>
#include <regex>
// Vectors
#include <vector>


/** Checks if provided file path consists of valid characters.
 * 
 *  @param filepath File path to check.
 * 
 *  @throws Runtime Error if path is invalid.
*/
void check_filepath(std::string filepath)
{
    if (!regex_match(filepath, std::regex("^[\\w:\\\\/\\.]+$")))
        throw std::runtime_error(filepath + " is an illegal filepath");
}

 
int main(int argc, char *argv[])
{
    // Check arguments count
    if (argc < 3)
        throw std::runtime_error("Not enough souse (arguments)");

    std::string header_filepath = argv[argc-1];
    // Check last argument is a valid filepath
    check_filepath(header_filepath);
    // Check this file is a header file
    if (!regex_match(header_filepath, std::regex("^[\\w:\\\\/]*\\w+\\.h(pp)?$")))
        throw std::runtime_error(header_filepath + " is not a header file (.h/.hpp)");

    // Open header file
    std::ofstream header(header_filepath);
    if (!header)
        throw std::runtime_error("Unable to open header file");
    // All futher operations can result in exception so handle file closure
    try
    {
        // Put basic header beginning
        header << "#ifndef EMBEDED_FILES\n"
                "#define EMBEDED_FILES\n"
                "\n"
                "// In order to use uint8_t\n"
                "#include <cstdint>\n"
                "\n";

        // Iterate over files to embed
        for (int i = 1; i < argc-1; i++)
        {
            std::string filepath = argv[i];
            // Check filepath
            check_filepath(filepath);

            // Remove file path
            std::string filename = regex_replace(filepath, std::regex(".*[\\\\/]"), "");

            // Check file name
            if (!regex_match(filename, std::regex("^\\w+(\\.\\w+)?$")))
                throw std::runtime_error("File " + filename + " has an illegal name/extension");
            
            // Replace dot with underscore
            filename = regex_replace(filename, std::regex("\\."), "_");

            // Array declaration
            header << "const std::uint8_t " + filename + "[] = {";

            // Read file bytes into array
            std::ifstream file(filepath);
            if (!file)
                throw std::runtime_error("Unable to open resource file " + filepath);
            // All futher operations can result in exception so handle file closure
            try
            {
                char ch;
                char buffer[6];
                while (file.get(ch))
                {
                    sprintf(buffer, " %#04x,", ch);
                    header <<  buffer;
                }
                header << " 0x00 "; // make text file appear as null-terminated string
            }
            catch(const std::exception& e)
            {
                file.close();
                throw std::runtime_error(e.what());
            }
            file.close();

            // Array declaration ending
            header << "};\n";
        }

        // Put header ending
        header << "\n#endif";

        // Close file
        header.close();

        printf("Successefully embeded all files\n");
    }
    catch(const std::exception& e)
    {
        header.close();
        throw std::runtime_error(e.what());
    }
}