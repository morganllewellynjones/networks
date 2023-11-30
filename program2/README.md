Morgan Jones
CS 432
11/30/2023

Step 1. Run 'make' to compile all files.
Step 2. Run ./router_2, ./router_3, ./router_4, ./router_5, and ./router_6 in separate terminals.
Step 3. Run ./router_1.
Step 4. Profit.

The data will be stored in the respective files indicated in the assignment description.
The code should run without memory errors, but does have some memory leaks. 
For a small program with a finite endpoint such as this it shouldn't cause any problems.
The routers 2...6 do not self terminate. They continue waiting for new packets until they receive a SIGINT or you close the terminal tab.

Note for reading: Since most of the code is replicated across 6 different executables, the bulk of the working code is stored in router.c.

UDP and TCP: Originally I wrote this in TCP, but I transitioned to UDP because it was simpler and felt better, and because I hadn't designed a UDP based server connection before and wanted to take it for a spin. An artifact of the TCP design is that the struct Router includes a struct Host* called "interfaces" which is meant to correspond to each of the different interfaces. This was originally used to "pass in" an array of interfaces (such as interfaceA, interfaceB, etc.) that were associated with a client or server connection on the diagram. However, after I transitioned to UDP I realized that these structs were no longer necessary or used, since there was no need to store data associated with a connection, we just "fire and forget".
