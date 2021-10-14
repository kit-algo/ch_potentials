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
paths = glob.glob(base + "rphast_inc/*.json")
data = [json.load(open(path)) for path in paths]

queries = pd.DataFrame.from_records([{
    **progress,
    'algo': algo['algo'],
    'experiment': exp['experiment'],
    'ball_size_exp': exp['ball_size_exp'],
    'graph': { 'europe': 'DIMACs Europe', 'osm_ger': 'OSM Germany', 'osm_europe': 'OSM Europe' }[[x for x in run['args'][1].split('/') if x != ''][-1]],
    }
    for run in data for exp in run['experiments'] for algo in exp['algo_runs'] for progress in algo['progress_states']])

queries['single_query_time_mus'] = queries['passed_time_mus'] / (2 ** queries['terminal_rank'])
queries['terminal_idx'] = (2 ** queries['terminal_rank'])
queries['passed_time_ms'] = queries['passed_time_mus'] / 1000.0
queries['Algorithm'] = queries['algo'].map({ 'lazy_rphast_many_to_one': 'CH', 'lazy_rphast_cch_many_to_one': 'CCH' })
queries['Ball Size $|B|$'] = queries['ball_size_exp'].map('$2^{{{}}}$'.format)

fig, (ax1, ax2) = plt.subplots(2, sharex=True, figsize=(11,8))
g = sns.lineplot(data=queries.query('graph == "OSM Germany" & ball_size_exp in [14,17,21,24]'), x='terminal_idx', y='passed_time_ms', hue='Ball Size $|B|$', style='Algorithm', ci=None, marker='X', ax=ax1)
g.set_xlabel('Number of Computed Distances')
g.set_ylabel('Running Time [ms]')
g = sns.lineplot(data=queries.query('graph == "OSM Germany" & ball_size_exp in [14,17,21,24]'), x='terminal_idx', y='single_query_time_mus', hue='Ball Size $|B|$', style='Algorithm', ci=None, marker='X', ax=ax2, legend=False)
g.set_yscale('log')
g.set_xlabel('Number of Computed Distances')
g.set_ylabel('Average Running Time per Distance [$\\mu$s]')
g.yaxis.set_major_formatter(mpl.ticker.FuncFormatter(lambda val, pos: f"{val if val < 1.0 else int(val)}"))
fig.tight_layout()
fig.savefig('paper/fig/lazy_rphast_inc.pdf')
