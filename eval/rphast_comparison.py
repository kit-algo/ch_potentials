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
paths = glob.glob(base + "rphast/*.json")
data = [json.load(open(path)) for path in paths]

queries = pd.DataFrame.from_records([{
    **algo,
    'experiment': exp['experiment'],
    'target_set_size_exp': exp['target_set_size_exp'],
    'ball_size_exp': exp['ball_size_exp'],
    'graph': { 'europe': 'DIMACs Europe', 'osm_ger': 'OSM Germany', 'osm_europe': 'OSM Europe' }[[x for x in run['args'][1].split('/') if x != ''][-1]],
    }
    for run in data for exp in run['experiments'] for algo in exp['algo_runs']])

queries['running_time_s'] = queries['running_time_ms'] / 1000.0

plt.figure(figsize=(11,4))
g = sns.boxplot(data=queries.loc[lambda x: x['algo'] == 'lazy_rphast_many_to_one'].loc[lambda x: x['target_set_size_exp'] == 14], x='ball_size_exp', y='running_time_ms', hue='graph', showmeans=True, linewidth=0.8, flierprops=dict(marker='o', markerfacecolor='none', markeredgewidth=0.3))
g.set_xlabel('Ball Size $|B|$')
g.set_ylabel('Running Time [ms]')
g.set_yscale('log')
g.legend().set_title('')
plt.tight_layout()
g.get_figure().savefig('paper/fig/lazy_rphast_many_to_one.pdf')


plt.figure(figsize=(11,4))
g = sns.boxplot(data=queries.loc[lambda x: x['algo'] == 'lazy_rphast_many_to_many'].loc[lambda x: x['target_set_size_exp'] == 14], x='ball_size_exp', y='running_time_s', hue='graph', showmeans=True, linewidth=0.8, flierprops=dict(marker='o', markerfacecolor='none', markeredgewidth=0.3))
g.set_xlabel('Ball Size $|B|$')
g.set_ylabel('Running Time [s]')
g.legend().set_title('')
plt.tight_layout()
g.get_figure().savefig('paper/fig/lazy_rphast_many_to_many.pdf')
