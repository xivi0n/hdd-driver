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
- closest-sector: gets the least access time sector from the given reference

Analysis contains scripts for evaluating the results of tests:
- generate_test_fat: generates mock File Allocation Table
- graph-and: displays access time on graph by sector number
- graph-tracks: displays track size by sector number
- generate-report: based on extract-tracks output generates the report, example:

```json
{
    "file_size": 160041885696,
    "sector_size": 512,
    "rev_time": 8333395,
    "rpm": 7199.947,
    "max_lba": 312581808,
    "tracks": {
        "1440": {
            "zones": [
                {
                    "start": 0.0,               // start % of disk
                    "end": 3.4666336052416717,  // end % of disk
                    "start_lba": 0,             // LBA start address
                    "end_lba": 10836066         // LBA end address
                },
                {
                    "start": 3.4684644859434686,
                    "end": 15.918282742801207,
                    "start_lba": 10841789,
                    "end_lba": 49757656
                }
            ],
            "bandwidth": 84.37437562962035
        },
    ...
    }
}
```

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