# Question 3
## To Run
```g++ client.cpp -o client -lpthread```
```g++ server.cpp -o server -lpthread```

## Server Program

<br> First stores the number of worker thread that can be made and initialize all the threads and the locks.
<br>
<br>bind: After the creation of the socket, the bind function binds the socket to the address and port number specified in addr.
<br>listen: It puts the server socket in a passive mode, where it waits for the client to approach the server to make a connection.
<br> Accept call gets the fd of the request send by client , which is then added to the queue. Semaphore post is made so that any worker thread that is free can handle the it.
<br> Each thread reads the data sent by the client using fd (string data) and send it to commandHandler fxn. 
<br> Command handler fxn tokenize it accordingly and the corresponding actions are performed.
<br> Mutex locks on dict values are used to avoid updation of values simultaneously.
<br> result is send back to the client using send_string_on_socket.

## Client Program
<br> The program first takes the input and store all the requests in an string array.
<br> It creates thread equal to number of request. Each threads handle 1 request.
<br> It sends the request to the server using send_string_on_socket fucntion after the time mentioned time. 
<br> It received the output from the server using read_string_from_socket function.
<br>This program first takes the input about total number of requests and information associated with each request.
