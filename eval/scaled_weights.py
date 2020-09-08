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

base = "exp/"
paths = glob.glob(base + "scaled_weights/*.json")
data = [json.load(open(path)) for path in paths]

queries = pd.DataFrame.from_records([{
    'weight_factor': exp['factor'],
    'graph': { 'data/europe/': 'DIMACs Europe', 'data/osm_ger/': 'OSM Germany', 'data/osm_europe/': 'OSM Europe' }[run['args'][1]],
    **algo }
    for run in data for exp in run['experiments'] for algo in exp['algo_runs'] if algo.get('algo') == 'CH Potentials Query'])

plt.figure(figsize=(6,3))
g = sns.boxplot(data=queries, x='weight_factor', y='running_time_ms', hue='graph', linewidth=0.8, showmeans=True)
g.xaxis.set_major_locator(mpl.ticker.MultipleLocator(5))
# g.xaxis.set_major_formatter(mpl.ticker.ScalarFormatter())
g.set_yscale('log')
g.set_ylabel('Running Time [ms]')
g.set_xlabel('Weight Factor $\\alpha$')
g.legend().remove()
plt.tight_layout()
g.get_figure().savefig('paper/fig/scaled_weights.pdf')
