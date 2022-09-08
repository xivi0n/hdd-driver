offset_1 = 2500
offset_2 = 3100
cluster_size = 12
taken = bytes([0xff, 0xff, 0xff, 0x0f])
free = bytes([0x00, 0x00, 0x00, 0x00])
with open("fat.out", "wb") as f:
    for i in range(offset_1//cluster_size):
        f.write(taken)
    for i in range(40):
        f.write(free)
    for i in range((offset_2 - offset_1)//cluster_size):
        f.write(taken)
