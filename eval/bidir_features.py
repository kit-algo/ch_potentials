#!/usr/bin/env python3

import numpy as np
import pandas as pd
pd.options.display.float_format = '{:.2f}'.format

import json
import glob
import os
import re

paths = glob.glob("exp/bidir_features/*.json")
data = [json.load(open(path)) for path in paths]

queries = pd.DataFrame.from_records([{
    **algo,
    'graph': { 'europe': 'DIMACs Europe', 'osm_ger': 'OSM Germany', 'osm_europe': 'OSM Europe' }[[x for x in run['args'][1].split('/') if x != ''][-1]],
    'experiment': exp['experiment'],
    'factor': exp['factor'],
    'probability': exp.get('probability', 1),
    'speed': exp.get('speed', 0),
    'potential': run['potential'],
    'num_queue_pops': algo['num_queue_pops'] + algo.get('core_search', {}).get('num_queue_pops', 0),
    'num_queue_pushs': algo['num_queue_pushs'] + algo.get('core_search', {}).get('num_queue_pushs', 0),
    'num_relaxed_arcs': algo['num_relaxed_arcs'] + algo.get('core_search', {}).get('num_relaxed_arcs', 0),
    }
    for run in data for exp in run['experiments'] for algo in exp['algo_runs']])

queries['choose_direction_strategy'] = queries['choose_direction_strategy'].fillna('unidir')
queries['bidir_pot'] = queries['bidir_pot'].fillna('unidir')
queries['improved_pruning'] = queries['improved_pruning'].fillna('unidir')
queries['num_queue_pushs_k'] = queries['num_queue_pushs'] / 1000

queries = queries.loc[lambda x: x['graph'] == 'OSM Germany']

pruning = queries.loc[lambda x: x['choose_direction_strategy'] != 'min_key'] \
    .loc[lambda x: x['bidir_pot'] != 'unidir'] \
    .groupby(['potential', 'experiment', 'factor', 'probability', 'algo', 'bidir_pot', 'improved_pruning'])[['running_time_ms', 'num_queue_pushs_k']].mean().unstack(0) \
    .loc[('weight_scale', 1.05, 1.00)] \
    .rename(columns={
        'running_time_ms': 'Running time [ms]',
        'num_queue_pushs_k': 'Queue pushs [$\\cdot 10^3$]',
        'CH': 'CH-Pot.',
        'CCH': 'CCH-Pot.',
    }) \
    .rename(index={
        'Average': 'Avg.',
        'Symmetric': 'Sym.',
    })

lines = pruning.to_latex(escape=False).split("\n")

lines = [R'\begin{tabular}{lllrrrrrrrr}'] + [lines[1]] + [
    ' & '.join(["Low Deg.", "Bidir.", "Impr."] + lines[2].split(' & ')[3:]) + R" \cmidrule(lr){4-7} \cmidrule(lr){8-11}",
    ' & '.join(["Opt.",     "Pot.",   "Pruning"] + lines[3].split(' & ')[3:])
] + lines[5:]
# linespace
# center measures
# center or fill bidir pot
# pot order

output = "\n".join(lines) + "\n"
output = output.replace('Bidrectional Dijkstra Query', R'\multirow{4}{*}{\xmark}')
output = output.replace('Virtual Topocore Bidirectional Core Query', R'\multirow{4}{*}{\cmark}')
output = output.replace('True', '\\cmark')
output = output.replace('False', '\\xmark')
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

# table = queries_subset.groupby(['Heuristic', 'BCC', 'Skip Deg. 2', 'Skip Deg. 3'])[['running_time_ms', 'num_queue_pushs']].mean()
# table['num_queue_pushs'] = table['num_queue_pushs'] / 1000
# table = table.round(1)
# table = table.unstack(0).stack(0) \
#     .swaplevel(2, 3).swaplevel(1,2).swaplevel(0,1) \
#     .loc[['running_time_ms', 'num_queue_pushs']][['Zero', 'ALT', 'CH', 'Oracle']].reset_index(level=[2,3]) \
#     .rename(index={
#         'running_time_ms': R'\multirow{4}{*}{\rotatebox[origin=c]{90}{\shortstack{Running\\time [ms]}}}',
#         'num_queue_pushs': R'\multirow{4}{*}{\rotatebox[origin=c]{90}{\shortstack{Queue\\pushs [$\cdot 10^3$]}}}',
#     })

# lines = table.to_latex(escape=False).split("\n")

# lines = [R'\begin{tabular}{clllrrrr}'] + [lines[1]] + [
#     R"       & BCC & Deg2 & Deg3 & Zero & ALT & CH-Pot. & Oracle \\"
# ] + lines[4:]

# output = "\n".join(lines) + "\n"
# output = output.replace('True', '\\cmark')
# output = output.replace('False', '\\xmark')
# output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

# with open("paper/table/building_blocks.tex", 'w') as f:
#   f.write(output)
