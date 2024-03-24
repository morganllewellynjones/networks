## Morgan Jones
## CS 432
## 11/30/2023

### Summary
The purpose of this project was to simulate a group of routers forwarding IP packets through a network. The network consists of only 6 routers, and all new packets originate from router_1 before being forwarded to other routers. The project included the forwarding tables of each router. Delays are included in the forwarding process to allow the viewer to switch between router terminals during execution, and observe logging statements as the routers move traffic. Additionally, logs are written to the following text files: 
- received_by_router_#.txt -> the router_# received a packet.
- sent_by_router_#.txt -> the router_# sent a packet.
- out_router_#.txt -> the router_# received a packet destined for themselves.
- discarded_by_router_#.txt -> the router_# discarded a packet whose TTL (time to live) was 0.

On reflection these log files should have been written to a separate directory (not the project root), but `make clean` will take care of them nonetheless.

### Operation
Step 1. Run `make` to compile all binaries.
Step 2. Run `./router_2`, `./router_3`, `./router_4`, `./router_5`, and `./router_6` in separate terminals.
Step 3. Run `./router_1`.
Step 4. Profit.

The code should run without memory errors, except for some memory leaks. For a small program with a finite endpoint such as this it shouldn't cause any problems.
The routers 2...6 do not self terminate. They continue waiting for new packets until they receive a SIGINT or you close the terminal tab. The only reason that router_1 self terminates is that in this simulation it is the "origin" router and it knows when it has exhausted its CSV packet file. The other routers are not reading from a file, they are only responding to UDP requests.

Log files are stored in txt format as specified in the file description. They are dumped in the source directory but can be removed with `make clean`.

### Notes
Since most of the code is replicated across 6 different executables, the bulk of the working code is stored in router.c.

UDP and TCP: Originally I wrote this in TCP, but I transitioned to UDP because it was simpler and felt better, and because I hadn't designed a UDP based server connection before and wanted to take it for a spin. An artifact of the TCP design is that the struct Router includes a struct Host* called "interfaces" which is meant to correspond to each of the different interfaces. This was originally used to "pass in" an array of interfaces (such as interfaceA, interfaceB, etc.) that were associated with a client or server connection on the diagram. However, after I transitioned to UDP I realized that these structs were no longer necessary or used, since there was no need to store data associated with a connection, we just "fire and forget".
