
#include <iostream>
#include <fstream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

#include <boost/program_options/errors.hpp>

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

    const std::string input_file_path = vm["input"].as<std::string>();
    const std::string output_file_path = vm["output"].as<std::string>();

    std::fstream input_file(input_file_path, std::ios::in);

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

}
