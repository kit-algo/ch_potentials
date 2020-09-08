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


queries = pd.DataFrame.from_records([{
    'graph': run['args'][1],
    'num_nodes': (run['graph'] if 'graph' in run else run['experiments'][0]['graph'])['num_nodes'],
    'num_edges': (run['graph'] if 'graph' in run else run['experiments'][0]['graph'])['num_arcs'],
    } for run in data if 'graph' in run or 'experiments' in run])

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

table = queries.groupby(['graph'])[['num_nodes', 'num_edges']].mean()

table = table.reindex([
    cwd + '/data/osm_ger/',
    cwd + '/data/ptv17',
    cwd + '/data/ger06'])

table = table.join(preprocessing.groupby('graph').mean())
table['num_nodes'] = table['num_nodes'] / 1000000.0
table['num_edges'] = table['num_edges'] / 1000000.0
table = table.round(1)

table = table.rename(index={
    cwd + '/data/ger06': 'TDGer06',
    cwd + '/data/ptv17': 'TDEur17',
    cwd + '/data/osm_europe/': 'OSM Europe',
    cwd + '/data/osm_ger/': 'OSM Ger',
    cwd + '/data/osm_ger_td/': 'TD OSM Ger',
    cwd + '/data/europe/': 'DIMACs Europe',
})

lines = table.to_latex(escape=False).split("\n")

lines = lines[:2] + [
  R" &          Nodes &          Edges & Preprocessing \\"
  R" & $[\cdot 10^6]$ & $[\cdot 10^6]$ &           [s] \\"
] + lines[4:]

output = "\n".join(lines) + "\n"
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/graphs.tex", 'w') as f:
  f.write(output)
