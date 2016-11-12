
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

int main(int argc, char** argv)
{
    boost::program_options::options_description description("Options");

    description.add_options()
        ("i", boost::program_options::value<std::string>(), "input file")
        ("o", boost::program_options::value<std::string>(), "output file")
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, description), vm);
    boost::program_options::notify(vm);


}
