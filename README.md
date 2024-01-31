Technologies used
  The server is TCP, concurrent, the fork function being called every time a client connects to the server.
  
Implementation details
    A socket is created, the server structure is prepared and then bind is done to attach the socket. The server remains blocked until the customers come due
    the listen() function call.
    Concurrent serving of clients is done with the fork() function. Every time when
    a client is accepted, a fork() is made, the child having the role of taking over the page
    desired by the browser, to check the file extension (if it is .http or .txt) and
    to send the correct HTTP headers using the write() function.
    The client connects to the server by typing in the browser localhost:PORT (number
    the defined port) and then the requested file will be received and then displayed.
