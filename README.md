networks-project
================

File synchronization between any two terminals like in Dropbox and p2p video streaming, course project in computer Networks

DropBox like software for file synchronisation consist of a central server which manages the data for each of the users. On the server, we maintain folders for each of the registered users containing the files uploaded by the users on the server. The server maintains a database for the user names and passwords of the registered users for authenticated login. The user is allowed to upload data, download data, get the file list of his uploaded data and delete files as required.
In software for peer to peer video streaming, as soon as a client gets connected to the tracker, the tracker updates all the information regarding the client that is ip of the client and available files on the client. All the files available with the client for streaming are contained in the share folder. Every client can ask for all the files available for streaming and choose one of them. Then it fetches from the tracker the ip of the client from which it wants to stream the file. The client then connects to the fetched ip and a streaming connection is created.
