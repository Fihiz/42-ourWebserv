# [ nginx.conf ]

`{ }` sont des zones qui symbolisent des contexts.
- fonction organisationnelle, logique conditionnelle
- héritage lorsque ils sont implenté sous forme parent-enfant (peuvent être modifier par les enfants)


**Le context principal/global**

Bloc se trouvant hors des autres contextes, un peu comme une variable globale.
C'est l'environnement le plus large.
Sert à définir: utilisateur, groupe d'utilisateurs, fichier log, fichier pid, processeur de travail, etc...


**Le context des évènements**

Bloc se trouvant dans le context principal.
Un seul par fichier de configuration.
Permet de gérer le traitement de connexion, comportement des processus de travail.
`epoll` permet de gérer automatiquement le plus efficace.
Permet de configurér le nombre de connexion par travailleur (mutex?, threads?).


**Le context HTTP**

Bloc se trouvant au même niveau que celui des évènements, il ne peux pas être imbriqué.
Contient la façon dont les connextions doivent être effectué.


**Le context serveur**

Bloc se trouvant dans le context HTTP.
Peut comporter plusieurs bloc server.
Algo: `listen` et `server_name` (nom dans l'entête host) sont les directives qui permettent de selectionner le bon server virtuel pour répondre au client.
Peuvent remplacer de nombreuses directives pouvant être déjà défini dans le context http.


**Le context de localisation**

Bloc se trouvant dans le context serveur. (résolution: serveur->localisation)
Permet de traiter un certain type de demande client.
Algo: définition se trouvant dans la ligne d'ouverture du bloc. Tien compte de l'URI.
(URI, partie aprés le nom de domaine ou host:port).
Peuvent être multiples et/ou imbriqués.

**Autres*

Le context upstream
Le context mail
Le context if
Le context limit_except

project-based learning github
https://github.com/EbookFoundation/free-programming-books

https://defn.io/2018/02/25/web-app-from-scratch-01/
https://joaoventura.net/blog/2017/python-webserver/
https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
https://eli.thegreenplace.net/2017/concurrent-servers-part-1-introduction/
https://ncona.com/2019/04/building-a-simple-server-with-cpp/