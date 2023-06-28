import re
import matplotlib.pyplot as plt
import statistics

with open("./timeResults", "r") as f:
    content = f.read()

pattern = r"Thread number: (\d+)\nFilter Took (\d+\.\d+) seconds\nFilter Took (\d+\.\d+) seconds\nFilter Took (\d+\.\d+) seconds"
times = re.findall(pattern, content)

threads = []
results = []
for time in times:
    threads.append(time[0])
    average = round((float(time[1]) + float(time[2]) + float(time[3]))/3, 5)
    results.append(average)
    print(time)

index = results.index(min(results))
plt.xlabel('Threads')
plt.ylabel('Filter function time')
plt.plot(threads, results)
plt.plot(threads[index], results[index], 'ro')
plt.show()