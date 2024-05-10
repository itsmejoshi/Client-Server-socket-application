#include <iostream>
#include <fstream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <queue>
#include <vector>
#include <string>
#include <utility>
#include <cstring>
#include <cstdlib>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <cstring>
#include <map>




#define SERVER_PORT 2465
#define MAX_LINE 102400
#define MAX_PENDING 5

using namespace std;

std::vector<std::pair<std::string, string>> activeUsers;// UserID, IP
std::map<std::string, int> socketMap;
std::queue<std::string> messages;
std::mutex mutex;
std::condition_variable cv;

// Other specific properties for program 1 functionality...
// Assuming all properties are here.


string convertToString(char* a, int size)
	{
		string strng = a;
		return strng;
	}

std::string extractUserName(const char* buf) {
    std::string command(buf);
    std::size_t start =  command.find("SEND ") + strlen("SEND "); // Find the end of "SEND "
    std::size_t end = command.find(' ', start); // Find the space after the username
    return command.substr(start, end - start); // Return the username
}


void handleClient(int new_s, std::string clientip) {
	
    char buf[MAX_LINE];
	char get[MAX_LINE];
	int len;
	int size;
	int s;
	string username;
	



	string login = "LOGIN"; //user commands
	string logout = "LOGOUT";
	string quit = "QUIT";
	string msgget = "MSGGET";
	string msgstore = "MSGSTORE";
	string shtdown = "SHUTDOWN";
	string who = "WHO";
	//pair<string, string> user;
	pair<string, string> user_log;
	

	string messages[200]; //stores the messages from txt file in array
	string temp;
	string temp1;
	string addmsg;

	int clientSocket = new_s;
	pair<string, string> user{string(clientip), string(clientip)};
	activeUsers.push_back(user);
	socketMap[string(clientip)] = clientSocket;
	
	

	int itotal = 0; //limiter for number of messages stored
	int i = 0;

	ifstream ifile;
	ofstream ofile;

	bool loggedIn = false; //conditions
	bool shutdown = false;
	bool rootLogIn = false;

	string acc0 = "LOGIN root root01"; //users for acceptable logins
	string acc1 = "LOGIN john john01";
	string acc2 = "LOGIN david david01";
	string acc3 = "LOGIN mary mary01";

	string rec0 = "SEND john"; //users for acceptable logins
	string rec1 = "SEND root";
	string rec2 = "SEND david";
	string rec3 = "SEND mary";


	ifile.open("messages.txt");
	int j = 0;
	if (ifile.is_open()) //opens txt file and grabs each line from the file, stores that line in the array
	{
		getline(ifile, messages[j]);
		messages[j] += '\n';
		messages[j] += '\0';
		while (ifile)
		{
			++j;
			++itotal; //keeps track of the total number of messages stored
			getline(ifile, messages[j]);
			messages[j] += '\n';
			messages[j] += '\0';
		}
	}
	ifile.close();

	ofile.open("messages.txt", ios::app); //ensures that when msgstore is called, message added on newline
	ofile << "\n";
	ofile.close();

    // Read data from the client
    // if (recv(new_s, buf, sizeof(buf), 0) <= 0) {
    //     close(new_s);
    //     return;
    // }

    std::string message(buf);
	string send = "SEND";
	message = message.substr(0, message.find_first_of("\r\n"));
	


			while(len = recv(new_s, buf, sizeof(buf) - 1, 0))
			{
				
				buf[len] = '\0';
				
				cout << buf;

				if (strcmp(buf, who.c_str()) == 10) //WHO Command Begins
				{

					
					temp = "s: 200 OK\nThe list of the active users:\n"; // Start response
					// Iterate over the active users vector and add each one to the response
					for (const auto& user : activeUsers) {
						temp += user.first + " " + user.second + "\n";
					}
					// Copy response to buffer and send
					strcpy(buf, temp.c_str());
					buf[temp.size()] = '\0';
					::send(new_s, buf, strlen(buf) + 1, 0);
					

					// No break statement, so the server will continue to handle other commands
				} //WHO Command End
				else if (strcmp(buf, msgget.c_str()) == 10) //MSGGET Command Begins
				{
					
					temp = "s: 200 OK\n";
					temp += messages[i];
					//temp += "\n";
					strcpy(buf, temp.c_str());
					i++;
					if (i == itotal) //resets incrementer for msgget calls
					{
						i = 0;
					}
					::send(new_s, buf, strlen(buf) + 1, 0);
					
					//break;
				} //MSGGET Command Ends

				else if (strcmp(buf, msgstore.c_str()) == 10) //MSGSTORE Command Begins
				{
					if (loggedIn == false) //error condition if user is not logged in
					{
						temp = "s: 401 You are not currently logged in, login first";
						temp += "\n";
						strcpy(buf, temp.c_str());
						::send(new_s, buf, strlen(buf) + 1, 0);
						//break;
					}
					if (loggedIn == true)
					{
						temp = "s: 200 OK\n";
						temp += "s: Enter a message of the day\n"; //added message for user friendliness
						strcpy(buf, temp.c_str());
						::send(new_s, buf, strlen(buf) + 1, 0); //sends 200 OK, then waits to  receive the user's message
						recv(new_s, get, sizeof(get), 0);
						size = sizeof(get) / sizeof(char);
						addmsg = convertToString(get, size); //converts the message into a string and stores it in a variable
						
						ofile.open("messages.txt", ios::app);
						ofile << addmsg; //writes the user's message at the first available newline
						ofile.close();
						messages[itotal] = addmsg;
						itotal++; //increments since there is a new total
						cout << addmsg;
						temp = "s: 200 OK\n";
						strcpy(buf, temp.c_str());
						::send(new_s, buf, strlen(buf) + 1, 0);
						//break;
					} //MSGSTORE Command Ends
					
				}
				else if (string(buf).substr(0, send.size()) == send) {
					if (!loggedIn) {
						std::string err = "s: 420 You are not currently logged in, login first\n";
						::send(new_s, err.c_str(), err.size() + 1, 0);
					} 
					else {
						std::string recipient = extractUserName(buf);  // You would have to implement 'extractUserName'

						// find the recipient user in the activeUsers list
						auto recipientUser = std::find_if(
							activeUsers.begin(),
							activeUsers.end(),
							[&recipient](const std::pair<std::string, std::string>& user) {  // assuming the second value of the pair has the socket of the user
								return user.first == recipient;
							});

					// if the recipient user is not active
							if (recipientUser == activeUsers.end()) {
								std::string err = "s: 420 The user does not exist or is not logged in\n";
								::send(new_s, err.c_str(), err.size() + 1, 0);
							} 
							else {
								// The user is found, forward the message (stored in 'buf') to designated user
								::send(socketMap[recipientUser->first], buf, strlen(buf) + 1, 0);
							}
   				 }
				}
				else if (strcmp(buf, acc1.c_str()) == 10 || strcmp(buf, acc2.c_str()) == 10 || strcmp(buf, acc3.c_str()) == 10)//LOGIN Command Begins
				{
						temp = "s: 200 OK\n"; 
						loggedIn = true; //for users that are not the root
						
						
						// update activeUsers list with the logged in user
						std::string username = string(buf).substr(6);

						
						
						
						string bufStr(buf);
						size_t firstSpacePos = bufStr.find(' ');
						size_t secondSpacePos = bufStr.find(' ', firstSpacePos + 1);
						username = bufStr.substr(firstSpacePos + 1, secondSpacePos - firstSpacePos - 1);
						
    					

						for (auto& activeUser : activeUsers) {
       						 if (activeUser.second == user.second) {
           						 activeUser.first = username;
            					break;
       						 }
  					    }

    					
						strcpy(buf, temp.c_str());
						::send(new_s, buf, strlen(buf) + 1, 0);
						//break;
				}
				else if (strcmp(buf, acc0.c_str()) == 10) //if the user is the root
				{
				
					temp = "s: 200 OK\n";
					strcpy(buf, temp.c_str());
					::send(new_s, buf, strlen(buf) + 1, 0);
					loggedIn = true;
					rootLogIn = true;
					//break;


					std::string username = string(buf).substr(6);

					string bufStr(buf);
					size_t startPos = bufStr.find(' ', 6);
					size_t endPos = bufStr.find(' ', startPos + 1);
					username = bufStr.substr(startPos + 1, endPos - startPos - 1);
    				pair<string, string> user{username, string(clientip)};
    				activeUsers.push_back(user);

				}
				else if (strcmp(buf, login.c_str()) == 32) //error case for any other users
				{
						temp = "s: 410 Wrong UserID or Password\n";
						strcpy(buf, temp.c_str());
						::send(new_s, buf, strlen(buf) + 1, 0);
						//break;
						
				}//LOGIN Command Ends

				else if (strcmp(buf, logout.c_str()) == 10) //LOGOUT Command Begins
				{
					if (loggedIn == true)
					{
						loggedIn = false; //user is logged out, preventing user from using msgstore
						temp = "s: 200 OK\n";
						strcpy(buf, temp.c_str());
						::send(new_s, buf, strlen(buf) + 1, 0);
					}
					if (rootLogIn)
					{
						rootLogIn = false; //added condition that has to be changed if user is the root
					}
				} //LOGOUT Command Ends

				// else if (strcmp(buf, who.c_str()) == 10) //WHO Command Begins
				// {

				// 	cout << "I am in who";
				// 	temp = "s: 200 OK\nThe list of the active users:\n"; // Start response
				// 	// Iterate over the active users vector and add each one to the response
				// 	for (const auto& user : activeUsers) {
				// 		temp += user.first + " " + user.second + "\n";
				// 	}
				// 	// Copy response to buffer and send
				// 	strcpy(buf, temp.c_str());
				// 	buf[temp.size()] = '\0';
				// 	send(new_s, buf, strlen(buf) + 1, 0);
				// 	break;

				// 	// No break statement, so the server will continue to handle other commands
				// } //WHO Command End

				else if (strcmp(buf, quit.c_str()) == 0) //QUIT Command Begins
				{
					temp = "s: 200 OK\n";
					strcpy(buf, temp.c_str()); //simply sends 200 OK, termination process on client side 
					::send(new_s, buf, strlen(buf) + 1, 0);
					activeUsers.erase(remove(activeUsers.begin(), activeUsers.end(), user), activeUsers.end());
					break;
				} //QUIT Command Ends

				else if (strcmp(buf, shtdown.c_str()) == 10) //SHUTDOWN Command Begins
				{
					if (rootLogIn) //only can be used if the user is the root
					{
						temp = "s: 200 OK\n";
						strcpy(buf, temp.c_str());
						::send(new_s, buf, strlen(buf) + 1, 0);
						shutdown = true;
						cout << "System has shutdown\n"; //outputs message, closes socket, terminates server
						close(new_s);
						exit(1);
						//
						break;
					}
					else {
						temp = "s: 402 User not allowed\n";
						strcpy(buf, temp.c_str());
						::send(new_s, buf, strlen(buf) + 1, 0);
						//break;
					}
				} //SHUTDOWN Command Ends

				else{
					temp = "s: Enter a valid command\n"; //error message if user enters any other command
					strcpy(buf, temp.c_str());
					::send(new_s, buf, strlen(buf) + 1, 0);
					//break;
				}
				memset(buf, 0, sizeof(buf));
			}

		socketMap.erase(username);	
		activeUsers.erase(remove(activeUsers.begin(), activeUsers.end(), user_log), activeUsers.end());
		close(new_s);

	
	

		

    // // Handle different commands
    // // Considering command logic from both programs...
    // if (message.substr(0, 3) == "WHO") {
    //     // Implement WHO command logic
    // } else if (message.substr(0, 4) == "SEND") {
    //     // Implement SEND command logic
    // } else if (message.substr(0, 5) == "LOGIN") {
    //     // Implement LOGIN command logic
    // } else if (message.substr(0, 7) == "MSGGET") {
    //     // Implement MSGGET command logic
    // }else if (message.substr(0, 8) == "SHUTDOWN") {
    //     // Implement SHUTDOWN command logic
    // }

  
}


int main(int argc, char** argv) {
    struct sockaddr_in sin;
    socklen_t addrlen;
	struct sockaddr_in clientInfo;
	char* clientip = new char[INET_ADDRSTRLEN];
	int clientPort;
    int new_s;
	int s;


    /* build address data structure */
    bzero((char*)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);
	

    /* setup passive open */
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket");
        exit(1);
    }



    if (::bind(serverSocket, (struct sockaddr*) & sin, sizeof(sin)) != 0) {
    	perror("bind");
    	exit(1);
	}

	listen(serverSocket, MAX_PENDING);
    // Listen for connections
    // if (listen(serverSocket, 10) == -1) {
    //     perror("Error listening");
    //     close(serverSocket);
    //     return 1;
    // }
    
    addrlen = sizeof(sin);
    cout << "The server is up, waiting for connection" << endl;

    while (1) {
        // Accept a connection
        if ((new_s = accept(serverSocket, (struct sockaddr*) & sin, &addrlen)) < 0) {
            perror("accept");
            exit(1);
        }

		inet_ntop(AF_INET, &clientInfo.sin_addr, clientip, INET_ADDRSTRLEN);
  		clientPort = ntohs(clientInfo.sin_port);
		

		int clientSocket = new_s;
    	std::pair<std::string, int> user{string(clientip), clientSocket}; 
		
		// pair<string, string> user{string(clientip), string(clientip)};
    	// activeUsers.push_back(user);
		

	    
        // Handle the client in a separate thread
       std::thread threadObj(handleClient, new_s, clientip);
	   threadObj.detach();
    }

    
}