#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import psutil
import math 
import sys
import time
import pandas as pd
import matplotlib.pyplot as plt




def plot_dse (df, dsename = None, dsecolor = None):
    
    x = df["cumulative duration"]
    y = df["throughput"]
    
    plt.ylim(0,1.3*y.max())
    steplines = plt.step(x,y, where = "post", color =  dsecolor, linewidth=0.4, alpha=0.5, label = dsename) #  'C0o'
    current_color = steplines[0].get_color()
    
    
    df["cummaxth"] = df["throughput"].cummax()
    dfgb = df.groupby("cummaxth")

    x2 = dfgb.min()["cumulative duration"]
    y2 = dfgb.max()["throughput"]
    
    markerline, stemlines, baseline = plt.stem(x2, y2,current_color, use_line_collection = True, basefmt =' ')
    plt.setp(stemlines, 'linewidth', 2)
    plt.setp(markerline, 'color', current_color)
    



def plot_app_dse (logdir, appname):

    filename1 = f"{logdir}/{appname}_dselog_kiter.csv"
    filename2 = f"{logdir}/{appname}_dselog_sdf3.csv"

    total_time = time.time()
    start_time = time.time()
    print("Load", filename2)    
    df2 = pd.read_csv(filename2, usecols = ["throughput", "cumulative duration"])
    print ("Done after", time.time() -  start_time,"sec.")        

    start_time = time.time()
    print("Plot", filename2)    
    plot_dse (df2, "sdf3", "black") 
    print ("Done after", time.time() -  start_time,"sec.")        
    
    
    start_time = time.time()
    print("Load", filename1)
    df1 = pd.read_csv(filename1, usecols = ["throughput", "cumulative duration"])
    print ("Done after", time.time() -  start_time,"sec.")

    
    start_time = time.time()
    print("Plot", filename1)
    plot_dse (df1, "kiter","red")
    print ("Done after", time.time() -  start_time,"sec.")


    plt.xlabel("Execution time")
    plt.ylabel("Throughput")
    plt.title (f"{appname} DSE")
    plt.legend(loc='upper left',
           ncol=2,  borderaxespad=0.5)

    print("Finished in",time.time() - total_time,"sec.")
    print("")


def plot_all_dse (logdir, graphs, outputname = None):
    total = len(graphs)
    subx = max(1,int(math.sqrt(total)))
    suby = int(total / subx)
    assert(subx >= 1)
    while subx * suby < total:
        suby += 1

    print (f"Produce subplot of {subx} x {suby}")     
    fig = plt.figure(figsize=(suby*5, subx*5))
    
    fig.subplots_adjust(hspace=0.4, wspace=0.4)
    for i,name in zip(range(1, len(graphs) + 1),graphs):
        ax = fig.add_subplot(subx, suby, i)
        plot_app_dse(logdir, name)
        
    print ("Tight the layout...")
    fig.tight_layout()    

    if outputname :
        print ("Save the file:", outputname)
        plt.savefig(outputname)    

        
if __name__ == "__main__":
    import glob
    import argparse

    parser = argparse.ArgumentParser(description='Generate DSE Plots')
    parser.add_argument('graphs', metavar='G', type=str, nargs='*', 
                        help='Names of graphs to process')
    parser.add_argument('--logdir', type=str, 
                        help='location of log dirs')
    parser.add_argument('--outputfile', type=str, 
                        help='location of the output plot file')
    args = parser.parse_args()

    

    logdir = args.logdir
    outputname = args.outputfile
    graphs = args.graphs

    if len(graphs) == 0 :
        graphs = list(set([ x.split("/")[-1].split("_dselog")[0] for x in glob.glob(logdir + "/*")]))
    print("Process graphs:", graphs)
    
    process = psutil.Process(os.getpid())
    startmem = process.memory_info().rss

    plot_all_dse (logdir = logdir, graphs = graphs, outputname = outputname)

    endmem = process.memory_info().rss
    print ("Memory usage from",int(startmem/ (2**20)), " MB to",int(endmem / (2**20)), "MB")

    
