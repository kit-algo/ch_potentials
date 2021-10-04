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
    **algo,
    'BCC': run['bcc_core'],
    'Skip Deg. 2': run['skip_deg2'],
    'Skip Deg. 3': run['skip_deg3'],
    'Heuristic': run['potential'],
    'num_queue_pops': algo['num_queue_pops'] + algo.get('core_search', {}).get('num_queue_pops', 0),
    'num_queue_pushs': algo['num_queue_pushs'] + algo.get('core_search', {}).get('num_queue_pushs', 0),
    'num_relaxed_arcs': algo['num_relaxed_arcs'] + algo.get('core_search', {}).get('num_relaxed_arcs', 0),
} for run in data for algo in run['algo_runs'] if algo.get('algo') in ['Virtual Topocore Component Query', 'Dijkstra Query']])

table = queries.groupby(['Heuristic', 'BCC', 'Skip Deg. 2', 'Skip Deg. 3'])[['running_time_ms', 'num_queue_pushs']].mean()
table['num_queue_pushs'] = table['num_queue_pushs'] / 1000
table = table.round(1)
table = table.unstack(0).stack(0) \
    .swaplevel(2, 3).swaplevel(1,2).swaplevel(0,1) \
    .loc[['running_time_ms', 'num_queue_pushs']][['Zero', 'ALT', 'CH', 'CCH', 'Oracle']].reset_index(level=[2,3]) \
    .rename(index={
        'running_time_ms': R'\multirow{4}{*}{\rotatebox[origin=c]{90}{\shortstack{Running\\time [ms]}}}',
        'num_queue_pushs': R'\multirow{4}{*}{\rotatebox[origin=c]{90}{\shortstack{Queue\\pushs [$\cdot 10^3$]}}}',
    })

lines = table.to_latex(escape=False).split("\n")

lines = [R'\begin{tabular}{clllrrrrr}'] + [lines[1]] + [
    R"       & BCC & Deg2 & Deg3 & Zero & ALT & CH & CCH & Oracle \\"
] + lines[4:]

output = "\n".join(lines) + "\n"
output = output.replace('True', '\\cmark')
output = output.replace('False', '\\xmark')
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/building_blocks.tex", 'w') as f:
  f.write(output)
