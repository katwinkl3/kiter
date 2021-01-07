import os.path
import sys
from xml.dom import minidom

# This script parses an XML file from SDF3 and writes a CSV file with the
# appropriate data to visualise its pareto plot
# It takes in 2 arguments:
# (1)XML file of original graph, (2) XML file of DSE results from SDF3,
# and (3) an optional specified subdirectory to write logs into
# It uses (1) to identify and subsequently ignore feedback loops when
# writing the DSE results to the output CSV file (specified by dirName and
# fileName variables)

# Read in XML files
graph = minidom.parse(sys.argv[1])
doc = minidom.parse(sys.argv[2])
if len(sys.argv) > 3:
    dirName = sys.argv[3] + "/"  # specify subdirectory for output log
else:
    dirName = "./data/"  # default subdirectory


# Find feedback loops
channels = graph.getElementsByTagName("channel")
feedbackChannels = []
for channel in channels:
    if channel.getAttribute("srcActor") == channel.getAttribute("dstActor"):
        feedbackChannels.append(channel.getAttribute("name"))

# Get file name from XML results file
nameExt = os.path.split(sys.argv[2])[1]  # return filename with extension
name = os.path.splitext(nameExt)[0]  # remove extension
fileName = dirName + name + ".csv"  # specify filename

with open(fileName, 'w') as f:
    # start with column headers
    f.write("storage distribution size,throughput,channel quantities\n")
    distributions = doc.getElementsByTagName("distributionsSet")
    for dist in distributions:
        sz = dist.getAttribute("sz")
        thr = dist.getAttribute("thr")
        sds = dist.getElementsByTagName("distribution")
        for sd in sds:
            channelQuants = []
            distSz = 0
            ch = sd.getElementsByTagName("ch")
            for c in ch:  # iterate through and store channel quantities
                # ignore feedback loops
                if c.getAttribute("name") not in feedbackChannels:
                    quantity = c.getAttribute("sz")
                    channelQuants.append(quantity)
                    distSz += int(quantity)
                    f.write("%s,%s,\"%s\"\n"
                            % (distSz, thr, ",".join(channelQuants)))
# no need to explicitly close file when using 'with' keyword

print("Pareto point log written to: " + fileName)
