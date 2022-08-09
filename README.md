# Disk Driver used for testing purposes

This project is made for FAT32 partitions on a MBR type of boot sector.

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

Tests are example usage of driver,
- Test0 creates a file and returns it's address and cluster number
- Test1 calculates buffer size on a given device