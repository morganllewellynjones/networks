Morgan Jones
CS 432
10/29/2023

Run 'make' to compile down the project into an executable file 'proxy'.
Run proxy with -p portNumber (if you wish to use a portnumber other than 8080).
i.e. ./proxy -p portNumber

ex. use in client browser: localhost:portNumber/google.com

Close the script with a sig interrupt.

POST requests should work in theory, since the proxy mostly just updates the header and passes the request to the end server. Any body should be included and sent along with it. I have not been able to test POST requests.

The response from the server is always cached, but if the response code is not 200 the cache is removed.

The io.c file contains a lot of helper functions to make parsing strings in c a little safer and more maneagable (since almost all of c's built in str operations are destructive).

This project was developed using WSL's virtual kernel for Debian.

Unfortunately my printscrn button has stopped working. I have no idea what could have happened to it, but I am not able to generate screenshots. The code should be very runnable with the browser as a client however.

The code does not handle 301 errors, but it does avoid caching errors in general, so that you don't get trapped with a stale cache of a 404 request or the like.

Caching:
The script creates a "cache" directory and stores all cached files in that directory. It is persistent, but the make clean command should remove the cache directory and all files in it pretty conveniently. A request.http file is also cached with the most recent request stored for reference.

The script runs with no memory errors and no memory leaks, at least assuming base cases... but this is C and we are not guaranteed to have the same environment. This has not been thoroughly and deeply tested for all case instances, though I have done my best. If you encounter a segfault the code is likely to work properly on a rerun.
