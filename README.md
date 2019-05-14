## DATA DE-DUPLICATION

This is a data de-deuplication project implementated on the basis of client server architecture

the servercli folder acts as the server and the client folder as the client ( client can interact with the server from any machine in the network )

The client just provides the server with the files it wants to store on the server.

The server is responsible for the de-duplication task.
it only saves the file if it is unique the files already on the server, if not it instead save the reference of the file which is similar

This task is accomplished by maintaining / stroing the names of file send by each separacted clients to the server along with their location on the server.

to run the server : 

**g++ -o a server -lboost_serialization -lstdc++fs**

Command line parameter for the server is the port number , such as 8000

**./a 8000**

*Note: the executable file name for server should be of one character*

to run the client :

**g++ -o cli cli.cpp**

Command line arguments for the clients are ip address of server and port number ( this should be same as that used in server )

**./cli 127.0.0.1 8000**

*Note : Since the same machine is working as the server, we use 127.0.0.1 ( loopback address ) as ip of server*

<video >
  <source src="minor.mp4" type="video/mp4">
</video>
