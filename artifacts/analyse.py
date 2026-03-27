import csv
import statistics

# input the results file
FILE = "results.csv"

overlaps = []

# parse results 
with open(FILE, "r") as f:
    reader = csv.DictReader(f)
    for row in reader:
        overlaps.append(int(row["overlap"]))

if not overlaps:
    print("No data found.")
    exit()
# calculate mean, variance and success rate
mean = statistics.mean(overlaps)
variance = statistics.variance(overlaps) if len(overlaps) > 1 else 0
success_rate = sum(1 for x in overlaps if x > 0) / len(overlaps)

# outputs
print("=== PCP-LOST PoC Analysis ===")
print(f"Trials: {len(overlaps)}")
print(f"Mean overlap: {mean:.2f}")
print(f"Variance: {variance:.2f}")
print(f"Success rate (>0 overlap): {success_rate*100:.2f}%")
