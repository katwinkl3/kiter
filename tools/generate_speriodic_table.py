#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd
import numpy as np


def open_csv(filename) :
    
    df = pd.read_csv(filename, sep=";").sort_values(by="Filename")
    df = df.fillna(-1)
    df[ ["sumki", "SPeriodic" , "1Periodic" , "KPeriodic"]] = df[ ["sumki", "SPeriodic" , "1Periodic" , "KPeriodic"]].where(df[ ["sumki", "SPeriodic" , "1Periodic" , "KPeriodic"]] >= 0, np.nan)
    df["Filename"] = df["Filename"].replace(".+//(.*).xml","\\1", regex=True)
    return df


def print_throughput_table (df) :

    newnames = {"Filename": "Graph", 
                "tasks": "#tasks", 
                "buffers": "#buffers", 
                "SPeriodic": "Strictly Periodic", 
                "1Periodic": "1-Periodic", 
                "KPeriodic": "K-Periodic",
                "sumnt" : 'Complexity'}

    rdf = df.rename(columns=newnames)
    rdf = rdf.drop(columns=["sumqt", "sumphit", "sumki"])
    colformat = '|'.join([''] + ['l'] * rdf.index.nlevels + ['r'] * rdf.shape[1] + [''])
    latex = rdf.to_latex(float_format="{:0.2E}".format, column_format=colformat, index=False)
    #.replace('\\\\\n', '\\\\ \\midrule\n').replace("  ", " ").replace("  ", " ").replace("  ", " ")
    print(latex)

def print_schedule_size_table (df) :
    
    newnames = {"Filename": "Graph", 
                "tasks": "#tasks", 
                "buffers": "#buffers", 
                "tasks": "Strictly Periodic", 
                "sumphit": "1-Periodic", 
                "sumki": "K-Periodic",
                "sumnt" : 'MCM'}

    #rdf = df.drop(columns=["buffers","sumqt","1Periodic", "KPeriodic", "SPeriodic"])
    rdf = df.rename(columns=newnames)
    rdf = rdf [["Graph", "Strictly Periodic" , "1-Periodic", "K-Periodic", 'MCM' ]]
    colformat = '|'.join([''] + ['l'] * rdf.index.nlevels + ['r'] * rdf.shape[1] + [''])
    latex = rdf.to_latex(float_format="{:0.0f}".format, column_format=colformat, index=False)
    #.replace('\\\\\n', '\\\\ \\midrule\n').replace("  ", " ").replace("  ", " ").replace("  ", " ")
    print(latex)





if __name__ == "__main__":
    import argparse
    import glob

    parser = argparse.ArgumentParser(description="Generate Latex table for throughput")
    parser.add_argument(
        "filename", metavar="F", type=str, help="CSV file to process"
    )

    args = parser.parse_args()
    df = open_csv(args.filename)
    print_schedule_size_table (df)
    print_throughput_table (df) 

