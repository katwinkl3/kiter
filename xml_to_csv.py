import sys
from xml.dom import minidom

# This script parses an XML file from SDF3 and writes a CSV file with the
# appropriate data to visualise its pareto plot
# It takes in 2 arguments: (1) XML file, (2) Output CSV file
doc = minidom.parse(sys.argv[1])

with open(sys.argv[2], 'w') as f:
    f.write("storage distribution size, throughput, channel quantities\n")  # column headers
    distributions = doc.getElementsByTagName("distributionsSet")
    for dist in distributions:
        sz = dist.getAttribute("sz")
        thr = dist.getAttribute("thr")
        sds = dist.getElementsByTagName("distribution")
        channelQuants = []
        for sd in sds:
            ch = sd.getElementsByTagName("ch")
            for c in ch:  # iterate through and store channel quantities
                quantity = c.getAttribute("sz")
                channelQuants.append(quantity)
        f.write("%s,%s,\"%s\"\n" % (sz, thr, ",".join(channelQuants)))
        # no need to explicitly close file when using 'with' keyword
