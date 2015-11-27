#SharedMemoryServerClient
Simple server for sending files from client to server through shared memory. 
Name gets from COM component (written with ATL 8)
Shared memory synchronization made through monitor synchronization pattern.
Tests for COM component written as Visual Studio Unit Test Project
Tests for Shared memory server/client written with Google test project

Currently there are several issues:
* Rule of 3/rule of 5 broken
* Better move to lock free algorithms because if client crashes whole process would hang till reset of server
* Server should use thread pool for incoming files with queues of file pieces for each thread