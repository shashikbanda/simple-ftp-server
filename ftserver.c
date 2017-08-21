// Server side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h> 

// Function to validate input from client: either -l OR -g <FILENAME.txt>
int validPrompt(char str[]) {
    if(strcmp(str, "-l") == 0) {
        return 1; //Return 1 for -l
    }
    else if((str[0] == '-') && (str[1] == 'g')) {
        return 2; // Return 2 for -g <FILENAME.txt>
    }
    else {
        return 0; // Return 0 for invalid.
    }
}

int main(int argc, char *argv[])
{
	int server_fd, new_socket, valread, data_socket, server_data_fd;
    struct sockaddr_in address;
    struct sockaddr_in data_addr;
    int addrlen = sizeof(address);
    int data_addrlen = sizeof(data_addr);
    char buffer[1024] = "";
    char confirmation[12] = "confirmation";
    int validPromptBool = 1;
    int ctrl_port = atoi(argv[1]);
    char data_port_str[1024] = ""; 
    int readval;
    int data_port;
    char client_hostname[1024] = "";


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //Set up for connection for control connection.
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(ctrl_port);
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
    //     perror("accept");
    //     exit(EXIT_FAILURE);
    // }

    //Keep server running always with infinite while loop.
    while(1) {
        printf("Server open on %d\n", ctrl_port);
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        //Clear string array for each pass.
        memset(&buffer, '\0', sizeof(buffer));
        memset(&client_hostname, '\0', sizeof(client_hostname));
        memset(&data_port_str, '\0', sizeof(data_port_str));
        memset(&data_addr, '\0', sizeof(data_addr));
        // valread = 0;

        //Read input from client
        valread = recv( new_socket , &buffer, sizeof(buffer), 0);

        //Send input to validator function.
        validPromptBool = validPrompt(buffer);

        //Return 0 from validator, wrong.
        if(validPromptBool == 0) {
            printf("Sorry, invalid command entered.\n");
            // return 0;
        }

        //REturn 1 from validator, means we recieved a -l command.
        else if(validPromptBool == 1) {
            // -l
            send(new_socket, &confirmation, sizeof(confirmation), 0);

            //Socket communiation.
            readval = recv(new_socket, &client_hostname, sizeof(client_hostname), 0);
            send(new_socket, &confirmation, sizeof(confirmation), 0);
            printf("Connection recived from %s.\n", client_hostname);

            readval = recv(new_socket, &data_port_str, sizeof(data_port_str), 0);
            data_port = atoi(data_port_str);

            printf("List directory requested on port %d.\n", data_port);
            printf("Sending directory contents to %s:%d.\n", client_hostname, data_port);

            //Set up data connection struct.
            data_addr.sin_family = AF_INET;
            data_addr.sin_addr.s_addr = INADDR_ANY;
            data_addr.sin_port = htons(data_port);
            send(new_socket, &confirmation, sizeof(confirmation), 0);
            
            //Create data connction socket.
            if ((server_data_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }
            //Data connection bind.
            if (bind(server_data_fd, (struct sockaddr *)&data_addr, sizeof(data_addr))<0) {
                    perror("bind failed");
                    exit(EXIT_FAILURE);
            }
            //Data connection listen.
            if (listen(server_data_fd, 3) < 0) {
                perror("listen");
                exit(EXIT_FAILURE);
            }
            //Data connection accept.
            if ((data_socket = accept(server_data_fd, (struct sockaddr *)&data_addr, (socklen_t*)&data_addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            
            //Need to send the contents of current directory.
            DIR *d;
            struct dirent *dir;
            char filename[16] = "";
            char sendMore[3] = "yes"; // Sends if more needs to be sent.
            char doneFile[2] = "no"; //Sends when all of file sent.
            d = opendir(".");
            if (d) {
                //Iterate through current directory.
                while ((dir = readdir(d)) != NULL) {
                    memset(&filename, '\0', sizeof(filename));
                    // printf("%s\n", dir->d_name);
                    strcpy(filename, dir->d_name);

                    send(data_socket, &sendMore, sizeof(sendMore), 0);
                    readval = recv(data_socket, &buffer, sizeof(buffer), 0);
                    
                    send(data_socket, &filename, sizeof(filename), 0);
                    readval = recv(data_socket, &buffer, sizeof(buffer), 0);
                }
                send(data_socket, &doneFile, sizeof(doneFile), 0);
                closedir(d);
                // close(data_socket);
            }
            // send(data_socket, &confirmation, sizeof(confirmation), 0);
            
        }
        // -g command entered.
        else if(validPromptBool == 2) {
            // -g FILENAME.txt
            char fdne[3] = "dne";
            char fde[3] = "fde";
            // printf("%s is valid input.\n", buffer);
            send(new_socket, &confirmation, sizeof(confirmation), 0);
            
            //Parse the string into the command and filename.
            char *token;
            char *search = " ";
            token = strtok(buffer, search);
            token = strtok(NULL, search); //contains filename.

            readval = recv(new_socket, &client_hostname, sizeof(client_hostname), 0);
            send(new_socket, &confirmation, sizeof(confirmation), 0);
            printf("connection recived from %s\n", client_hostname);

            // send(new_socket, &confirmation, sizeof(confirmation), 0);
            readval = recv(new_socket, &data_port_str, sizeof(data_port_str), 0);
            data_port = atoi(data_port_str);

            printf("File %s requested on port %d\n", token, data_port);

            FILE *fp;
            if (!(fp = fopen(token, "r"))) {
                /* Handle error, return, exit, try again etc. */
                printf("File %s not found. Sending error message to %s:%d\n", token, client_hostname, data_port);
                send(new_socket, &fdne, sizeof(fdne), 0);
            }
            else {
                printf("Sending file %s to %s:%d\n", token, client_hostname, data_port);
                send(new_socket, &fde, sizeof(fde), 0);
            }

            //Set up data connection.
            data_addr.sin_family = AF_INET;
            data_addr.sin_addr.s_addr = INADDR_ANY;
            data_addr.sin_port = htons(data_port);
            
            //Set up similar data connection to last seciton.
            if ((server_data_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }
            if (bind(server_data_fd, (struct sockaddr *)&data_addr, sizeof(data_addr))<0) {
                perror("data bind failed");
                exit(EXIT_FAILURE);
            }
            if (listen(server_data_fd, 3) < 0) {
                perror("listen");
                exit(EXIT_FAILURE);
            }
            if ((data_socket = accept(server_data_fd, (struct sockaddr *)&data_addr, (socklen_t*)&data_addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // char filename[100], c;
            char line[256];
            char filename[1024] = "";
            char sendMore[3] = "yes";
            char doneFile[2] = "no";


            send(data_socket, token, strlen(token), 0);
            readval = recv(data_socket, &buffer, sizeof(buffer), 0); //bs
            while (fgets(line, sizeof(line), fp)) {
                memset(&filename, '\0', sizeof(filename));
                strcpy(filename, line);

                send(data_socket, &sendMore, sizeof(sendMore), 0);
                readval = recv(data_socket, &buffer, sizeof(buffer), 0);
                
                send(data_socket, &filename, sizeof(filename), 0);
                readval = recv(data_socket, &buffer, sizeof(buffer), 0);
            }
            //Send done.
            send(data_socket, &doneFile, sizeof(doneFile), 0);
            //Clear variables.
            memset(&filename, '\0', sizeof(filename));
            memset(&token, '\0', sizeof(token));
            memset(&data_port, '\0', sizeof(data_port));
            fclose(fp);
            // close(data_socket);
        } 
    }
    return 0;
}