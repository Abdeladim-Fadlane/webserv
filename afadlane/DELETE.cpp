#include"webserv.hpp"

void DELETE::IsFIle(Data &dataClient)
{
    if (remove(dataClient.Path.c_str()) == 0) 
    {
        dataClient.statusCode = " 204 No Content";
        dataClient.code = 204;
    } 
    else
    {
        dataClient.statusCode = "500 Internal Server Error";
        dataClient.code = 500;
    }
}

void DELETE::IsDir(Data &dataClient)
{
    DIR *dir = opendir(dataClient.Path.c_str());
    if (!dir) 
    {
        dataClient.statusCode = " 403 Forbidden";
        dataClient.code = 403;
        return;
    }
    struct dirent * it;
    while ((it = readdir(dir)) != NULL) 
    {
        if (strcmp(it->d_name, ".") != 0 && strcmp(it->d_name, "..") != 0)
        {
            std::string itPath = dataClient.Path + it->d_name;
            if (stat(itPath.c_str(), &statInfo) == 0)
            {
                if (S_ISDIR(statInfo.st_mode))
                {
                    Data subDirData = dataClient;
                    subDirData.Path = itPath + "/";
                    IsDir(subDirData); 
                } 
                else
                {
                    if (remove(itPath.c_str()) != 0)
                    {
                        dataClient.statusCode = " 500 Internal Server Error";
                        dataClient.code = 500;
                        closedir(dir);
                        return;
                    }
                }
            } 
            else 
            {
                dataClient.statusCode = " 500 Internal Server Error";
                dataClient.code = 500;
                closedir(dir);
                return;
            }
        }
    }
    closedir(dir);
    if (rmdir(dataClient.Path.c_str()) == 0)
    {    
        dataClient.statusCode = " 204 No Content";
        dataClient.code = 204;
    }
    else
    {
        dataClient.statusCode = " 403 Forbidden";
        dataClient.code = 403;
    }
}
void  DELETE::checkErrnoStat(Data &dataClient)
{
    if (errno == EACCES)
    {
        dataClient.statusCode = " 403 Forbidden";
        dataClient.code = 403;
    }
    else if(errno == ENOENT)
    {
        dataClient.statusCode = " 404 NOT FOUND";
        dataClient.code = 404;
        
    }
    else if(errno == ENOTDIR)
    {
        dataClient.statusCode = " 409 Conflict";
        dataClient.code = 409;
    }
    else 
    {
        dataClient.statusCode = " 500 Internal Server Error";
        dataClient.code = 500;
    }
}

void    DELETE::deleteMethod(Data &dataClient)
{
    if (stat(dataClient.Path.c_str(), &statInfo) != 0)
    {
        checkErrnoStat(dataClient);
        return;
    }
    if (S_ISDIR(statInfo.st_mode))
    {
        if(dataClient.Path[dataClient.Path.length() - 1] != '/')
        {
            dataClient.statusCode = " 409 Conflict";
            dataClient.code = 409;
        }
        else
            IsDir(dataClient);
    } 
    else if (S_ISREG(statInfo.st_mode)) 
        IsFIle(dataClient);
    dataClient.isDelete = true;
}