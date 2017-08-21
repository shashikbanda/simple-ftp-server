#!/usr/bin/python
import socket
import sys

BUFFER_SIZE = 1024

control_port = sys.argv[2]
word = sys.argv[3]
# Create a TCP/IP socket

ctrl_s = socket.socket()			
#host name of server.
host = sys.argv[1] + ".engr.oregonstate.edu"
#control port number.
port = int(control_port)

ctrl_s.connect((host, port))
if word[3] == "l":
	word = "-l"
	#cmd = str(sys.argv[3]) #valid: "-l" or "-g <INPUTFILE.txt>"
	data_port = str(sys.argv[4])

	#send command (-g, -l, ...)
	ctrl_s.send(word)
	data = ctrl_s.recv(BUFFER_SIZE) #confirmation.

	#current host name (of client).
	clienthostname = socket.gethostname()
	ctrl_s.send(clienthostname)
	data = ctrl_s.recv(BUFFER_SIZE) #confirmation.

	ctrl_s.send(data_port)
	data = ctrl_s.recv(BUFFER_SIZE)

	if data == "confirmation":
		data_s = socket.socket()
		d_port = int(data_port)
		data_s.connect((host, d_port))
		acceptData = 1
		print "Recieving directory structure from", sys.argv[1], ":", sys.argv[4]
		#print contents of directory to terminal.
		while acceptData == 1:
			data = data_s.recv(BUFFER_SIZE)
			data_s.send(word)
			if data == "no":
				break
			data = data_s.recv(BUFFER_SIZE)
			data_s.send(word)
			print data
	#close sockets.
	ctrl_s.close()
	data_s.close()

elif word[3] == "g":
	#sends -g appended with file name.
	word = "-g " + sys.argv[4]
	data_port = sys.argv[5]
	ctrl_s.send(word) 
	data = ctrl_s.recv(BUFFER_SIZE) #confirmation.

	clienthostname = socket.gethostname()
	ctrl_s.send(clienthostname)
	data = ctrl_s.recv(BUFFER_SIZE) #confirmation.
	
	ctrl_s.send(data_port)
	data = ctrl_s.recv(BUFFER_SIZE) #confirmation.
	if data == "dne":
		print sys.argv[1], ":", data_port, "says File Not Found."
	else:
		data_s = socket.socket()
		d_port = int(data_port)
		data_s.connect((host, d_port))
		acceptData = 1
		filename = data_s.recv(BUFFER_SIZE)
		data_s.send(word)
		# print "filname = ", filename
		print "Recieving", filename, "from", sys.argv[1], ":", sys.argv[5]
		fo = open(filename, "wb")
		while acceptData == 1:
			data = data_s.recv(BUFFER_SIZE)
			data_s.send(word)
			if data == "no":
				break
			data = data_s.recv(BUFFER_SIZE)
			data_s.send(word)
			#write to file.
			fo.write(data)
		#close file.
		fo.close()
		print "File transfer complete."
	#close sockets.
	ctrl_s.close()
	data_s.close()

else:
	print "Invalid command. Exiting."


