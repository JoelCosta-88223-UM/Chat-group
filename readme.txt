#READ ME

#how to install
Compile tcpserver.c and tcpclient.c


#how to create servers
Create a server by running the tcpserver executabel file with the port number (ex.: ./tcpserver.elf 5000).


#how to create clients
Create a client by running the tcpclient executabel file with the server address and port number (ex.: ./tcpclient.elf 10.42.0.1 5000)
(When the client is created there should appear a message on the server saying "New connection")


#how to use
Send message from one client to every connected client. 
Use: /send.out-"message" (ex.:/send.out-Hello)
