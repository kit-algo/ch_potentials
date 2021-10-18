#!/usr/bin/env python3

import pandas as pd

import json
import glob
import os
import re

base = "exp/"
cwd = os.getcwd()
paths = glob.glob(base + "applications/*.json")
data = [json.load(open(path)) for path in paths]

def path_to_graph(path):
    return {
        'ger06': 'TDGer06',
        'ptv17': 'TDEur17',
        'ptv20': 'TDEur20',
        'osm_europe': 'OSM Europe',
        'osm_ger': 'OSM Ger',
        'osm_ger_td': 'OSM Ger',
        'europe': 'DIMACs Eur',
    }[[x for x in path.split('/') if x != ''][-1]]

only_public = 'ONLY_PUBLIC' in os.environ

queries = pd.DataFrame.from_records([{
    'exp': exp['experiment'],
    'graph': run['args'][1],
    **algo }
    for run in data for exp in run.get('experiments', []) for algo in exp['algo_runs'] if algo.get('algo') == 'CH Potentials Query'])

queries = queries.append(pd.DataFrame.from_records([{
    'exp': run['program'],
    'graph': run['args'][1],
    **algo }
    for run in data for algo in run.get('algo_runs', []) if algo.get('algo') == 'CH Potentials Query']))


dijkstra_queries = pd.DataFrame.from_records([{
    'exp': exp['experiment'],
    'graph': run['args'][1],
    **algo }
    for run in data for exp in run.get('experiments', []) for algo in exp['algo_runs'] if algo.get('algo') == 'Dijkstra Query'])

dijkstra_queries = dijkstra_queries.append(pd.DataFrame.from_records([{
    'exp': run['program'],
    'graph': run['args'][1],
    **algo }
    for run in data for algo in run.get('algo_runs', []) if algo.get('algo') == 'Dijkstra Query']))


queries['affected'] = queries['lower_bound'] != queries['result']
queries['increase'] = (queries['result'] / queries['lower_bound'] - 1) * 100

table = queries.groupby(['graph', 'exp']).agg(
    running_time_ms=('running_time_ms', 'mean'),
    num_queue_pushs=('num_queue_pushs', 'mean'),
    increase=('increase', 'mean'),
    # affected=('affected', 'sum'),
    # size=('affected', 'count')
    )

table = table.reindex(['data/europe/',
    cwd + '/data/osm_europe/',
    cwd + '/data/osm_ger/',
    cwd + '/data/osm_ger_td/',
    cwd + '/data/ptv17',
    cwd + '/data/ger06'], level=0) \
    .reindex(['chpot_td',
    'random_times_2',
    'random_times_10',
    'fastest_times_2',
    'fastest_times_10',
    'perfect',
    'chpot_turns',
    'no_tunnels',
    'no_highways',
    'chpot_live',
    'chpot_td_live',
    'chpot_turns_td_live',
    ], level=1)

# table['affected'] = table['affected'] * 100 / table['size']
# table = table[table.columns[0:-1]]
table['num_queue_pushs'] = table['num_queue_pushs'] / 1000
table = table.join(dijkstra_queries.groupby(['graph', 'exp']).agg(dijkstra_running_time_ms=('running_time_ms', 'mean')))
table['speedup'] = table['dijkstra_running_time_ms'] / table['running_time_ms']
table = table.round(1)

if only_public:
    ger_name = R'\multirow{4}{*}{OSM Ger}'
else:
    ger_name = R'\multirow{8}{*}{OSM Ger}'


table = table.rename(index={
    cwd + '/data/ger06': 'TDGer06',
    cwd + '/data/ptv17': 'TDEur17',
    cwd + '/data/osm_europe/': 'OSM Europe',
    cwd + '/data/osm_ger/': ger_name,
    cwd + '/data/osm_ger_td/': ger_name,
    cwd + '/data/europe/': 'DIMACs Europe',
    'perfect': 'Unmodified ($w_q=w_\\ell$)',
    'no_highways': 'No Highways',
    'no_tunnels': 'No Tunnels',
    'chpot_turns': 'Turns',
    'chpot_td': 'TD',
    'chpot_live': 'Live',
    'chpot_td_live': 'TD + Live',
    'chpot_turns_td_live': 'TD + Live + Turns',
    'fastest_times_10': 'Fastest $\\times 10$',
    'fastest_times_2': 'Fastest $\\times 2$',
    'random_times_10': 'Random $\\times 10$',
    'random_times_2': 'Random $\\times 2$',
})

orig_lines = table.to_latex(escape=False).split("\n")

lines = orig_lines[:2] + [
  R" & &   Running &                Queue &     Length & Dijkstra & Speedup \\"
  R" & & time [ms] & $[\cdot 10^3]$ & incr. [\%] &     [ms] &         \\"
]

if only_public:
    lines += orig_lines[4:]
else:
    lines += orig_lines[4:13] + ["\\addlinespace"] + orig_lines[13:]

output = "\n".join(lines) + "\n"
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/applications.tex", 'w') as f:
  f.write(output)
