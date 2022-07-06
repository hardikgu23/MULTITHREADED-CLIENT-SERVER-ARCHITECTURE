#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "headers.h"

/////////////////////////////
#include <iostream>
#include <assert.h>
#include <tuple>
using namespace std;
/////////////////////////////

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define ANSI_RESET "\x1b[0m"

typedef long long LL;

#define pb push_back
#define debug(x) cout << #x << " : " << x << endl
#define part cout << "-----------------------------------" << endl;

///////////////////////////////
#define MAX_CLIENTS 4
#define PORT_ARG 8001

const int initial_msg_len = 256;

////////////////////////////////////

const LL buff_sz = 1048576;
///////////////////////////////////////////////////
pair<string, int> read_string_from_socket(const int &fd, int bytes)
{
    std::string output;
    output.resize(bytes);

    int bytes_received = read(fd, &output[0], bytes - 1);
    debug(bytes_received);
    if (bytes_received <= 0)
    {
        cerr << "Failed to read data from socket. \n";
    }

    output[bytes_received] = 0;
    output.resize(bytes_received);
    // debug(output);
    return {output, bytes_received};
}

int send_string_on_socket(int fd, const string &s)
{
    // debug(s.length());
    int bytes_sent = write(fd, s.c_str(), s.length());
    if (bytes_sent < 0)
    {
        cerr << "Failed to SEND DATA via socket.\n";
    }

    return bytes_sent;
}

///////////////////////////////////////////////////

void commandHandler(string command, int fd)
{

    // cout << "Command Received " << command << endl;

    // converting string to array
    char *array;
    char *temp2;
    array = (char *)malloc(sizeof(char) * (command.size() + 1));

    for (int i = 0; i < command.size(); i++)
        array[i] = command[i];

    array[command.size()] = '\0';

    // make the token from the command seperated by space
    int no_of_args = 0;
    char *args[10];
    // let maximum length of each args to be 50
    for (int i = 0; i < 10; i++)
    {
        args[i] = (char *)malloc(50 * sizeof(char));
    }

    char *temp = strtok_r(array, " ", &temp2);
    // use strtok_r to make tokens from the command
    while (temp != NULL)
    {
        strcpy(args[no_of_args], temp);
        no_of_args++;
        temp = strtok_r(NULL, " ", &temp2);
    }

    if (no_of_args == 0)
    {
        printf("No argument received\n");
    }

    // handle the commands
    if (strcmp(args[0], "insert") == 0)
    {
        // for insert , no of argument should be 4
        if (no_of_args == 3)
        {
            int key = atoi(args[1]);

            // acquire lock of the dictionary
            pthread_mutex_lock(&mutexDict[key]);

            // insert into dictionary if it is emoty else send error
            if (dict[key] == "")
            {
                dict[key] = args[2];
                sleep(2);
                send_string_on_socket(fd, "Insertion successful");
            }
            else
            {
                sleep(2);
                send_string_on_socket(fd, "Key already exists");
            }

            pthread_mutex_unlock(&mutexDict[key]);
        }
        else
        {
            sleep(2);
            send_string_on_socket(fd, "Wrong number of argument provided");
        }
    }
    else if (strcmp(args[0], "delete") == 0)
    {
        if (no_of_args == 2)
        {
            int key = atoi(args[1]);
            // acquire lock of the dictionary

            pthread_mutex_lock(&mutexDict[key]);
            if (dict[key] == "")
            {
                sleep(2);
                send_string_on_socket(fd, "No such key exists");
            }
            else
            {
                dict[key] = "";
                sleep(2);
                send_string_on_socket(fd, "Deletion successful");
            }
            pthread_mutex_unlock(&mutexDict[key]);
        }
        else
        {
            // sleep(2);
            send_string_on_socket(fd, "Wrong number of argument provided");
        }
    }
    else if (strcmp(args[0], "update") == 0)
    {
        if (no_of_args == 3)
        {
            int key = atoi(args[1]);
            // acquire lock of the dictionary

            pthread_mutex_lock(&mutexDict[key]);
            if (dict[key] == "")
            {
                sleep(2);
                send_string_on_socket(fd, "Key does not exist");
            }
            else
            {
                dict[key] = args[2];
                sleep(2);
                send_string_on_socket(fd, args[2]);
            }
            pthread_mutex_unlock(&mutexDict[key]);
        }
        else
        {
            // sleep(2);
            send_string_on_socket(fd, "Wrong number of argument provided");
        }
    }
    else if (strcmp(args[0], "concat") == 0)
    {
        if (no_of_args == 3)
        {
            int key1 = atoi(args[1]);
            int key2 = atoi(args[2]);
            // acquire lock of the dictionary

            pthread_mutex_lock(&mutexDict[key1]);
            pthread_mutex_lock(&mutexDict[key2]);
            if ((dict[key1] == "") || (dict[key2] == ""))
            {
                // sleep(2);
                send_string_on_socket(fd, "Concat failed as at least one of the keys does not exist");
            }
            else
            {
                string a = dict[key1];
                string b = dict[key2];

                dict[key1] = dict[key1] + b;
                dict[key2] = dict[key2] + a;

            }
            pthread_mutex_unlock(&mutexDict[key1]);
            pthread_mutex_unlock(&mutexDict[key2]);

            // sleep(2);
            send_string_on_socket(fd, dict[key2]);
        }
        else
        {
            // sleep(2);
            send_string_on_socket(fd, "Wrong number of argument provided");
        }
    }
    else if (strcmp(args[0], "fetch") == 0)
    {
        if (no_of_args != 2)
        {
            int key = atoi(args[1]);

            pthread_mutex_lock(&mutexDict[key]);
            if (dict[key] == "")
            {
                sleep(2);
                send_string_on_socket(fd, "Key does not exist");
            }
            else
            {
                sleep(2);
                send_string_on_socket(fd, dict[key]);
            }
            pthread_mutex_unlock(&mutexDict[key]);
        }
        else
        {
            sleep(2);
            send_string_on_socket(fd, "Wrong number of argument provided");
        }
    }

    else
        printf("Invalid command\n");
}

//////////////////////////////////////////////////////

void *handleServerThread(void *args)
{
    while (1)
    {
        // if any fd is available in queue , then pop it else condition wait on it till it becomes available
        
        sem_wait(&semaphore);

        pthread_mutex_lock(&mutexQueue);
        int fd = que.front();
        que.pop();
        pthread_mutex_unlock(&mutexQueue);

        // read the string sent to the server from client using the fd
        char received_data[1000];
        int no_of_bytes_received;

        if ((no_of_bytes_received = read(fd, received_data, 900)) < 0)
        {
            perror("Error during receiving data from client:");
            return NULL;
        }

        // converting the array into string
        received_data[no_of_bytes_received] = '\0';

        string command(received_data);

        // cout << command << " " << fd << " " << no_of_bytes_received << endl;
        // send the string to handle
        commandHandler(command, fd);
    }

    return NULL;
}

int main(int argc, char *argv[])
{

    int i, j, k, t, n;

    // get the value of n = how many
    n = stoi(argv[1]);

    // printf("Value of n is %d\n" , n);

    // create n threads and their mutex
    pthread_t threadServer[n];

    // initialize the locks
    for (int i = 0; i < 101; i++)
        pthread_mutex_init(&mutexDict[i], NULL);

    // pthread_mutex_init(&mutexQueue, NULL);
     sem_init(&semaphore , 0 , 0);

    // initialize thread for server
    for (int i = 0; i < n; i++)
    {
        pthread_create(&threadServer[i], NULL, handleServerThread, NULL);
    }

    int wel_socket_fd, client_socket_fd, port_number;
    socklen_t clilen;

    struct sockaddr_in serv_addr_obj, client_addr_obj;
    /////////////////////////////////////////////////////////////////////////
    /* create socket */
    /*
    The server program must have a special door—more precisely,
    a special socket—that welcomes some initial contact 
    from a client process running on an arbitrary host
    */
    //get welcoming socket
    //get ip,port
    /////////////////////////
    wel_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (wel_socket_fd < 0)
    {
        perror("ERROR creating welcoming socket");
        exit(-1);
    }

    //////////////////////////////////////////////////////////////////////
    /* IP address can be anything (INADDR_ANY) */
    bzero((char *)&serv_addr_obj, sizeof(serv_addr_obj));
    port_number = PORT_ARG;
    serv_addr_obj.sin_family = AF_INET;
    // On the server side I understand that INADDR_ANY will bind the port to all available interfaces,
    serv_addr_obj.sin_addr.s_addr = INADDR_ANY;
    serv_addr_obj.sin_port = htons(port_number); //process specifies port

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* bind socket to this port number on this machine */
    /*When a socket is created with socket(2), it exists in a name space
       (address family) but has no address assigned to it.  bind() assigns
       the address specified by addr to the socket referred to by the file
       descriptor wel_sock_fd.  addrlen specifies the size, in bytes, of the
       address structure pointed to by addr.  */

    //CHECK WHY THE CASTING IS REQUIRED
    if (bind(wel_socket_fd, (struct sockaddr *)&serv_addr_obj, sizeof(serv_addr_obj)) < 0)
    {
        perror("Error on bind on welcome socket: ");
        exit(-1);
    }
    //////////////////////////////////////////////////////////////////////////////////////

    /* listen for incoming connection requests */

    listen(wel_socket_fd, MAX_CLIENTS);
    cout << "Server has started listening on the LISTEN PORT" << endl;
    clilen = sizeof(client_addr_obj);

    while (1)
    {
        /* accept a new request, create a client_socket_fd */
        /*
        During the three-way handshake, the client process knocks on the welcoming door
of the server process. When the server “hears” the knocking, it creates a new door—
more precisely, a new socket that is dedicated to that particular client. 
        */
        //accept is a blocking call
        printf("Waiting for a new client to request for a connection\n");
        client_socket_fd = accept(wel_socket_fd, (struct sockaddr *)&client_addr_obj, &clilen);
        if (client_socket_fd < 0)
        {
            perror("ERROR while accept() system call occurred in SERVER");
            exit(-1);
        }

        printf(BGRN "New client connected from port number %d and IP %s \n" ANSI_RESET, ntohs(client_addr_obj.sin_port), inet_ntoa(client_addr_obj.sin_addr));

        // add the fd tp the queue
        pthread_mutex_lock(&mutexQueue);
        que.push(client_socket_fd);
        pthread_mutex_unlock(&mutexQueue);

        // semaphore post
        sem_post(&semaphore);
    }

    close(wel_socket_fd);
    return 0;
}