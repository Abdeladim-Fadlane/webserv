#include"webserv.hpp"
#include "../akatfi/parsinfCon/Configfile.hpp"

int main(int argc,char *argv[])
{
    try
    {
        if(argc != 2)
            throw std::runtime_error("Error argments");
        ConfigFile config(argv[1]);
        config.parceConfig();
        multiplexing(config);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    } 
}