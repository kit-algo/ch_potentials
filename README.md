# CH-Potentials

This repository contains the source code for the CH-Potentials algorithm implementation, experimental evaluation and paper.

### Requirements

To reproduce our results you need the following:

- A linux environment
- Recent GCC (we use `9.3.1`)
- Rust Nightly (`1.47.0`)
- Ruby (`2.5.8`) and the `rake` (`12.3.3`) Gem
- Python (`3.8.5`) with `matplotlib`, `seaborn` and `pandas`

Newer versions of these tools are likely to continue working, some past versions may do also but we cannot give any guarantees.

### Reproducing

To run experiments with publicly available data run

```bash
ONLY_PUBLIC=1 rake
```

This will download all necessary data, run experiments, generate plots and tables and finally regenerate the paper.
You can use the `NUM_DIJKSTRA_QUERIES` (default 1000) and `CHPOT_NUM_QUERIES` (default 10000) environment variables to configure the number of queries performed for each experiment.

#### Caveats

The OSM Germany graph will have less nodes than reported in the paper.
The reason is that RoutingKit omits pure modeling nodes.
However, a lot of the Mapbox data matches to link segments between modeling nodes, so we include these nodes where necessary to match the data.
