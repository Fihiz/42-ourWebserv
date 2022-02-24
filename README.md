# WEBSERV
[Subject](https://cdn.intra.42.fr/pdf/pdf/24190/en.subject.pdf)  

*__Purpose__*  

This project is about __writing our own HTTP server in C++ 98__ and be able to test with an actual browser.  

HTTP is one of the most used protocols on the internet. HTTP means HyperText Transfer Protocol. It is a communication protocol between a client and a server. As a request-response protocol, the client sends a request (into a navigator) to the server which will be responsible for responding instantly. HTTP clients generally use Transmission Control Protocol (TCP) connections to communicate with servers.  

A Socket allows communication between two different processes on the same or different machines. It's a way to talk to other computers using file descriptors.  

*__Makefile usage__*

``` make exec ```  
compiling with useful flags and launching the executable with the default config file

``` make valgrind ```  
compiling and launching with valgrind


``` make ARG=fsanitize ```  
compiling and launching with fsanitize

``` make ARG=noflags ```  
compiling and launching without flags for 

⚠️ Do not launch valgrind and fsanitize at the same time


## Some useful links
[To learn about a simple http server](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)  
[Understanding sockets and their functions](https://broux.developpez.com/articles/c/sockets/)  
[Discovering HTTP requests](https://www.pierre-giraud.com/http-reseau-securite-cours/requete-reponse-session/)  
[The RFC... in french](http://abcdrfc.free.fr/rfc-vf/pdf/rfc2616.pdf) 
