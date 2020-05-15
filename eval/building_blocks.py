#!/usr/bin/env python3

import pandas as pd

import json
import glob
import os

base = os.environ['CHPOT_EXP_OUTPUT_DIR']
paths = glob.glob(base + "building_blocks/*.json")
data = [json.load(open(path)) for path in paths]

queries = pd.DataFrame.from_records([{
    'features': run['feature_flags'].replace('DEFAULT, TDCCH_APPROX, TDCCH_POSTCUSTOMIZATION, TDCCH_PRECUSTOMIZATION, TDCCH_QUERY_ASTAR, TDCCH_TRIANGLE_SORTING', ''),
    **algo }
    for run in data for exp in run.get('experiments', []) for algo in exp['algo_runs'] if algo.get('algo') == 'CH Potentials Query'])

table = queries.groupby('features')[['running_time_ms', 'num_queue_pops', 'num_relaxed_arcs']].mean()
table['num_relaxed_arcs'] = table['num_relaxed_arcs'] / 1000000
table['num_queue_pops'] = table['num_queue_pops'] / 1000

with open("paper/table/building_blocks.tex", 'w') as f:
  f.write(table.to_latex())
