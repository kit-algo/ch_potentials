#!/usr/bin/env python3

import pandas as pd

import json
import glob
import os
import re

base = "exp/"
paths = glob.glob(base + "building_blocks/*.json")
data = [json.load(open(path)) for path in paths]

queries = pd.DataFrame.from_records([{
    'features': run['feature_flags'],
    **algo }
    for run in data for exp in run.get('experiments', []) for algo in exp['algo_runs'] if algo.get('algo') in ['CH Potentials Query', 'Dijkstra Query']])

queries['BCC'] = (queries['algo'] == 'CH Potentials Query') & ~queries['features'].str.contains('CHPOT_NO_BCC')
queries['Skip Deg. 2'] = (queries['algo'] == 'CH Potentials Query') & ~queries['features'].str.contains('CHPOT_NO_DEG2')
queries['Skip Deg. 3'] = (queries['algo'] == 'CH Potentials Query') & ~queries['features'].str.contains('CHPOT_NO_DEG3')

def heuristic_name(feature_flags):
    if 'CHPOT_ONLY_TOPO' in feature_flags:
        return 'Zero'
    elif 'CHPOT_ORACLE' in feature_flags:
        return 'Oracle'
    elif 'CHPOT_ALT' in feature_flags:
        return 'ALT'
    else:
        return 'CH'

queries['Heuristic'] = queries['features'].apply(heuristic_name)

queries_subset = queries.loc[lambda x: ((x['algo'] == 'Dijkstra Query') & (x['Heuristic'] == 'Zero')) | ((x['algo'] != 'Dijkstra Query') & ~((x['Heuristic'] == 'Zero') & ~x['BCC'] & ~x['Skip Deg. 2'] & ~x['Skip Deg. 3']))]

table = queries_subset.groupby(['Heuristic', 'BCC', 'Skip Deg. 2', 'Skip Deg. 3'])[['running_time_ms', 'num_queue_pushs']].mean()
table['num_queue_pushs'] = table['num_queue_pushs'] / 1000
table = table.round(1)
table = table.unstack(0).stack(0) \
    .swaplevel(2, 3).swaplevel(1,2).swaplevel(0,1) \
    .loc[['running_time_ms', 'num_queue_pushs']][['Zero', 'ALT', 'CH', 'Oracle']].reset_index(level=[2,3]) \
    .rename(index={
        'running_time_ms': R'\multirow{4}{*}{\rotatebox[origin=c]{90}{\shortstack{Running\\time [ms]}}}',
        'num_queue_pushs': R'\multirow{4}{*}{\rotatebox[origin=c]{90}{\shortstack{Queue\\pushs [$\cdot 10^3$]}}}',
    })

lines = table.to_latex(escape=False).split("\n")

lines = [R'\begin{tabular}{clllrrrr}'] + [lines[1]] + [
    R"       & BCC & Deg2 & Deg3 & Zero & ALT & CH-Pot. & Oracle \\"
] + lines[4:]

output = "\n".join(lines) + "\n"
output = output.replace('True', '\\cmark')
output = output.replace('False', '\\xmark')
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/building_blocks.tex", 'w') as f:
  f.write(output)
