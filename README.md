# Disk Driver used for testing purposes

This project is made for FAT32 partitions on a MBR type of boot sector.
Project aims at research and deeper understandings of how disk controller 
works and how we can improve it's performance

Driver interface provides structs for:
- MBR
- Partition Entry
- Boot Sector
- FS Information Sector
- FAT and Entry
- Directory Root and Entry

Also we can extract and edit information of particular sector, read/write to specific cluster
get diretory root entry based on short filename and a lot of helper functions as getters/setters
for almost everything we need.

Tests are example usage of driver:
- test0: creates a file and returns it's address and cluster number
- test1: calculates disk buffer size on a given device
- test2: compares the performance of system calls and flags used for writing files
- rev-time: calculates rpm and revolution time in nanoseconds
- extract-tracks: extracts the track size for each sector on hdd
- acces-time: multi-purpose test used for calculating the access time and angular position

## Building 

Setup: 
```sh
$ mkdir bin
$ cd bin
$ cmake ../hdd-driver
```

Build:
```sh
cmake --build .
```