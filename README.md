# SharedMemoryServerClient

* Simple server for sending files from client to server through shared memory.
* Name is gotten from COM component (written with ATL 8)
* Shared memory synchronization made through monitor synchronization pattern. 
* Tests for COM component written as Visual Studio Unit Test Project
* Tests for Shared memory server/client written with Google test project

# Currently there are several TODO:

* Better move to lock free algorithms
* Server should use thread pool for incoming files with queues of file pieces for each thread