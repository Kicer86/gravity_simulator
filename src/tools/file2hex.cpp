
#include <iostream>
#include <fstream>

#include <boost/algorithm/string/replace.hpp>

#include <boost/filesystem.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>


int generateHex(const boost::program_options::variables_map& options)
{
    const std::string input_file_path = options["input"].as<std::string>();
    const std::string output_file_path = options["output"].as<std::string>();

    std::fstream input_file(input_file_path, std::ios::in | std::ios::binary);

    if (input_file.fail())
    {
        std::cerr << "Could not open file " << input_file_path << " for reading" << std::endl;
        return 1;
    }

    std::fstream output_file(output_file_path, std::ios::out | std::ios::trunc);

    if (output_file.fail())
    {
        std::cerr << "Could not open file " << input_file_path << " for writing" << std::endl;
        return 1;
    }

    boost::filesystem::path input_path(input_file_path);
    std::string input_file_name = input_path.filename().string();
    boost::algorithm::replace_all(input_file_name, ".", "_");

    output_file << std::endl;
    output_file << "const char* " << input_file_name << " =" << std::endl;
    output_file << "{ " << std::endl;

    int c = 0;
    for(;;)
    {
        const std::char_traits<char>::int_type in = input_file.get();
        if (in == std::char_traits<char>::eof() )
            break;

        if (c == 0)
            output_file << '\t';

        output_file << in << ", ";

        c++;
        if (c == 32)
        {
            output_file << std::endl;
            c = 0;
        }
    }

    output_file << std::endl;
    output_file << "};" << std::endl;

    return 0;
}

int main(int argc, char** argv)
{
    boost::program_options::options_description description("Options");

    description.add_options()
        ("input,i", boost::program_options::value<std::string>()->required(), "input file")
        ("output,o", boost::program_options::value<std::string>()->required(), "output file")
        ("help,h", "show help message")
    ;

    boost::program_options::variables_map vm;

    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);

        if (vm.count("help"))
        {
            std::cout << description << std::endl;

            return 1;
        }

        boost::program_options::notify(vm);
    }
    catch(const std::logic_error& error)
    {
        std::cerr << error.what() << std::endl;

        return 1;
    }

    return generateHex(vm);
}
