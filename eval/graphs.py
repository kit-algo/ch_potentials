#!/usr/bin/env python3

import pandas as pd

import json
import glob
import os
import re

base = "exp/"
cwd = os.getcwd()
paths = glob.glob(base + "preprocessing/*.json")
data = [json.load(open(path)) for path in paths]

def path_to_graph(path):
    return {
        'ger06': 'TDGer06',
        'ptv17': 'TDEur17',
        'ptv20': 'TDEur20',
        'osm_europe': 'OSM Europe',
        'osm_ger': 'OSM Ger',
        'osm_ger_td': 'TD OSM Ger',
        'europe': 'DIMACs Europe',
    }[[x for x in path.split('/') if x != ''][-1]]

graphs = pd.DataFrame.from_records([{
    **run
    'graph': path_to_graph(run['args'][1]),
    'num_nodes': run['graph'],
    'num_edges': run['graph'],
} for run in data if 'graph' in run])

runtime_pattern = re.compile(".*Needed (\\d+)musec\\..*")

def parse_contraction_output(path):
    stats = { 'ch_contraction_running_time_s': 0.0 }

    with open(path, 'r') as f:
        for line in f:
            if not 'graph' in stats:
                stats['graph'] = path_to_graph(line.strip())
            else:
                match = runtime_pattern.match(line)
                if match:
                    stats['ch_contraction_running_time_s'] += int(match[1]) / 1000000

    return stats

ch_preprocessing = pd.DataFrame.from_records([parse_contraction_output(path) for path in glob.glob(base + "preprocessing/ch/*.out")])

runtime_pattern = re.compile(".*running time : (\\d+)musec.*")

def parse_flowcutter_partition_output(path):
  stats = { 'cch_ordering_running_time_s': 0.0 }

  with open(path, 'r') as f:
    for line in f:
      if not 'graph' in stats:
        stats['graph'] = path_to_graph(line.strip())
      else:
        match = runtime_pattern.match(line)
        if match:
          stats['cch_ordering_running_time_s'] += int(match[1]) / 1000000

  return stats

cch_ordering = pd.DataFrame.from_records([parse_contraction_output(path) for path in glob.glob(base + "preprocessing/cch/*.out")])

table = graphs.groupby(['graph'])[['num_nodes', 'num_edges']].mean()

table = table.reindex(['OSM Ger', 'DIMACs Europe', 'TDGer06', 'TDEur17', 'TDEur20'])

table = table.join(ch_preprocessing.groupby('graph').mean()).join(cch_ordering.groupby('graph').mean())
table['num_nodes'] = table['num_nodes'] / 1000000.0
table['num_edges'] = table['num_edges'] / 1000000.0
table = table.round(1)

lines = table.to_latex(escape=False).split("\n")

lines = lines[:2] + [
  R" &          Nodes &          Edges & Preprocessing \\"
  R" & $[\cdot 10^6]$ & $[\cdot 10^6]$ &           [s] \\"
] + lines[4:]

output = "\n".join(lines) + "\n"
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/graphs.tex", 'w') as f:
  f.write(output)
