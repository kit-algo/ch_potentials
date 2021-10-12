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
    .groupby(['potential', 'experiment', 'factor', 'probability', 'algo', 'bidir_pot', 'improved_pruning'])[['running_time_ms', 'num_queue_pushs_k']].mean() \
    .unstack(0) \
    .loc[('weight_scale', 1.05, 1.00)] \
    .reindex(columns=['Zero', 'ALT', 'CH', 'CCH', 'Oracle'], level=1)

pruning[('num_queue_pushs_k', '(C)CH/Oracle')] = pruning[('num_queue_pushs_k', 'CH')]
pruning = pruning.drop([('num_queue_pushs_k', 'CH'), ('num_queue_pushs_k', 'CCH'), ('num_queue_pushs_k', 'Oracle')], axis = 'columns')

pruning = pruning.rename(columns={
        'running_time_ms': 'Running time [ms]',
        'num_queue_pushs_k': 'Queue pushs [$\\cdot 10^3$]',
    }) \
    .reset_index()

lines = pruning.to_latex(escape=False, index=False, multicolumn_format='c', column_format='c@{\hskip6pt}c@{\hskip3pt}crrrrrrrr').split("\n")

lines = lines[0:2] + [
    R' &  &  & \multicolumn{5}{c}{Running time [ms]} & \multicolumn{3}{c}{Queue pushs [$\cdot 10^3$]} \\ \cmidrule(lr){4-8} \cmidrule(lr){9-11}',
    R'Low Deg.      & Bidirectional & New & \multirow{2}{*}{Zero} & \multirow{2}{*}{ALT} & \multirow{2}{*}{CH} & \multirow{2}{*}{CCH} & \multirow{2}{*}{Oracle} & \multirow{2}{*}{Zero} & \multirow{2}{*}{ALT} & (C)CH/ \\',
    R'Opt. & Potential     & Pruning  & & & & & & & & Oracle \\'
] + lines[4:9] + ['\\addlinespace'] + lines[9:]

output = "\n".join(lines) + "\n"
output = output.replace('Bidrectional Dijkstra Query', R'\xmark')
output = output.replace('Virtual Topocore Bidirectional Core Query', R'\cmark')
output = output.replace('True', '\\cmark')
output = output.replace('False', '\\xmark')
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/bidir_pruning.tex", 'w') as f:
  f.write(output)



bidir_switch_strat = queries.loc[lambda x: x['bidir_pot'] != 'unidir'] \
    .loc[lambda x: x['algo'].str.contains('Topocore')] \
    .loc[lambda x: ~(((x['bidir_pot'] == 'Average') & (x['improved_pruning'] == True)) | ((x['bidir_pot'] == 'Symmetric') & (x['improved_pruning'] == False)))] \
    .groupby(['potential', 'experiment', 'factor', 'probability', 'bidir_pot', 'choose_direction_strategy'])[['running_time_ms', 'num_queue_pushs_k']].mean().unstack(0) \
    .loc[('probabilistic_scale_by_speed', 1.50)] \
    .reindex(columns=['Zero', 'ALT', 'CH', 'CCH', 'Oracle'], level=1)

bidir_switch_strat[('num_queue_pushs_k', '(C)CH/Oracle')] = bidir_switch_strat[('num_queue_pushs_k', 'CH')]
bidir_switch_strat = bidir_switch_strat.drop([('num_queue_pushs_k', 'CH'), ('num_queue_pushs_k', 'CCH'), ('num_queue_pushs_k', 'Oracle')], axis = 'columns')

bidir_switch_strat = bidir_switch_strat.rename(columns={
        'running_time_ms': 'Running time [ms]',
        'num_queue_pushs_k': 'Queue pushs [$\\cdot 10^3$]',
    }) \
    .rename(index={
        0.00: '> 80kph',
        1.00: '< 80kph',
        'alternating': 'Alternating',
        'min_key': 'Min. Key',
    })

table = bidir_switch_strat.reset_index()
table.loc[1:3, 'probability'] = ''
table.loc[5:9, 'probability'] = ''

lines = table.to_latex(escape=False, index=False, multicolumn_format='c', column_format='cccrrrrrrrr').split("\n")

lines = lines[0:2] + [
    R' &  &  & \multicolumn{5}{c}{Running time [ms]} & \multicolumn{3}{c}{Queue pushs [$\cdot 10^3$]} \\ \cmidrule(lr){4-8} \cmidrule(lr){9-11}'
    R'\multirow{2}{*}{$w_q$} & Bidirectional & Choose    & \multirow{2}{*}{Zero} & \multirow{2}{*}{ALT} & \multirow{2}{*}{CH} & \multirow{2}{*}{CCH} & \multirow{2}{*}{Oracle} & \multirow{2}{*}{Zero} & \multirow{2}{*}{ALT} & (C)CH/ \\',
    R' & Potential     & Direction & & & & & & & & Oracle \\'
] + lines[4:9] + ['\\addlinespace'] + lines[9:]

output = "\n".join(lines) + "\n"
output = output.replace('> 80kph', R'\multirow{4}{*}{\rotatebox[origin=c]{90}{\shortstack{$w_{\ell} \cdot 1.5$ if\\ $v >$ 80kph}}}')
output = output.replace('< 80kph', R'\multirow{4}{*}{\rotatebox[origin=c]{90}{\shortstack{$w_{\ell} \cdot 1.5$ if\\ $v <$ 80kph}}}')
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/bidir_switching.tex", 'w') as f:
  f.write(output)



bidir = queries.loc[lambda x: x['algo'].str.contains('Topocore')] \
    .loc[lambda x: x['choose_direction_strategy'] != 'min_key'] \
    .loc[lambda x: ~(((x['bidir_pot'] == 'Average') & (x['improved_pruning'] == True)) | ((x['bidir_pot'] == 'Symmetric') & (x['improved_pruning'] == False)))] \
    .groupby(['potential', 'experiment', 'factor', 'probability', 'bidir_pot'])[['running_time_ms', 'num_queue_pushs_k']].mean().unstack(0) \
    .reindex(columns=['Zero', 'ALT', 'CH', 'CCH', 'Oracle'], level=1) \
    .reindex(index=['weight_scale', 'probabilistic_scale_by_speed'], level=0) \
    .reindex(index=[1.0], level=2) \
    .reindex(index=['unidir', 'Average', 'Symmetric'], level=3)

bidir[('num_queue_pushs_k', '(C)CH/Oracle')] = bidir[('num_queue_pushs_k', 'CH')]
bidir = bidir.drop([('num_queue_pushs_k', 'CH'), ('num_queue_pushs_k', 'CCH'), ('num_queue_pushs_k', 'Oracle')], axis = 'columns')

bidir = bidir.rename(columns={
    'running_time_ms': 'Running time [ms]',
    'num_queue_pushs_k': 'Queue pushs [$\\cdot 10^3$]',
}, index={'unidir': 'Unidirectional'})

bidir.loc[('probabilistic_scale_by_speed', 1.5, 1.0), '$w_q$'] = '< 80kph'
bidir.loc[('weight_scale', 1.0, 1.0), '$w_q$'] = 'unmodified'
bidir.loc[('weight_scale', 1.05, 1.0), '$w_q$'] = '*1.05'
bidir = bidir.reset_index(level=[0,1,2], drop=True).reset_index().set_index(['$w_q$', 'bidir_pot'])

lines = bidir.to_latex(escape=False, multicolumn_format='c', column_format='ccrrrrrrrr').split("\n")

lines = lines[0:2] + [
    R' &  & \multicolumn{5}{c}{Running time [ms]} & \multicolumn{3}{c}{Queue pushs [$\cdot 10^3$]} \\ \cmidrule(lr){3-7} \cmidrule(lr){8-10}'
    R'\multirow{2}{*}{$w_q$} & & \multirow{2}{*}{Zero} & \multirow{2}{*}{ALT} & \multirow{2}{*}{CH} & \multirow{2}{*}{CCH} & \multirow{2}{*}{Oracle} & \multirow{2}{*}{Zero} & \multirow{2}{*}{ALT} & (C)CH/ \\',
    R' & & & & & & & & & Oracle \\'
] + lines[5:9] + ['\\addlinespace'] + lines[9:12] + ['\\addlinespace'] + lines[12:]

output = "\n".join(lines) + "\n"
output = output.replace('> 80kph', R'\multirow{3}{*}{\shortstack{$w_{\ell} \cdot 1.5$ if\\ $v >$ 80kph}}')
output = output.replace('< 80kph', R'\multirow{3}{*}{\shortstack{$w_{\ell} \cdot 1.5$ if\\ $v <$ 80kph}}')
output = output.replace('unmodified', R'\multirow{3}{*}{$w_{\ell}$}')
output = output.replace('*1.05', R'\multirow{3}{*}{$w_{\ell} \cdot 1.05$}')
output = re.sub(re.compile('([0-9]{3}(?=[0-9]))'), '\\g<0>,\\\\', output[::-1])[::-1]

with open("paper/table/bidir.tex", 'w') as f:
  f.write(output)
