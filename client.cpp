#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
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


using namespace std;
using json = nlohmann::json;


int main(int argc, char *argv[])
{
    char *message;
    char *response;
    string cookies = "NU", JWT = "NU";
    string id_carte ; 

    int sockfd;
    json credential, credential_carte;
    json JavaWT;
    char read[15];
    int port = 8080;

    char* address = (char *) "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
    char* ip_address = (char *) "3.8.116.10";
    char* root_regg = (char *) "/api/v1/tema/auth/register";
    char* root_aut = (char *) "/api/v1/tema/auth/login";
    char* root_acc = (char *) "/api/v1/tema/library/access";
    char* root_books = (char *)"/api/v1/tema/library/books";
    char* root_logout = (char *)"/api/v1/tema/auth/logout";
    
    cout << "Comenziile acceptate : register, login, enter_library, get_books, get_book, add_book, delete_book, logout, exit" << endl;

    while (1)
    {
        fgets(read, 15, stdin);
        if (!strcmp(read, "register\n")) { //daca tastaza "register" 
           
            sockfd = open_connection(ip_address,port, AF_INET, SOCK_STREAM, 0);// deschidem cnexiunea 
          
            /*formam json-ul cu username-ul si parola*/
            cout << "username= ";
                cin >> read;
            credential["username"] = read;
            
            cout << "password= ";
                cin >> read;
            credential["password"] = read;
            
            /*formez post requestul pentru inregistrarea noului cont*/
            message = compute_post_request(ip_address, root_regg, (char *)"application/json", credential, 1, "NU");    
            puts(message); // verific daca am format bine post requestul 
            /*trinit la server mesajul*/
            send_to_server(sockfd, message);
            /*primesc de la server raspunsul*/
            response = receive_from_server(sockfd);
            puts(response); // afisez raspunsul dat de server

        } else if (!strcmp(read, "login\n")) { //daca tastaza "login" 
           
            sockfd = open_connection(ip_address,port, AF_INET, SOCK_STREAM, 0);// deschidem conexiunea 

            /*daca sunt deja conectat afiseaza un mesaj de atentionare*/
            if (cookies != "NU"){
                cout << "Sunteti deja logat, pentru a va loga intr-un cont nou delogativa" << endl;
            } else {  // daca nu dormez json-ul pentru conectare
                
                cout << "username=";
                cin >> read;
                credential["username"] = read;
                cout << "password=";
                cin >> read;
                credential["password"] = read;
                
                /*formez post requestul pentru logarea in noul cont*/
                message = compute_post_request(ip_address, root_aut, (char *)"application/json", credential, 1, "NU");    
                puts(message);// verific mesajul format  
                
                /*trinit la server mesajul*/
                send_to_server(sockfd, message);
                response = receive_from_server(sockfd); // primesc de la server raspunsul 

                /*parsez raspunsul de la server sa scot cookie-ul*/            
                vector<string> parsparsRaspuns;   // definsec vectorul de stringuri in care salvez cuvinetele 
                char * delim;                       // definesc char* in care salvez delimitatorul 
                delim = strtok(response," =;");     // salvez delimitatorii
                while (delim != NULL) {             // cat timp delim nu e null  
                    string aux(delim);              // scot fiecare cuvant si il transform din char* in string
                    
                    parsparsRaspuns.push_back(aux); //pun cuvantul convertit in vector 
                    delim = strtok(NULL," =;");     //continui cu urmatoru cuvant parsat
                }

                string e_ok = parsparsRaspuns[1];   //pe pozitia 1 din vectorul de cuvinte se afla raspunsul de la server

                if (e_ok == "200" || e_ok == "201") // daca sa logat cu succes 
                {
                    cout << "Operatie realizata cu success" << endl;       
                } else {    // daca nu cere sa se introduca alt cont 
                    cout << "Eroare, introduceti un cont valid" << endl;
                }

                /*parcurg vectorul de stringuri pana cand ajung la "connect.sid" */
                for (long unsigned int i = 0; i < parsparsRaspuns.size(); i++ ) {
                    if (parsparsRaspuns[i] == "connect.sid") {  // daca am ajuns inseamna ca urmatorul string e cooki-ul 
                        cookies = parsparsRaspuns[i+1];  // il salvez in variabila cookies
                        break;                           // si ies din for 
                    }
                }
            }
            
        } if (!strcmp(read, "enter_library\n")) { // daca tastez "enter_library"
            
            sockfd = open_connection(ip_address,port, AF_INET, SOCK_STREAM, 0); // deschidem conexiunea

            // daca nu sa logat adica cookies au valoarea initiala  
            if (cookies == "NU") {
                cout << "Nu sunteti logat " << endl; // mesaj ca nu sunt logat 
            } else {
                /*formeaza get request catre server pentru a avea acces in biblioteca */
                message = compute_get_request(ip_address, root_acc, "enter_library" ,cookies, JWT,"");
              
                puts(message); // verific mesajul format 
                /*trimit la server mesajul*/
                send_to_server(sockfd, message);    
                response = receive_from_server(sockfd); // primesc raspuns de la server 
                puts(response);// si il afisez
                 


                vector<string> parsparsJWT;  // definesc vectorul de stringuri pentru parsarea JWT-ului 
                // procesul este similar cu parsarea de la cookie cu exceptia delimitatorii dupa care caut
                char * delim;                   
                delim = strtok(response,"{}");
                while (delim != NULL) {
                    string aux(delim);
                    
                    parsparsJWT.push_back(aux);
                    delim = strtok(NULL," =;");
                }

                // salvez JWT ul prsat in json-ul JavaWT
                JWT = "{" + parsparsJWT[parsparsJWT.size() - 1];
                JavaWT = json::parse(JWT);
            }
 
        } else if (!strcmp(read , "get_books\n")) {// daca tastez "get_books"
            
            sockfd = open_connection(ip_address,port, AF_INET, SOCK_STREAM, 0);// deschidem conexiunea
            
            if (JWT == "NU"){ // daca nu am primit acces la biblioteca afisez mesaj
                cout << "Nu aveti acces la biblioteca " << endl; 
            } else {// daca am primit         
                /*formeaza get request catre server pentru a avea acces in biblioteca */
                message = compute_get_request(ip_address, root_books,"get_books" ,cookies, JavaWT["token"],"");
                puts(message); // verific mesajul format 
                send_to_server(sockfd, message);    //trimit la server mesajul format 
                response = receive_from_server(sockfd); //primesc raspunsul de la server 
                puts(response); // printez raspunsul
            }
        } else if (!strcmp(read , "add_book\n")) {// daca tastez "add_book"

            sockfd = open_connection(ip_address,port, AF_INET, SOCK_STREAM, 0);// deschidem conexiunea
        
            if (JWT == "NU") { // daca nu am primit acces la biblioteca afisez mesaj
                cout << "Nu aveti acces la biblioteca " << endl; 
            } else {// daca da formez json-ul pentru carte 
                cout << "titlu=";
                    cin >> read;
                credential_carte["title"] = read;
                
                cout << "autor=";
                    cin >> read;
                credential_carte["author"] = read;
                
                cout << "gen=";
                    cin >> read;
                credential_carte["genre"] = read;
                
                cout << "editura=";
                    cin >> read;
                credential_carte["publisher"] = read;

                /*verific intr-un while infinit daca utilizatorul a introdus un numar 
                daca da trece mai depare daca nu ramana pe loc pana introduce un nr*/
                char* verify;                
                while (1) {
                    cout << "nr_pagini=";
                        cin >> read;
                    
                    strtol(read, &verify, 10);
                    
                    if(*verify == 0){
                         break;
                    }else 
                        cout << "Introduceti un numar !!"<< endl;; 
                }

                credential_carte["page_count"] = read;
                
            
                /*formez post requestul pentru inregistrarea noii carti*/
                message = compute_post_request(ip_address, root_books, (char *)"application/json", credential_carte,  0, JavaWT["token"]);    
                puts(message); // verific mesaj
                
                send_to_server(sockfd, message); //trimit mesaj la server 
                response = receive_from_server(sockfd);// primesc raspunsul de la server
                puts(response);// afisare raspuns server
            }

        } else if (!strcmp(read , "get_book\n")) {// daca tastez "add_book"
            sockfd = open_connection(ip_address,port, AF_INET, SOCK_STREAM, 0);// deschidem conexiunea

            if (JWT == "NU") {// daca JWT nu sa modificat --> mesaj nu are acces
                cout << "Nu aveti acces la biblioteca " << endl; 
            } else {// altfel
                /*salvama id carte si il trimitem in compute_get_request*/
                cout << "id_carte=" ;
                    cin >> id_carte;
                /*parsez la id_cale "/" pentru a se adauga la calea deja existenta */    
                id_carte = "/"+id_carte;
                 /*formez get requestul pentru informatiile cartii pe care vreau sa o vad */
                 message = compute_get_request(ip_address, root_books, "get_books" ,cookies, JavaWT["token"], id_carte);
                puts(message);// afisez mesajul

                send_to_server(sockfd, message);// trimit la server mesajul format 
                response = receive_from_server(sockfd); // primesc raspunsul de la server
                puts(response);// afisez mesajul de la server
            }

        } else if (!strcmp(read , "delete_book\n")) {// daca tastez "delete_book"

            sockfd = open_connection(ip_address,port, AF_INET, SOCK_STREAM, 0); // deschidem conexiunea
            
            if (JWT == "NU") {// daca nu avem JWT atunci nu avem acces la biblioteca
                cout << "Nu aveti acces la biblioteca " << endl; //si afisam mesaj
            } else {// daca da 
                /*citim id-ul cartii de sters */
                cout << "id_carte=" ;
                    cin >> id_carte;

                /*parsez la id_cale "/" pentru a se adauga la calea deja existenta */    
                id_carte = "/"+id_carte;
                 /*formez get requestul pentru stergerea cartii pe care am dat-o */
                message = compute_get_request(ip_address, root_books, "delete_book" ,cookies, JavaWT["token"], id_carte);
                puts(message);/*afisez mesajul format*/

                send_to_server(sockfd, message);/*trimit la server mesajul format */
                response = receive_from_server(sockfd);//primesc raspuns de la server 
                puts(response);//afisez raspunsul de la server 
            }

        } else if (!strcmp(read , "logout\n")) {//daca tastez "logout"

            sockfd = open_connection(ip_address,port, AF_INET, SOCK_STREAM, 0); // deschidem conexiunea
        
            if (cookies == "NU") { // daca nu am cookei pentru vre-un cont 
                cout << "Nu aveti acces la cont" << endl; // nu sunt logat 
            } else {// altfel 
                 /*formez get requestul pentru delogarea user-ului */
                message = compute_get_request(ip_address, root_logout,"logout" ,cookies, JavaWT["token"], "");
                puts(message);// printez mesajul format 

                send_to_server(sockfd, message);// trimit la server mesajul format 
                response = receive_from_server(sockfd);//primesc raspuns de la server 
                puts(response);//afisez raspunsul de la server
                /*resetez JWT-ul si cookie-ul */
                JWT = "NU";
                cookies = "NU";
            }
        } else  if (!strcmp(read , "exit\n")) {//daca tastez "exit"
            break;// break din bucla while 
        } 

    }

    free(message);       //eliberez memoria message
    free(response);      //eliberez memorie response

    return 0;
}