import json
import re

def get_bandwidth(params, size):
    return (params["sector_size"] * size * 1e9) / params["rev_time"] / 1024 / 1024

params = {}
with open("tracks-fast.txt", "r") as f:
    for i, line in enumerate(f):
        r = re.match("([a-zA-Z_]+)[\t]([0-9]+[.]+[0-9]+|[0-9]+)[\n]", line)
        if r is None:
            break
        if "." in r.groups()[1]:
            params[r.groups()[0]] = float(r.groups()[1])
        else:
            params[r.groups()[0]] = int(r.groups()[1])

print(params)
        
x = []
y = []
with open("test.txt", "r") as f:
    for line in f:
        r = re.findall("[0-9]+[\t][0-9]+[\n]", line)
        if (len(r) > 0):
            r = [int(x) for x in r[0].split("\t")]
            x.append(r[0])
            y.append(r[1])

perc = 0.05
max_lba = params["file_size"]//params["sector_size"]
params["max_lba"] = max_lba
params["tracks"] = {}
if len(x) > 0:
    start = 0
    prev = y[0]
    for i in range(len(x)):
        if not (1-perc)*prev <= y[i] <= (1+perc)*prev or i == len(x) - 1:
            if prev in params["tracks"]:
                params["tracks"][prev]["zones"].append({
                    "start": 100 * start / max_lba,
                    "end": 100 if i == len(x) - 1 else 100 * x[i] / max_lba,
                    "start_lba": start,
                    "end_lba": max_lba if i == len(x) - 1 else x[i],
                })
            else:
                params["tracks"][prev] = {}
                params["tracks"][prev]["zones"] = [{
                    "start": 100 * start / max_lba,
                    "end": 100 if i == len(x) - 1 else 100 * x[i] / max_lba,
                    "start_lba": start,
                    "end_lba": max_lba if i == len(x) - 1 else x[i],
                }]
                params["tracks"][prev]["bandwidth"] = get_bandwidth(params, prev)
            start = x[i]
            prev = y[i]
        

with open("report.json", "w") as f:
    json.dump(params, f, indent=4)
