import csv
import matplotlib.pyplot as plt

trials = []
overlaps = []

with open("results.csv", "r") as f:
    reader = csv.DictReader(f)
    for row in reader:
        trials.append(int(row["trial"]))
        overlaps.append(int(row["overlap"]))

plt.figure()
plt.plot(trials, overlaps)
plt.xlabel("Trial")
plt.ylabel("Overlap")
plt.title("Cross-Cache Overlap per Trial")
plt.grid()

plt.savefig("overlap_plot.png")
print("Saved plot to overlap_plot.png")
