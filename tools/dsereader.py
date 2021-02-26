#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import math
import os
import time

import matplotlib.pyplot as plt
import pandas as pd
import psutil
import seaborn as sns

sns.set_theme()

methods = {"kiter": "red", "speriodic": "yellow", "periodic": "green", "sdf3": "black"}


def load_app_dse(
    logdir,
    appname,
    method,
    cols=["throughput", "cumulative duration", "storage distribution size"],
):

    filename = f"{logdir}/{appname}_dselog_{method}.csv"

    try:
        df = pd.read_csv(filename, usecols=cols)
        return df
    except pd.errors.EmptyDataError:
        return pd.DataFrame()
    except FileNotFoundError:
        return pd.DataFrame()


def extract_pareto(df):
    # sort by sd
    pareto = df[df["throughput"] > 0].sort_values("storage distribution size").copy()

    # update th to max possible (assume ordered by sd)
    pareto["throughput"] = pareto["throughput"].cummax()

    # Take the max th for each sd
    pareto = pareto.groupby("storage distribution size").max().reset_index()

    # Take the min sd for each th
    pareto = pareto.groupby("throughput").min().reset_index()

    return pareto


def plot_dse(df, dsename=None, dsecolor=None):

    if len(df) == 0:
        return 0

    x = df["cumulative duration"]
    y = df["throughput"]

    if len(y) == 0:
        return 0

    steplines = plt.step(
        x, y, where="post", color=dsecolor, linewidth=0.4, alpha=0.5, label=dsename
    )  # 'C0o'
    current_color = steplines[0].get_color()

    df["cummaxth"] = df["throughput"].cummax()
    dfgb = df.groupby("cummaxth")

    x2 = dfgb.min()["cumulative duration"]
    y2 = dfgb.max()["throughput"]

    pareto = extract_pareto(df)
    x2, y2 = pareto["cumulative duration"], pareto["throughput"]

    markerline, stemlines, baseline = plt.stem(
        x2, y2, current_color, use_line_collection=True, basefmt=" "
    )
    plt.setp(stemlines, "linewidth", 0.5)
    plt.setp(markerline, "color", current_color)
    plt.setp(markerline, "markersize", 1)

    return y2.max()


def plot_pareto(df, dsename=None, dsecolor=None):

    if len(df) == 0:
        return 0

    pareto = extract_pareto(df)

    # Add period
    pareto["period"] = 1 / pareto["throughput"]
    df["period"] = 1 / df["throughput"]

    x, y = pareto["storage distribution size"], pareto["period"]
    if len(y) == 0:
        return 0

    markersize = 3
    linewidth = 1

    scatterpoints = plt.scatter(
        df["storage distribution size"],
        df["period"],
        s=markersize,
        alpha=0.1,
        label=None,
    )
    steplines = plt.step(
        x, y, where="post", color=dsecolor, linewidth=linewidth, alpha=1, label=dsename
    )  # 'C0o'
    current_color = steplines[0].get_color()
    steppoints = plt.plot(x, y, "C2o", markersize=markersize, alpha=0.5, label=None)
    plt.setp(steppoints, "color", current_color)
    plt.setp(scatterpoints, "color", current_color)

    return y.max()


def plot_app_dse(logdir, appname):

    total_time = time.time()
    ymax = 0
    for method, color in methods.items():

        start_time = time.time()
        print("Load", appname, method)
        df = load_app_dse(logdir, appname, method)
        print("Loaded after", time.time() - start_time, "sec.")

        start_time = time.time()
        print("Plot", appname, method)
        ymax = max(ymax, plot_dse(df, method, color))
        print("Plotted after", time.time() - start_time, "sec.")

    plt.xlabel("Execution time (ms)")
    plt.ylabel("Throughput (Hz)")
    plt.title(f"{appname}")
    plt.legend(loc="upper left", ncol=2, borderaxespad=0.5)
    if ymax:
        plt.ylim(bottom=0, top=1.3 * ymax)

    print("Finished in", time.time() - total_time, "sec.")
    print("")


def plot_app_pareto(logdir, appname):

    total_time = time.time()
    ymax = 0

    for method, color in methods.items():

        start_time = time.time()
        print("Load", appname, method)
        df = load_app_dse(
            logdir, appname, method, cols=["throughput", "storage distribution size"]
        )
        print("Loaded after", time.time() - start_time, "sec.")

        start_time = time.time()
        print("Plot", appname, method)
        ymax = max(ymax, plot_pareto(df, method, color))
        print("Plotted after", time.time() - start_time, "sec.")

    plt.xlabel("Storage Distribution")
    plt.ylabel("Period (sec)")
    plt.title(f"{appname}")
    plt.legend(loc="upper left", ncol=2, borderaxespad=0.5)
    if ymax:
        plt.ylim(bottom=0, top=1.3 * ymax)

    print("Finished in", time.time() - total_time, "sec.")
    print("")


def plot_all(logdir, graphs, plotfunc=plot_app_dse, outputname=None):
    total = len(graphs)
    subx = max(1, int(math.sqrt(total)))
    suby = int(total / subx)
    assert subx >= 1
    while subx * suby < total:
        suby += 1

    print(f"Produce subplot of {subx} x {suby}")
    fig = plt.figure(figsize=(suby * 5, subx * 5))

    fig.subplots_adjust(hspace=0.4, wspace=0.4)
    for i, name in zip(range(1, len(graphs) + 1), graphs):
        _ = fig.add_subplot(subx, suby, i)
        plotfunc(logdir, name)

    print("Tight the layout...")
    fig.tight_layout()

    if outputname:
        print("Save the file:", outputname)
        plt.savefig(outputname)

    plt.clf()
    plt.cla()  # Clear axis


def plot_all_pareto(logdir, graphs, outputname=None):
    plot_all(logdir, graphs, plotfunc=plot_app_pareto, outputname=outputname)


def plot_all_dse(logdir, graphs, outputname=None):
    plot_all(logdir, graphs, plotfunc=plot_app_dse, outputname=outputname)


if __name__ == "__main__":
    import argparse
    import glob

    parser = argparse.ArgumentParser(description="Generate DSE Plots")
    parser.add_argument(
        "graphs", metavar="G", type=str, nargs="*", help="Names of graphs to process"
    )
    parser.add_argument(
        "--logdir", type=str, help="location of log dirs", required=True
    )
    parser.add_argument(
        "--opareto",
        type=str,
        help="location of the output dse plot file",
        required=False,
    )
    parser.add_argument(
        "--odse",
        type=str,
        help="location of the output pareto plot file",
        required=False,
    )
    args = parser.parse_args()

    logdir = args.logdir
    graphs = args.graphs

    if len(graphs) == 0:
        graphs = list(
            set(
                [x.split("/")[-1].split("_dselog")[0] for x in glob.glob(logdir + "/*")]
            )
        )
    print("Process graphs:", graphs)

    process = psutil.Process(os.getpid())
    startmem = process.memory_info().rss

    if args.odse:
        print("Generate DSE output")
        plot_all_dse(logdir=logdir, graphs=graphs, outputname=args.odse)

    if args.opareto:
        print("Generate pareto output")
        plot_all_pareto(logdir=logdir, graphs=graphs, outputname=args.opareto)

    endmem = process.memory_info().rss
    print(
        "Memory usage from",
        int(startmem / (2 ** 20)),
        " MB to",
        int(endmem / (2 ** 20)),
        "MB",
    )
