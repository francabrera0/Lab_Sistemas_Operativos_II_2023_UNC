import re
import matplotlib.pyplot as plt
import statistics

with open("./tests/log/timeResults", "r") as f:
    content = f.read()

pattern = r"\d+\,\d+ seconds time elapsed"
times = re.findall(pattern, content)

times = [time.replace("seconds time elapsed", "") for time in times]
times = [time.replace(",", ".") for time in times]
times = [float(time) for time in times]


min = min(times)
max = max(times)
mean = statistics.median(times)

plt.scatter(range(len(times)), times)
plt.xlabel("Test")
plt.ylabel("Time [s]")
plt.axhline(min, color='red')
plt.axhline(max, color='red')
plt.axhline(mean, color='green')

print("Min: ", min)
print("Max: ", max)
print("Median: ", mean)

plt.show()