#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

//portul la care se va face conexiunea
// pentru logica din spatele proiectului m-am documentat de pe site-ul https://medium.com/from-
// the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-
// from-scratch-d1ef8945e4fa
#define PORT 9888


int main ()
{
    int sock_d;	//descriptorul de socket
    struct sockaddr_in server;	// structura serverului
    struct sockaddr_in from; //structura browserului

    // cream un socket de tip TCP
    sock_d = socket (AF_INET, SOCK_STREAM, 0);
    /* pregatim structurile de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));
    // umplem structura folosita de server
    // stabilirea familiei de socket-uri
    server.sin_family = AF_INET;
    // acceptam orice adresa
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    // utilizam un port utilizator
    server.sin_port = htons (PORT);

    // facem bind la socket
    bind (sock_d, (struct sockaddr *) &server, sizeof (struct sockaddr));


    // ascultam daca vin clienti
    listen (sock_d, 5);

    // servim in mod concurent clientii
    while (1)
    {
    	int client;
    	int length = sizeof (from);
    	printf ("Asteptam la portul %d...\n",PORT);
    	fflush (stdout);

    	// acceptam un client
    	client = accept (sock_d, (struct sockaddr *) &from, &length);
        //facem un fork pentru fiecare client conectat pentru a servi in mod concurent clientii
    	int pid;
    	if ((pid = fork()) == -1)
        {
    		close(client);
    		continue;
    	}
        else if (0<pid)
            {
    		    // parinte
    		    close(client);
    		    while(waitpid(-1,NULL,WNOHANG));
    		    continue;
    	    }
            else if (0==pid)
                {
    		        // copil
                    char buffer[30000] = {0};
                    char page[50] = "";
                    read(client , buffer, 30000);
    		            close(sock_d);
                    char *tok;
                    char s[] = "\n";
                    char aux[200];
                    tok = strtok(buffer, s);
                    //obtinem url-ul si il sectionam pentru a obtine numele paginii dorite
                    while (tok)
                    {
                      if(sscanf(tok, "GET /%s HTTP/1.1",aux) == 1){
                        if(strcmp(aux,"favicon.ico")!=0){
                        strcpy(page,aux);
                        printf("I GOT IT, THE PAGE IS: %s\n",page);
                      }
                      }
                      tok = strtok(NULL, s);
                    }
                    char *filename = page;
                    //verificam daca pagina este de tip html sau txt pentru a alege antetele HTTP corecte
                    if(strstr(filename,".html"))
                    {
                      char *resp =
                      "HTTP/1.1 200 OK\n"
                      "Content-Type: text/html\n"
                      "Connection: close\n";
                      //trimitem header ul
                      write(client,resp,strlen(resp));
                      char buf[1000]= {0};
                      //aflam dimensiunea fisierului
                      struct stat info;
                      stat(filename,&info);
                      int filedesc = open(filename, O_RDONLY);
                      int filesize = info.st_size;
                      read(filedesc,buf,filesize);
                      //trimitem body ul
                      write(client,buf,filesize);
                    }else if(strstr(filename,".txt"))
                    {
                      char *resp =
                      "HTTP/1.1 200 OK\n"
                      "Content-Type: text/plain\n"
                      "Connection: close\n\n";
                      write(client,resp,strlen(resp));
                      char buf[1000]= {0};
                      struct stat info;
                      stat(filename,&info);
                      int filedesc = open(filename, O_RDONLY);
                      int filesize = info.st_size;
                      read(filedesc,buf,filesize);
                      write(client,buf,filesize);
                    }else
                    {
                      //aceasta va fi pagina de start
                      char *resp =
                      "HTTP/1.1 200 OK\n"
                      "Content-Type: text/html\n"
                      "Connection: close\n";
                      char *filename = "index.html";
                      write(client,resp,strlen(resp));
                      char buf[1000]= {0};
                      struct stat info;
                      stat(filename,&info);
                      int filedesc = open(filename, O_RDONLY);
                      int filesize = info.st_size;
                      read(filedesc,buf,filesize);
                      write(client,buf,filesize);
                    }
                    printf("Am trimis fisierul cu succes!\n");
                    close (client);
                    printf("Conexiune cu clientul s-a terminat!\n");
    		        exit(0);
    	        }

    }
    return 0;
}
