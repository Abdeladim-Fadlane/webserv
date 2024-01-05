#include"webserv.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

struct LocationConfig
{
    std::vector<std::string>index;
    std::string upload;
    std::string path;
    std::string root;
    std::string autoindex;
    std::vector<std::string> methods;
    std::vector<std::string> cgiPath;
    std::string proxyReadTimeout;
};
int flag = true;

struct ServerConfig
{
    int listen;
    std::string host;
    std::string root;
    std::string clientMaxBodySize;
    std::vector<LocationConfig> locations;
};
void printInfo(std::vector<ServerConfig> &servers)
{
    // std::cout << "Number of servers: " << servers.size() << std::endl;
    for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        std::cout << "Listen: " << it->listen << std::endl;
        std::cout << "Host: " << it->host << std::endl;
        std::cout << "root: " << it->root << std::endl;
        std::cout << "client max size: " << it->clientMaxBodySize << std::endl;
        for (std::vector<LocationConfig>::iterator locIt = it->locations.begin(); locIt != it->locations.end(); locIt++)
        {
            std::cout << "--------------------------------------------------->" << std::endl;
            std::cout << "PathLocation: " << locIt->path << std::endl;
            std::cout << "autoindex: " << locIt->autoindex << std::endl;
            std::cout << "cgiPath: " << locIt->cgiPath[0] << std::endl;
            std::cout << "cgiPath 2: " << locIt->cgiPath[1] << std::endl;
            std::cout << "root: " << locIt->root << std::endl;
            std::cout << "proxyReadTimeout: " << locIt->proxyReadTimeout << std::endl;
            std::cout << "methods :" << locIt->methods[0] << std::endl;
            std::cout << "methods 2 :" << locIt->methods[1] << std::endl;
            std::cout << "index :" << locIt->index[0] << std::endl;
            std::cout << "index  2 :" << locIt->index[1] << std::endl;
            std::cout << "upload  :" << locIt->upload << std::endl;
            std::cout << "--------------------------------------------------->" << std::endl;
        }
    }
}

void splitArguments(std::istringstream &wiss, std::vector<std::string>& result)
{
    std::string token;
    while (wiss >> token)
    {
        result.push_back(token);
    }
}

void    checkStreamEmty(std::istringstream &wiss)
{
    if(wiss.peek() != EOF || !wiss.fail())
        throw std::runtime_error("Error block ..location..");
}
/* Don't repeat yourself. */
bool parseLocationBlock(LocationConfig &location,std::ifstream &file)
{ 
    std::string line;
    std::string key ,data;
    while(std::getline(file,line))
    {
        std::istringstream wiss(line);
         int bracketCount = 0;
        while (wiss >> key)
        {   
            // std::cout<<"<<<-"<<key<<"->>>\n";
            if (key == "}")
                return true;
            else if (key == "autoindex" && location.autoindex.empty())
            {
                wiss >> location.autoindex;
                checkStreamEmty(wiss);
            }
            else if (key == "upload" && location.upload.empty())
            {
                wiss >> location.upload;
                if(location.upload != "on" && location.upload != "off")
                    throw std::runtime_error("Error location block2");
                checkStreamEmty(wiss);
            }
            else if (key == "path" && location.path.empty())
            {
                wiss >> location.path ;
                checkStreamEmty(wiss);
            }
            else if (key == "proxy_read_time_out" && location.proxyReadTimeout.empty())
            {
                wiss >> location.proxyReadTimeout ;
                checkStreamEmty(wiss);
            }
            else if (key == "root" && location.root.empty())
            {
                wiss >>  location.root ;
                checkStreamEmty(wiss);
            }
            else if (key == "index" && location.index.empty())
                splitArguments(wiss,location.index);
            else if (key == "methods" && location.methods.empty())
                splitArguments(wiss,location.methods);
            else if (key == "cgi_path" && location.cgiPath.empty())
                splitArguments(wiss,location.cgiPath);
            else
                throw std::runtime_error("Error./.../ location block");
        }   
    }
    return false;
}

bool parseServerBlock( ServerConfig &server,std::ifstream &file)
{
    std::string line;
    /* Don't touch it if work !!! */
    std::string key;
    // std::cout<<server.listen <<"\n";
    int bracketCount = 0;
    while(getline(file,line))
    {
        std::istringstream wiss(line);
        while (wiss >> key)
        {
            //  std::cout<<key<<"\n";
            if (key == "{" )
            {
                bracketCount++;
                continue;
            }
            if (key == "}" )
            {
                bracketCount--;
                if (bracketCount < 0)
                {
                    throw std::runtime_error("curly brackets in server block");
                }
                return true;
            }
            else if (key == "listen" && server.listen == 0)
            {
                wiss >> server.listen;
                checkStreamEmty(wiss);
                if(server.listen <= 0 || server.listen >= 65000)
                    throw std::runtime_error("Eroor port");
            }
            else if (key == "host" && server.host.empty())
            {
                wiss >> server.host;
                checkStreamEmty(wiss);
            }
            else if (key == "root" && server.root.empty())
            {
                wiss >> server.root ;
                checkStreamEmty(wiss);
            }
            else if (key == "client_max_body_size" && server.clientMaxBodySize.empty())
            {
                wiss >> server.clientMaxBodySize;
                    checkStreamEmty(wiss);
            }
            else if(key == "location")
            {
                LocationConfig location;
                if(parseLocationBlock(location,file) == true)
                {
                    server.locations.push_back(location);
                    location = LocationConfig();
                }
                else
                    throw std::runtime_error("Error location block... 1");
            }
            else 
                throw std::runtime_error("Error server block ...2");
        }
    }
    if (bracketCount != 0)
    {
        throw std::runtime_error("curly brackets in server block");
    }
    return false;
}

void parseConfigFile(const std::string &fileName, std::vector<ServerConfig> &servers)
{
    std::ifstream file(fileName.c_str());
    if (!file.is_open()) {
        std::cout << "Error in file\n";
        return;
    }
    /* This is the way !! */
    std::string line;
    ServerConfig server;
    std::string token ;
    while (std::getline(file, line))
    {
        std::istringstream wiss(line);  
        while(wiss >> token)
        {
            // std::cout<<"<<"<< token <<">>\n";
            if(token == "server")
            {
                if (parseServerBlock(server,file) == true)
                {
                    servers.push_back(server);
                    server = ServerConfig();
                }
                else
                    throw std::runtime_error("Error Server block aaa");
            }
            else if(token == "{")
                continue;
            else if(token == "}")
            {
                break;
            }
            else
                throw std::runtime_error("Error Server block ..... ");
        }
    }

    file.close();
}

int main(int ac ,char *av[])
{
    /* Clean code !!!!!!! */
    try
    {
        if(ac > 2)
            throw std::runtime_error("Error agements .");
        std::vector<ServerConfig> servers;
        const char *file = "webserv.conf";
        if(ac == 2)
            file = av[1];
        parseConfigFile(file ,servers);
        printInfo(servers);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }
}
