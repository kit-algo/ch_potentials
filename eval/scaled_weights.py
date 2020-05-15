#!/usr/bin/env python3

import numpy as np
import matplotlib as mpl
from matplotlib import pyplot as plt
import seaborn as sns
sns.set()

import pandas as pd

import json
import glob
import os

base = os.environ['CHPOT_EXP_OUTPUT_DIR']
paths = glob.glob(base + "scaled_weights/*.json")
data = [json.load(open(path)) for path in paths]

queries = pd.DataFrame.from_records([{
    'weight_factor': exp['factor'],
    'graph': run['args'][1],
    **algo }
    for run in data for exp in run['experiments'] for algo in exp['algo_runs'] if algo.get('algo') == 'CH Potentials Query'])

plt.figure(figsize=(11,4))
g = sns.boxplot(data=queries, x='weight_factor', y='running_time_ms', hue='graph', linewidth=0.8)
g.set_ylabel('Running Time [ms]')
g.set_xlabel('Weight Factor')
plt.tight_layout()
g.get_figure().savefig('paper/fig/scaled_weights.pdf')
