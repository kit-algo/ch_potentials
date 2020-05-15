#!/usr/bin/env python3

import pandas as pd

import json
import glob
import os
import re

base = os.environ['CHPOT_EXP_OUTPUT_DIR']
paths = glob.glob(base + "applications/*.json")
data = [json.load(open(path)) for path in paths]


queries = pd.DataFrame.from_records([{
    'exp': exp['experiment'],
    'graph': run['args'][1],
    **algo }
    for run in data for exp in run.get('experiments', []) for algo in exp['algo_runs'] if algo.get('algo') == 'CH Potentials Query'])

queries = queries.append(pd.DataFrame.from_records([{
    'exp': run['program'],
    'graph': run['args'][1],
    **algo }
    for run in data for algo in run.get('algo_runs', []) if algo.get('algo') in ['CH Potentials Query', 'CH Potentials TD Query']]))


dijkstra_queries = pd.DataFrame.from_records([{
    'exp': exp['experiment'],
    'graph': run['args'][1],
    **algo }
    for run in data for exp in run.get('experiments', []) for algo in exp['algo_runs'] if algo.get('algo') == 'Dijkstra Query'])

dijkstra_queries = dijkstra_queries.append(pd.DataFrame.from_records([{
    'exp': run['program'],
    'graph': run['args'][1],
    **algo }
    for run in data for algo in run.get('algo_runs', []) if algo.get('algo') in ['Dijkstra Query', 'TD Dijkstra Query']]))


runtime_pattern = re.compile(".*Needed (\\d+)musec\\..*")

def parse_contraction_output(path):
    stats = { 'preprocessing_running_time_s': 0.0 }

    with open(path, 'r') as f:
        for line in f:
            if not 'graph' in stats:
                stats['graph'] = line.strip()
            else:
                match = runtime_pattern.match(line)
                if match:
                    stats['preprocessing_running_time_s'] += int(match[1]) / 1000000

    return stats

preprocessing = pd.DataFrame.from_records([parse_contraction_output(path) for path in glob.glob(base + "preprocessing/*.out")])

queries['affected'] = queries['lower_bound'] != queries['result']
queries['increase'] = queries['result'] / queries['lower_bound']

table = queries.groupby(['graph', 'exp']).agg(
    running_time_ms=('running_time_ms', 'mean'),
    num_pot_evals=('num_pot_evals', 'mean'),
    num_relaxed_arcs=('num_relaxed_arcs', 'mean'),
    increase=('increase', 'mean'),
    affected=('affected', 'sum'),
    size=('affected', 'count'))
table['affected'] = table['affected'] * 100 / table['size']
table = table[table.columns[0:-1]]
table['num_relaxed_arcs'] = table['num_relaxed_arcs'] / 1000
table['num_pot_evals'] = table['num_pot_evals'] / 1000
table = table.join(preprocessing.groupby('graph').mean())
table = table.join(dijkstra_queries.groupby(['graph', 'exp']).agg(dijkstra_running_time_ms=('running_time_ms', 'mean')))
#table = table.round(1)

with open("paper/table/applications.tex", 'w') as f:
  f.write(table.to_latex())
