#Morgan Jones
#CS 432
#10/29/2023

##Summary
The project objective was to build a simple web proxy server. This proxy server listens on localhost for client requests. If a specific web page is requested it checks its cache for that webpage and serves it, or (in the event that the file is not cached) it queries the appropriate server for the HTTP page to serve and then caches the file.

##Development
This project was developed in about 2 weeks of part time work. I rejected the Python skeleton code given to me for this project in favor of developing it in C. My reasoning was that a low level project should use a low-level language. I was interested in learning the nuts and bolts of web socket interaction, and thought that C would be a more suitable language for this.

The tested development environment for this project is Debian 11 on WSL.

##Compilation
Run 'make' to compile down the project into an executable file. The program can be executed through the binary file proxy.

##Operation
- Run proxy with -p portNumber (if you wish to use a portnumber other than 8080).
- i.e. ./proxy -p portNumber
- Query the proxy with a client, most likely a browser on localhost.
- i.e. localhost:portNumber/google.com
- The proxy will query the given server and return the HTTP contents, caching a successful result.
- Close the script with sig interrupt.

##Utilities
The io.c file contains a lot of helper functions to make parsing strings in C a little safer and more maneagable (since almost all of C's built in str operations are destructive).

##Caching
The script creates a "cache" directory and stores all cached files in that directory. It is persistent, but the make clean command should remove the cache directory. A request.http file is also cached with the most recent request stored for reference.
The response from the server is always cached, but if the response code is not 200 the cache is removed.
The code does not have specialized error handling, but it does avoid caching errors in general, so that you don't get trapped with a stale cache of an errored request (such as a 404 request).

