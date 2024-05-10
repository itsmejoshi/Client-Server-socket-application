
#include <stdio.h>
#include <iostream>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <unistd.h>


using namespace std;

#define SERVER_PORT 2465
#define MAX_LINE 256

int main(int argc, char * argv[]) {

    struct sockaddr_in sin;
    char buf[MAX_LINE];
    char rbuf[MAX_LINE];
    int len;
    string send = "SEND";

    string acc0 = "LOGIN root root01"; //users for acceptable logins
    string acc1 = "LOGIN john john01";
    string acc2 = "LOGIN david david01";
    string acc3 = "LOGIN mary mary01";

    string login = "LOGIN"; //Commands that require client side interaction
    string logout = "LOGOUT";
    string quit = "QUIT";
    string msgstore = "MSGSTORE";
    string who="WHO";

    string comd = "c: "; //Used for proper format
    
    int s;

    bool loggedIn = false; //Used to help support use of both cases

    if (argc < 2) {
		cout << "Usage: client <Server IP Address>" << endl;
		exit(1);
    }

    /* active open */
    if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr  = inet_addr(argv[1]);
    sin.sin_port = htons (SERVER_PORT);

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("connect");
		close(s);
		exit(1);
    }
    
  while(1){
    /* main loop; get and send lines of text */
    cout << comd;
    while (fgets(buf, sizeof(buf), stdin)) {
          
          for (int i = 0; i < MAX_LINE; i++) //Unifies all commands to uppercase so that the client can enter commands in both upper and lowercase without error
          {
              buf[i] = toupper(buf[i]);
          }
   
    
		buf[MAX_LINE -1] = '\0';
		len = strlen(buf) + 1;
		::send (s, buf, len, 0);
		recv (s, rbuf, sizeof(rbuf), 0);
		cout << rbuf;
        if (strcmp(buf, quit.c_str()) == 10) //if the user quits, display message, close socket, terminate client
        {
            cout << "Client has terminated\n";
            close(s);
            exit(1);
            break;
        }

        if(strcmp(buf, send.c_str()) == 10) {
          std::string msg = string(buf).substr(5) + "\n";
          ::send(s, msg.c_str(), msg.size() + 1, 0);
          recv(s, rbuf, sizeof(rbuf), 0);
          std::cout << rbuf;
         }
        if (strcmp(buf, acc0.c_str()) == 10 || strcmp(buf, acc1.c_str()) == 10 || strcmp(buf, acc2.c_str()) == 10 || strcmp(buf, acc3.c_str()) == 10)
        {
                loggedIn = true; //sets login condition to true to support lower if-statement for msgstore
        }
        if (strcmp(buf, logout.c_str()) == 10) 
        {
                loggedIn = false;
        }
        
        if (strcmp(buf, msgstore.c_str()) == 10 && loggedIn ==true) //Allows program to unify all commands to uppercase without affecting the message entered by the user
        {
            cout << comd;
            fgets(buf, sizeof(buf), stdin);
            buf[MAX_LINE - 1] = '\0';
            len = strlen(buf) + 1;
            ::send(s, buf, len, 0);
            recv(s, rbuf, sizeof(rbuf), 0);
            cout << rbuf;
        }
       

		    break;
    }

  }
  
    close(s);
} 
 

    
 
