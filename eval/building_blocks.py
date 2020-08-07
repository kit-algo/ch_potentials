#!/usr/bin/env python3

import pandas as pd

import json
import glob
import os
import re

base = os.environ['CHPOT_EXP_OUTPUT_DIR']
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
        return '-'
    elif 'CHPOT_ORACLE' in feature_flags:
        return 'Oracle'
    else:
        return 'CH'

queries['Heuristic'] = queries['features'].apply(heuristic_name)

table = queries.groupby(['algo', 'Heuristic', 'BCC', 'Skip Deg. 2', 'Skip Deg. 3'])[['running_time_ms', 'num_queue_pops', 'num_relaxed_arcs']].mean()
table['num_relaxed_arcs'] = table['num_relaxed_arcs'] / 1000
table['num_queue_pops'] = table['num_queue_pops'] / 1000
table = table.round(1)

# table = table.rename(index={
#     'CHPOT_NO_BCC, CHPOT_NO_DEG2, CHPOT_NO_DEG3, CHPOT_ONLY_TOPO, ': 'No Deg2, No Deg3, No BCC, No Pot.',
#     'CHPOT_NO_DEG2, CHPOT_NO_DEG3, CHPOT_ONLY_TOPO, ':               'No Deg2, No Deg3, No Pot.',
#     'CHPOT_NO_DEG3, CHPOT_ONLY_TOPO, ':                              'No Deg3, No Pot.',
#     'CHPOT_ONLY_TOPO, ':                                             'No Pot.',
#     'CHPOT_NO_BCC, CHPOT_NO_DEG2, CHPOT_NO_DEG3, ':                  'No Deg2, No Deg3, No BCC',
#     'CHPOT_NO_DEG2, CHPOT_NO_DEG3, ':                                'No Deg2, No Deg3',
#     'CHPOT_NO_DEG3, ':                                               'No Deg3',
# })

# table = table.reindex(['No Deg2, No Deg3, No BCC, No Pot.', 'No Deg2, No Deg3, No Pot.', 'No Deg3, No Pot.', 'No Pot.',
#     'No Deg2, No Deg3, No BCC', 'No Deg2, No Deg3', 'No Deg3', ''])

lines = table.to_latex(escape=False).split("\n")

lines = lines[:2] + [
    R" & & & & & Running Time &     Queue Pops &   Relaxed Arcs \\",
    R" & Heur. & BCC & Deg. 2 & Deg. 3 &         [ms] & [$\cdot 10^3$] & [$\cdot 10^3$] \\"
] + lines[4:]

output = "\n".join(lines) + "\n"
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/building_blocks.tex", 'w') as f:
  f.write(output)
