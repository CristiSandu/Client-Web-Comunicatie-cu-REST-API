#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include "helpers.h"
#include "requests.h"
#include "json.hpp"
#include <iostream>
#include <string>

using json = nlohmann::json;
using namespace std; 


char *compute_get_request(char *host,char *url,string tip,
                            string cookies, string JWT,string id_carte)
{
    /*aloc memorie pentru mesaj si linia din mesaj*/
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    /* daca tip-comanda este (tip == "enter_library") || (tip == "get_books") || (tip == "logout")*/
    if ((tip == "enter_library") || (tip == "get_books") || (tip == "logout")) { 
        if (id_carte == "")//daca nu am primit nici-un id de carte 
            sprintf(line, "GET %s HTTP/1.1", url);// formez cererea cu Get si url-ul dat
        else //daca am primit un id concatenez noul id la url-ul dat
        {
            string str(url);
            string aux = str + id_carte;
            sprintf(line, "GET %s HTTP/1.1", aux.c_str());
        }

    } else if (tip == "delete_book"){ // daca tip-comanda este delete_book 
        /*concatenez url la id_carte primit si formez mesajul cu DELETE*/
        string str(url);
        string aux = str + id_carte;
        sprintf(line, "DELETE %s HTTP/1.1", aux.c_str());
    } 

    compute_message(message, line); // pun in mesaj lini formata 
    
    sprintf(line, "Host: %s", host);// formez linia de Host din mesajul de trimis la server
    compute_message(message, line);// pun in mesaj lini formata
    
    if (tip == "enter_library" || tip == "logout") { // daca tip-ul este de enter_library sau logout 
        
        //formez antetul cookie
        string rez = "connect.sid=" + cookies;

        if (cookies != "NU") { // si daca am cookie 
                sprintf(line, "Cookie: %s", rez.c_str());// formez lini de cerere pentru cookie
                compute_message(message, line);
        }
        compute_message(message, "\r\n");//si  adaug o linie libera in mesaj

    } else if (tip == "get_books" || tip == "delete_book") { // daca tip-ul este get_books sau delete_books

        /*adaug antetul pentru JWT*/
        string rez = "Bearer " + JWT;

        //cout << rez.c_str() << endl;
        
        if (JWT != "NU") {// daca am JWT-ul formez linia de Authorization din mesaj
                sprintf(line, "Authorization: %s", rez.c_str());
                compute_message(message, line);
        }

         compute_message(message, "\r\n");//si  adaug o linie libera in mesaj
    }
    
    return message; // returnez mesajul format 
}

char *compute_post_request(char *host, char *url, char* content_type, json continut,
                              int ok, string JWT)
{
    /*aloc memorie pentru mesaj ,linia din mesaj si body_data_buffer */
    char *message = (char *) calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char *)calloc(LINELEN, sizeof(char));

    /*formez linile aferente POST-ului si Host-ului cu host-ul si url dat ca parametru */
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

   /*daca ok = 0 flagul de pentru add_book e pus si atunci ne trebuie si JWT-ul pentru adaugare carte */
    if (ok == 0) {
        sprintf(line, "Authorization: Bearer %s",JWT.c_str());
        compute_message(message, line);
    }
    
    /*formam linia aferenta content-Type-ului*/
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    /*convertim json-ul folosind .dump() intr-un string */
    string cont = continut.dump();
    int len = cont.length(); // si ii calculam lungimea 

    // formama linia aferenta lungimi continutului
    sprintf(line, "Content-Length: %d", len);
    compute_message(message, line);

    // punem o linie goala 
    compute_message(message, "");

    // formam linia aferenta json-ului pentru autentificare sau informatiile despre carte 
    //care trebuie adaugata
    strcpy(body_data_buffer, cont.c_str());

    // adaugam linia in mesaj 
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    // eliberam memoria petru linie si body_data_buffer
    free(line);
    free(body_data_buffer);

    //returnam mesajul format 
    return message;
}
