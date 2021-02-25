#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd

newnames = {"Filename": "Graph", 
             "tasks": "#tasks", 
             "buffers": "#buffers", 
             "SPeriodic": "Strictly Periodic", 
             "1Periodic": "1-Periodic", 
             "KPeriodic": "K-Periodic",
             "sumnt" : 'Complexity'}

def print_latex (filename) :
    df = pd.read_csv(filename, sep=";").sort_values(by="Filename")
    df["Filename"] = df["Filename"].replace(".+//(.*).xml","\\1", regex=True)
    df = df.rename(columns=newnames)
    df = df.drop(columns=["sumqt", "sumphit","sumki"])

    colformat = '|'.join([''] + ['l'] * df.index.nlevels + ['r'] * df.shape[1] + [''])
    latex = df.to_latex(float_format="{:0.2E}".format, column_format=colformat, index=False)

    print(latex)




if __name__ == "__main__":
    import argparse
    import glob

    parser = argparse.ArgumentParser(description="Generate Latex table for throughput")
    parser.add_argument(
        "filename", metavar="F", type=str, help="CSV file to process"
    )

    args = parser.parse_args()
    print_latex (args.filename) 
