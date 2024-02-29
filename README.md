#  A Client-Server Socket Application
This programming assignment focuses on building a client-server socket application using Java. The application implements the "Yet Another Message of the Day" (YAMOTD) protocol, allowing clients to communicate with a server via TCP sockets.
In this project, you will create both a server and a client process that interact using the YAMOTD protocol. The server is responsible for several functions:
Providing a "message of the day" (MOTD) to clients when they send a MSGGET command.
Allowing authenticated users to upload MOTD messages using MSGSTORE, which are stored on the server and can be retrieved by other clients.
Allowing the "root" user to send a SHUTDOWN command to gracefully close the server.
Verifying user identities through LOGIN.

## Pre-requisites
Java 
A development environment like Visual Studio Code or any Java IDE

## How to run
Launch the server first, followed by the client. During development, you can use the localhost address (127.0.0.1) for testing.

## Running
Run the `server`  first in your terminal, then run the `client` .with localhost address `127.0.0.1` for testing.

### Command Outputs
#### MSGGET
Use this command to request the "Anyone who has never made a mistake has never tried anything new" from the server.
The server retrieves messages from its message store file and returns them in chronological order.
```
Enter a command: MSGGET
200 OK
Anyone who has never made a mistake has never tried anything new
```

#### SHUTDOWN
The SHUTDOWN command allows the root user to gracefully shut down the server.
Upon successful execution, both the client and server programs terminate.
Note: Only users with root access can use this command.
```
Enter a command: SHUTDOWN
402 User not allowed to execute this command
```

#### LOGIN
The LOGIN command enables a client to log in to the server by providing a username and password.
If the credentials are valid, the user gains access; otherwise, they receive an error message indicating an incorrect username or password.
```
Enter a command: login john john01
200 OK
```
Available users and commands
```
login root root01";
login john john01";
login david david01
login mary mary01";
```

#### LOGOUT
The LOGOUT command allows a logged-in user to log out from the server.
If no user is logged in, the server responds with a message stating that no users are currently logged in.
```
Enter a command: LOGOUT
200 OK
```

#### QUIT 
The QUIT command allows a client to terminate its connection to the server.
The server remains operational and continues to accept connections from other clients.
shell

```
Enter a command: QUIT
200 OK
```

#### MSGSTORE
Use the MSGSTORE command to upload a message to the server.
The server stores the message in its messages.txt file, with a maximum limit of 20 messages.
Users can only store a message when they are logged in.
```
Enter a command: MSGSTORE message
200 OK
```
