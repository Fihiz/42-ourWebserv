# TEAM FOR WEBSERV
[Le sujet](https://cdn.intra.42.fr/pdf/pdf/24190/en.subject.pdf)  

## Makefile usage

``` make exec ```  
compiling with useful flags and launching the executable with the default config file

``` make valgrind ```  
compiling and launching with valgrind


``` make ARG=fsanitize ```  
compiling and launching with fsanitize

``` make ARG=noflags ```  
compiling and launching without flags for 

⚠️ Do not launch valgrind and fsanitize at the same time


## Quelques liens utiles
[Pour s'initier à un serveur HTTP simple](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)  
[Comprendre les sockets et leurs fonctions](https://broux.developpez.com/articles/c/sockets/)  
[Découvrir les requêtes](https://www.pierre-giraud.com/http-reseau-securite-cours/requete-reponse-session/)  
[La RFC... en français](http://abcdrfc.free.fr/rfc-vf/pdf/rfc2616.pdf) 