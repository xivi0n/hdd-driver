import re
import matplotlib.pyplot as plt

x = []
y = []
with open("tracks-slow.txt", "r") as f:
    for line in f:
        r = re.findall("[0-9]+[\t][0-9]+[\n]", line)
        # print(r)
        if (len(r) > 0):
            r = [int(x) for x in r[0].split("\t")]
            x.append(r[0])
            y.append(r[1])

plt.plot(x,y)
plt.ylabel("Track size")
plt.xlabel("Sector number")
plt.show()