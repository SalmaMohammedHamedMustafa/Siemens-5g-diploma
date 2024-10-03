# generator
- generate Ethernet packets and dumps the packets into a txt file
- gives the option to wait for a client connection to send the file

# reciever
- recieves the packets file
- parses the raw data
- checks the CRC and dumps the results to a file

# How to build?
- go to the program folder/directory 
- open the command line
##  In linux:
```bash
mkdir build
cd build
cmake ..
make
```
## In windows (MinGW (GCC))
- if you are using another compiler the steps will be close you can search it 
```bash 
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```
# How to run?
## for the generator
```bash
./EthernetApp
```
## for the reciever
- wait for the generator to be waiting for connections
```bash 
./ClientApp
```