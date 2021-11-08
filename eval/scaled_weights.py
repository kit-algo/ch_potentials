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
    **algo,
    'weight_factor': exp['factor'],
    'graph': { 'europe': 'DIMACS Europe', 'osm_ger': 'OSM Germany', 'osm_europe': 'OSM Europe' }[[x for x in run['args'][1].split('/') if x != ''][-1]],
    'num_queue_pops': algo['num_queue_pops'] + algo.get('core_search', {}).get('num_queue_pops', 0),
    'num_queue_pushs': algo['num_queue_pushs'] + algo.get('core_search', {}).get('num_queue_pushs', 0),
    'num_relaxed_arcs': algo['num_relaxed_arcs'] + algo.get('core_search', {}).get('num_relaxed_arcs', 0)
} for run in data for exp in run['experiments'] for algo in exp['algo_runs'] if algo.get('algo') == 'Virtual Topocore Component Query'])

plt.figure(figsize=(11,4))
g = sns.boxplot(data=queries, x='weight_factor', y='running_time_ms', hue='graph', hue_order=['OSM Germany', 'DIMACS Europe'], showmeans=True, linewidth=0.8, flierprops=dict(marker='o', markerfacecolor='none', markeredgewidth=0.3))
g.xaxis.set_major_locator(mpl.ticker.MultipleLocator(5))
# g.xaxis.set_major_formatter(mpl.ticker.ScalarFormatter())
g.set_yscale('log')
g.set_ylabel('Running Time [ms]')
g.set_xlabel('Weight Factor $\\alpha$')
g.legend(loc='upper left').set_title('')
plt.tight_layout()
g.get_figure().savefig('paper/fig/scaled_weights.pdf')
