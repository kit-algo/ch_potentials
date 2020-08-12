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

table = table.reindex(['/algoDaten/zeitz/roadgraphs/europe/',
    '/algoDaten/zeitz/roadgraphs/osm_europe/',
    '/algoDaten/zeitz/roadgraphs/osm_ger/',
    '/algoDaten/zeitz/roadgraphs/osm_ger_td/',
    '/algoDaten/graphs/cleaned_td_road_data/ptv17-eur-car/day/di/',
    '/algoDaten/graphs/cleaned_td_road_data/de/day/dido/'], level=0) \
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

table = table.rename(index={
    '/algoDaten/graphs/cleaned_td_road_data/de/day/dido/': 'TDGer06',
    '/algoDaten/graphs/cleaned_td_road_data/ptv17-eur-car/day/di/': 'TDEur17',
    '/algoDaten/zeitz/roadgraphs/osm_europe/': 'OSM Europe',
    '/algoDaten/zeitz/roadgraphs/osm_ger/': 'OSM Ger',
    '/algoDaten/zeitz/roadgraphs/osm_ger_td/': 'OSM Ger',
    '/algoDaten/zeitz/roadgraphs/europe/': 'DIMACs Europe',
    'perfect': 'Unmodified',
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

lines = table.to_latex(escape=False).split("\n")

lines = lines[:2] + [
  R" & &   Running &                Queue &     Length & Dijkstra & Speedup \\"
  R" & & time [ms] & pushs $[\cdot 10^3]$ & incr. [\%] &     [ms] &         \\"
] + lines[4:13] + ["\\addlinespace"] + lines[13:]

output = "\n".join(lines) + "\n"
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/applications.tex", 'w') as f:
  f.write(output)
