#!/usr/bin/env python3

import pandas as pd

import json
import glob
import os
import re

base = "exp/"
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

algos = ['Virtual Topocore Component Query', 'Virtual Topocore Bidirectional Core Query']
queries = pd.DataFrame.from_records([{
    **algo,
    'exp': exp['experiment'],
    'graph': path_to_graph(run['args'][1]),
    'num_queue_pops': algo.get('num_queue_pops', 0) + algo.get('core_search', {}).get('num_queue_pops', 0),
    'num_queue_pushs': algo.get('num_queue_pushs', 0) + algo.get('core_search', {}).get('num_queue_pushs', 0),
    'num_relaxed_arcs': algo.get('num_relaxed_arcs', 0) + algo.get('core_search', {}).get('num_relaxed_arcs', 0)
    } for run in data for exp in run.get('experiments', []) for algo in exp['algo_runs'] if (algo.get('algo') in algos or ('algo' not in algo and 'biggest_bcc_size' not in algo))])

queries = queries.append(pd.DataFrame.from_records([{
    **algo,
    'exp': run['program'],
    'graph': path_to_graph(run['args'][1]),
    'num_queue_pops': algo.get('num_queue_pops', 0) + algo.get('core_search', {}).get('num_queue_pops', 0),
    'num_queue_pushs': algo.get('num_queue_pushs', 0) + algo.get('core_search', {}).get('num_queue_pushs', 0),
    'num_relaxed_arcs': algo.get('num_relaxed_arcs', 0) + algo.get('core_search', {}).get('num_relaxed_arcs', 0)
    } for run in data for algo in run.get('algo_runs', []) if algo.get('algo') in algos or ('algo' not in algo and 'biggest_bcc_size' not in algo)]))

algos = ['Dijkstra Query', 'Bidrectional Dijkstra Query']
dijkstra_queries = pd.DataFrame.from_records([{
    'exp': exp['experiment'],
    'graph': path_to_graph(run['args'][1]),
    **algo }
    for run in data for exp in run.get('experiments', []) for algo in exp['algo_runs'] if algo.get('algo') in algos])

dijkstra_queries = dijkstra_queries.append(pd.DataFrame.from_records([{
    'exp': run['program'],
    'graph': path_to_graph(run['args'][1]),
    **algo }
    for run in data for algo in run.get('algo_runs', []) if algo.get('algo') in algos]))

queries['affected'] = queries['lower_bound'] != queries['result']
queries['increase'] = (queries['result'] / queries['lower_bound'] - 1) * 100

table = queries.groupby(['graph', 'exp']).mean()[['running_time_ms', 'num_queue_pushs', 'increase']]
table['num_queue_pushs'] = table['num_queue_pushs'] / 1000
table = table.join(dijkstra_queries.groupby(['graph', 'exp']).agg(dijkstra_running_time_ms=('running_time_ms', 'mean')))
table = table.reset_index()
table['algo'] = table['exp'].map({
    'perfect': 'CH U',
    'chpot_td': 'CH U',
    'chpot_live': 'CH U',
    'chpot_turns': 'CH U',
    'chpot_td_live': 'CH U',
    'chpot_turns_td_live': 'CH U',
    'no_highways': 'CH U',
    'no_tunnels': 'CH U',
    'cchpot_live': 'CCH U',
    'cchpot_turns': 'CCH U',
    'bidir_cchpot_turns': 'CCH B',
    'bidir_chpot_live': 'CH B',
    'bidir_chpot_turns': 'CH B',
    'bidir_no_highways': 'CH B',
    'bidir_no_tunnels': 'CH B',
})
table['app'] = table['exp'].map({
    'perfect': 'Unmodified $w_q = w_{\\ell}$',
    'chpot_td': 'TD',
    'chpot_live': 'Live',
    'chpot_turns': 'Turns',
    'chpot_td_live': 'TD + Live',
    'chpot_turns_td_live': 'TD + Live + Turns',
    'no_highways': 'No Highways',
    'no_tunnels': 'No Tunnels',
    'cchpot_live': 'Live',
    'cchpot_turns': 'Live + Turns',
    'bidir_cchpot_turns': 'Live + Turns',
    'bidir_chpot_live': 'Live',
    'bidir_chpot_turns': 'Turns',
    'bidir_no_highways': 'No Highways',
    'bidir_no_tunnels': 'No Tunnels',
})
table = table.set_index(['graph', 'app', 'algo']) \
    .reindex(index=['Unmodified $w_q = w_{\\ell}$', 'No Tunnels', 'No Highways', 'Live', 'Turns', 'Live + Turns', 'TD', 'TD + Live', 'TD + Live + Turns'], level=1) \
    .reindex(index=['CH U', 'CH B', 'CCH U', 'CCH B'], level=2) \
    .drop(columns=['exp'])
table.at[('OSM Ger', 'No Tunnels', 'CH B'), 'dijkstra_running_time_ms'] = table.at[('OSM Ger', 'No Tunnels', 'CH U'), 'dijkstra_running_time_ms']
table.at[('OSM Ger', 'No Highways', 'CH B'), 'dijkstra_running_time_ms'] = table.at[('OSM Ger', 'No Highways', 'CH U'), 'dijkstra_running_time_ms']
if 'ONLY_PUBLIC' not in os.environ:
    table.at[('OSM Ger', 'Live', 'CH B'), 'dijkstra_running_time_ms'] = table.at[('OSM Ger', 'Live', 'CH U'), 'dijkstra_running_time_ms']
    table.at[('OSM Ger', 'Live', 'CCH U'), 'dijkstra_running_time_ms'] = table.at[('OSM Ger', 'Live', 'CH U'), 'dijkstra_running_time_ms']
table.at[('OSM Ger', 'Turns', 'CH B'), 'dijkstra_running_time_ms'] = table.at[('OSM Ger', 'Turns', 'CH U'), 'dijkstra_running_time_ms']
if 'ONLY_PUBLIC' not in os.environ:
    table.at[('OSM Ger', 'Live + Turns', 'CCH B'), 'dijkstra_running_time_ms'] = table.at[('OSM Ger', 'Live + Turns', 'CCH U'), 'dijkstra_running_time_ms']
table['speedup'] = table['dijkstra_running_time_ms'] / table['running_time_ms']
table = table.round(1)

lines = table.to_latex(escape=False).split('\n')

lines = [
    R"\begin{tabular}{llrrrrrr}",
    R"\toprule"
    R" & & &   Running &                Queue &     Length & Dijkstra & Speedup \\ & & & time [ms] & $[\cdot 10^3]$ & incr. [\%] &     [ms] &         \\"
] + lines[4:]

if 'ONLY_PUBLIC' not in os.environ:
    lines[3] += R" \addlinespace"
    lines[18] += R" \addlinespace"
    lines[4] += "[2pt]"
    lines[6] += "[2pt]"
    lines[8] += "[2pt]"
    lines[11] += "[2pt]"
    lines[13] += "[2pt]"
    lines[15] += "[2pt]"

output = "\n".join(lines) + "\n"
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/applications.tex", 'w') as f:
  f.write(output)
