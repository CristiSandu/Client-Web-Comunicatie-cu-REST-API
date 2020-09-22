# Client Web. Comunicatie cu REST API.

Proiectul contine ca si fisiere: buffer.cpp, buffer.h, requests.cpp, requests.h, client.cpp, 	helpers.cpp,  helpers.h .

In fiserul **client.cpp** am implementat clientul propriuzis celelate fisiere avand rol ajutator. 

In principiu secheletul este similar cu cel prezent in laboratorul 10, cu exceptia modificarilor aduse functiilor compute_get_request, compute_post_request pentru a putea compune noile tipuri de mesaje aferente cerintelor.

Ca si json am folosit nlohmann prezent in indrumar pentru c++. Din acest json am folosit functiile .dump pentru a transforma json-ul intr-un string care urmeaza sa fie trimis 
la server si functia parse pe care am folosit-o la transformarea stringului pe care l-am scos din raspunsul pentru comanda enter_library in care se afla JWT. 
Am foloit-o din prisma faptului ca este usor de folosit, astfel usurandu-mi munca de formare a mesajululi de transmis la server pentru logare si creare carte noua si parsarea JWT primit de la server.

La implementarea clientului am definit initial adresele url pe care trebuie sa le trimita pentru fiecare comanada in parte. Am mai definit 2 stringuri in care salvez cookie-ul primit la logarea in server si JWT dupa comanda de enter_library. Pentru datele de logare, datele aferente noii carti si pentru JWT primite de la server am folosit niste variabile de tip json.
Dupa intr-o bucla while cat timp nu dau break, citesc de la tastatura cu fgets comenziile si daca se potrivesc cu comenziile comenziile acceptat afisate la pornirea programului merg pe ramura aferenta fiecarei comenzi.
Pe toate ramurile initial deschid conexiunea si peurma formez cererea catre server aferenta fiecarei comenzi si astept un raspuns pe care il afisez la stdout. La aceste opereati folosesc functiile din laborator (din helpers.cpp).
Functiile care formeaza mesajul de trimis le-am modificat pentru a putea primi toate datele neceare fiecarei cereri.  
functia de compute_post_request -> primeste ca si parametri adresa ip a serverului, URL-ul, content_type-ul, json-ul format aferent user-ului si parolei pentru logarre si registrare sau json-ul detaliile unei carti la add_book, un flag care ne indica daca avem comanda de add_book ca sa adauge JWT  de autorizare acces biblioteca si JWT in format string pe care il trimitem cu autorizatia.
functia de compute_get_request  -> primeste ca si parametri adresa ip a serverului, URL-ul, numele comenzii primite pentru formarea diferentiata pentru fiecare comanda in parte, cookie-ul de acces, JWT in format string, id_carte pentru completarea url-ului la get_book si delete_book pentru celelalte este "". 
In aceste 2 functii formez in functei de comanda primita mesajul corespunzator fiecarei cereri.
Pentru comneziile enter_library si login unde trebuie sa parsez raspunsul primit pentru a scoate cookie-ul si JWT necesar comenziilor aferente bibliotecii si accesului in biblioteca am folosit pentru parsare strtok iar rezultatul de dupa parsare il salvez intr-un vector de stringuri. 
Pentru scoaterea cookie-ul parcurg vectorul pana la casirea stringului "connect.sid" iar apoi salvez urmatorul element in stringul aferent cookie-ul 
Pentru scoaterea JWT procesul scot stringul de pe ultima pozitie la care concatenez paranteza de inceput , iar rezultatul in salvez in stringul aferent JWT-ului.
Pentru a evita erorile am pus conditie de logare si de existare a JWT sau a cookie-ului. Daca acesta nu exista pentru comenziile la care ni se cere exista o comparatie care afiseaza un mesaj corespunzator care atentioneaza user-ul. Pe langa astea am mai pus o verificarea la adaugare carte daca pentru campul aferent nr pagini nu introduce un numar programul nu trece mai departe. Pentru verificare am folosit functia strtol intr-un while (1).
La comanda de logout restaurez string-urile pentru JWT si cookies la valoarea initiala dupa logare astfel daca user-ul nu mai e logat nu va putea da alte comenzi inafara de register,login sau exit, pentru celelalte primind un mesaj corespunzator.
Prin tastarea comenzii exit se face un break care iese din loop.
La final se elibereaza memoria aferenta mesajului la server si raspunsului de la server.

### [Cerinte](https://github.com/cristysandu/Client-Web-Comunicatie-cu-REST-API/blob/master/Tema_3_enunt.pdf)
