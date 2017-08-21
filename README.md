### Program Description

This is my implementation of a simple FTP server where the client can do two things:
- Request the directory contents of the server (-l)
- Transfer a text file from server (-g <FILENAME>)

The server is written in C. The client is written in Python. Network communication is handled with the Sockets API.

Note: Current implementation works on Oregon State University Engineering servers.

### Instructions to use FTP Server:

- Compile server : gcc ftserver.c -o server
- Run server: ./server <SERVER PORT>

- Run Python client : 
- To get directory : python ftclient.py <SERVER PORT> -l <DATA PORT>
- To transfer file : python ftclient.py <SERVER PORT> -g <FILE TO TRANSFER> <DATA PORT>

Each pass after this, the server will be running. Any commands sent through the Python client need to be sent to a different data port. For example, after the initial command above to the client ...

Example: 

python ftclient.py 30001 -l 30002
python ftclient.py 30001 -g shortfile.txt 30006
python ftclient.py 30001 -l 30008
.
.
.

Note: In FTP, it takes two TCP connections to transfer a file. The first is the control connection. This is to look through the file system of server. The second is the data connection to transfer the file on. <DATA PORT> is where we are transferring the file to on the client's machine.
