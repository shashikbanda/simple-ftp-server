Instructions to use FTP Server:

--Compile server : gcc ftserver.c -o server
--Run server: ./server 30001

--Run Python client : python ftclient.py 30001 -l 30002

Each pass after this, the server will be running. Any commands sent through the Python client need to be sent to a different data port. For example, after the initial command above to the client ...

python ftclient.py 30001 -l 30002
python ftclient.py 30001 -g shortfile.txt 30006
python ftclient.py 30001 -l 30008
.
.
.
