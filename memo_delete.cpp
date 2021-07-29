std::string     buildClientResponse(t_request &parsedRequest, const t_location *locationBlock, Config *serverConfigBlock)
{
    std::string responseToClient;

    // std::cout << strerror(errno) << std::endl;
    
    std::cout << parsedRequest.requestMethod << std::endl;
    
    if (parsedRequest.requestMethod == "GET") {
        if (parsedRequest.statusCode == "301 Moved Permanently")
        {
            responseToClient = "HTTP/1.1 301 Moved Permanently\nLocation: " + parsedRequest.location;
        }
        else
        {
            parsedRequest.pathInfoCgi = serverConfigBlock->getCgi(serverConfigBlock->getRoutes(parsedRequest.route), parsedRequest.fileExt);
            if ((parsedRequest.fileExt == ".php" || parsedRequest.fileExt == ".py") && parsedRequest.statusCode == "200 OK")
            {
                struct stat s;
                
                if (stat(const_cast<char *>(parsedRequest.pathInfoCgi.c_str()), &s) == 0)
                {
                    redirectCgiOutputToClient(parsedRequest);
                    // responseToClient = "\nHTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
                    //                     + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent + "\r\n";
                }
                else
                {
                    parsedRequest.statusCode = "500 Internal Server Error";
                    parsedRequest.fileContent = getContentFileError(serverConfigBlock, parsedRequest.statusCode);
                    parsedRequest.fileType = "text / html";
                    // responseToClient = "\nHTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
                    //                     + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent + "\r\n";
                }
            }
            else
            {
                if (parsedRequest.statusCode == "200 OK")
                    setContentDependingOnFileOrDirectory(parsedRequest, locationBlock, serverConfigBlock);
                else
                    parsedRequest.fileContent = getContentFileError(serverConfigBlock, parsedRequest.statusCode);
                // responseToClient = "HTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
                //                     + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent;
            }
        }
    }
    else if (parsedRequest.requestMethod == "DELETE") {
        struct stat s;
        int st = stat(const_cast<char *>(parsedRequest.fullPathInfo.c_str()), &s);
        if (st != 0) {
            parsedRequest.statusCode = "404 Not Found";
        }
        else {
            int fd = open(const_cast<char *>(parsedRequest.fullPathInfo.c_str()), S_IRWXU);
            if (fd == -1)
                parsedRequest.statusCode = "401 Access Denied";
            else {
                close(fd);
                if (parsedRequest.requestMethod == "DELETE" /*getmethod*/) {
                    if (remove(parsedRequest.fullPathInfo.c_str()) == -1)
                        parsedRequest.statusCode = "500 Internal Server Error";
                    else
                        parsedRequest.statusCode = "204 No Content";
                }
                else
                    parsedRequest.statusCode = "405 Method Not Allowed";
            }
        }
    }
    else if (parsedRequest.requestMethod == "POST") {
        std::cout << "METHOD POST ASKING !!" << std::endl;
    }
    else {
        std::cout << "UNEXPECTED METHOD ASKING !!" << std::endl;
    }
    responseToClient = "\nHTTP/1.1 " +  parsedRequest.statusCode + "\nContent-Type:" + parsedRequest.fileType + "\nContent-Length:" 
        + std::to_string(parsedRequest.fileContent.size()) + "\n\n" + parsedRequest.fileContent + "\r\n";
    return (responseToClient);
}
