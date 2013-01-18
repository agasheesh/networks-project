networks-project
================

File synchronization between any two terminals like in Dropbox and p2p video streaming, course project in computer Networks

1) File synchronization between any two terminals : DropBox like software for file synchronisation consist of a central server which 
manages the data for each of the users. On the server, we maintain folders for each of the registered users containing the files uploaded 
by the users on the server. The server maintains a database for the user names and passwords of the registered users for authenticated 
login. The user is allowed to upload data, download data, get the file list of his uploaded data and delete files as required.

Instructions:

(i) Client Module:
To run the client extract the ClientModule to the folder from which data has to be uploaded to the server and execute the 
following command:

 ./client.o <IP of server> <Port of server>
e.g. >./client.o 127.0.0.1 1234

All data downloaded from the server will be stored in the receivedClient folder.

(ii) Server Module:
To run the server extract the ServerModule to the folder to which data from the users has to be uploaded. Then the following 
command is to be executed:

./server.o <Port of server>
e.g. >./server.o 1234

The usernames and passwords of registered users will be stored in "database.txt".

2) P2P video streaming : In software for peer to peer video streaming, as soon as a client gets connected to the tracker, the tracker 
updates all the information regarding the client that is ip of the client and available files on the client. All the files available 
with the client for streaming are contained in the share folder. Every client can ask for all the files available for streaming and 
choose one of them. Then it fetches from the tracker the ip of the client from which it wants to stream the file. The client then connects 
to the fetched ip and a streaming connection is created.

Instructions:

You need to run two files:

To start tracker:
	./tracker.o <port>

To start client:
	./ptpvs.o <ip_of_tracker> <port_of_tracker>

To compile streamserver.c you will need vlc library.
You are on your own now! Choose the desired options from the client menu.
