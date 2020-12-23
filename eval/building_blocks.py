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

queries['BCC'] = ~queries['features'].str.contains('CHPOT_NO_BCC')
queries['Skip Deg. 2'] = ~queries['features'].str.contains('CHPOT_NO_DEG2')
queries['Skip Deg. 3'] = ~queries['features'].str.contains('CHPOT_NO_DEG3')

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

table = queries.groupby(['algo', 'Heuristic', 'BCC', 'Skip Deg. 2', 'Skip Deg. 3'])[['running_time_ms', 'num_queue_pushs']].mean()
table['num_queue_pushs'] = table['num_queue_pushs'] / 1000
table = table.round(1)

table = table.reindex(['Zero', 'ALT', 'CH', 'Oracle'], level=1)
table = table.rename(index={
    'Zero': R'\multirow{4}{*}{\rotatebox[origin=c]{90}{Zero}}',
    'ALT': R'\multirow{4}{*}{\rotatebox[origin=c]{90}{ALT}}',
    'CH': R'\multirow{4}{*}{\rotatebox[origin=c]{90}{\shortstack{CH-\\Potentials}}}',
    'Oracle': R'\multirow{2}{*}{Oracle}',
})

table = table.reset_index(level=0,drop=True).reset_index(level=[2,3])

lines = table.to_latex(escape=False).split("\n")

lines = [R'\begin{tabular}{clllrr}'] + [lines[1]] + [
    R" & & & & Running &     Queue \\",
    R" Heur. & BCC & Deg2 & Deg3 & time [ms] & [$\cdot 10^3$] \\"
# ] + lines[4:]
] + [lines[4], lines[21]] + lines[6:9] + ["\\addlinespace"] + lines[9:13] + ["\\addlinespace"] + lines[13:17] + ["\\addlinespace"] + [lines[17], lines[20]] + lines[24:]

output = "\n".join(lines) + "\n"
output = output.replace('True', '\\cmark')
output = output.replace('False', '\\xmark')
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/building_blocks.tex", 'w') as f:
  f.write(output)
