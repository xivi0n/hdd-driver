import matplotlib.pyplot as plt

x = []
y = []
x2 = []
y2 = []
with open("atime.txt", "r") as f:
    for line in f:
        line = line.split("\t")
        x.append(int(line[0]))
        y.append(int(line[1]))
        

with open("atime2.txt", "r") as f:
    for line in f:
        line = line.split("\t")
        x2.append(int(line[0]))
        y2.append(int(line[1]))

n = len(x)
plt.plot(x[:n],y[:n], '.')
plt.plot(x2[:n],y2[:n], '.')
plt.show()