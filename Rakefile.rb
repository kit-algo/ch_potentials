raise "No or wrong experiments output dir configured - please set CHPOT_EXP_OUTPUT_DIR env var" unless Dir.exists? ENV['CHPOT_EXP_OUTPUT_DIR'] || ""
exp_dir = ENV['CHPOT_EXP_OUTPUT_DIR']

file "paper/ch_potentials.pdf" => [
  "paper/ch_potentials.tex",
  "paper/fig/scaled_weights.pdf",
  "paper/table/applications.tex",
  "paper/table/building_blocks.tex"] do

  Dir.chdir "paper" do
    sh "latexmk -pdf ch_potentials.tex"
  end
end

task default: "paper/ch_potentials.pdf"

namespace "fig" do
  file "paper/fig/scaled_weights.pdf" => FileList["#{exp_dir}/scaled_weights/*.json"] + ["eval/scaled_weights.py"] do
    sh "eval/scaled_weights.py"
  end
end

namespace "table" do
  directory "paper/table"

  file "paper/table/applications.tex" => FileList[
    "#{exp_dir}/preprocessing/*.out",
    "#{exp_dir}/applications/*.json",
    ] + ["eval/applications.py", "paper/table"] do

    sh "eval/applications.py"
  end

  file "paper/table/building_blocks.tex" => FileList["#{exp_dir}/building_blocks/*.json"] + ["eval/building_blocks.py", "paper/table"] do
    sh "eval/building_blocks.py"
  end
end

osm_graph = "/algoDaten/zeitz/roadgraphs/osm_ger_rel/"
dimacs_graph = "/algoDaten/zeitz/roadgraphs/europe/"

static_graphs = [osm_graph, dimacs_graph]
td_graphs = [
  "/algoDaten/graphs/cleaned_td_road_data/ptv17-eur-car/day/di/",
  "/algoDaten/graphs/cleaned_td_road_data/de/day/dido/",
]
graphs = static_graphs + td_graphs

live_dir = '/algoDaten/mapbox/live-speeds/2019-08-02-15:41/'

namespace "prep" do
  directory osm_graph
  file osm_graph => "code/osm_import/build/import_osm" do
    wd = Dir.pwd
    Dir.chdir osm_graph do
      sh "#{wd}/code/osm_import/build/import_osm /algoDaten/zeitz/roadgraphs/germany-latest.osm.pbf #{Dir[live_dir + '*'].join(' ')}"
    end
  end

  graphs.each do |graph|
    directory graph + "lower_bound_ch"
    file graph + "lower_bound_ch" => ["code/compute_ch/build/compute_ch", graph + 'lower_bound'] do
      sh("code/compute_ch/build/compute_ch #{graph}/first_out #{graph}/head #{graph}/lower_bound " +
          "#{graph}/lower_bound_ch/order " +
          "#{graph}/lower_bound_ch/forward_first_out #{graph}/lower_bound_ch/forward_head #{graph}/lower_bound_ch/forward_weight " +
          "#{graph}/lower_bound_ch/backward_first_out #{graph}/lower_bound_ch/backward_head #{graph}/lower_bound_ch/backward_weight")
    end
  end

  static_graphs.each do |graph|
    file graph + "lower_bound" => graph do
      sh "ln -s #{graph}/travel_time #{graph}/lower_bound" unless File.exist? "#{graph}/travel_time"
    end
  end

  td_graphs.each do |graph|
    file graph + "lower_bound" do
      Dir.chdir "code/bmw_routing_engine/engine" do
        sh "cargo run --release --bin td_lower_bound -- #{graph}"
      end
    end
  end
end

namespace "exp" do
  desc "Run all experiments"
  task all: [:preprocessing, :scaled_weights, :building_blocks, :applications]

  task queries: [:scaled_weights, :building_blocks, :applications]

  directory "#{exp_dir}/preprocessing"
  directory "#{exp_dir}/scaled_weights"
  directory "#{exp_dir}/building_blocks"
  directory "#{exp_dir}/applications"

  task preprocessing: ["code/compute_ch/build/compute_ch", "#{exp_dir}/preprocessing"] + graphs.map { |g| g + 'lower_bound' } do
    graphs.each do |graph|
      5.times do
        filename = "#{exp_dir}/preprocessing/" + `date --iso-8601=seconds`.strip + '.out'
        sh "echo '#{graph}' >> #{filename}"
        sh("code/compute_ch/build/compute_ch #{graph}/first_out #{graph}/head #{graph}/lower_bound " +
          "#{graph}/lower_bound_ch/order " +
          "#{graph}/lower_bound_ch/forward_first_out #{graph}/lower_bound_ch/forward_head #{graph}/lower_bound_ch/forward_weight " +
          "#{graph}/lower_bound_ch/backward_first_out #{graph}/lower_bound_ch/backward_head #{graph}/lower_bound_ch/backward_weight " +
          ">> #{filename}")
      end
    end
  end

  task scaled_weights: [dimacs_graph + 'lower_bound_ch', osm_graph + 'lower_bound_ch'] + ["#{exp_dir}/scaled_weights"] do
    Dir.chdir "code/bmw_routing_engine/engine" do
      sh "cargo run --release --bin chpot_weight_scaling -- #{dimacs_graph} > #{exp_dir}/scaled_weights/$(date --iso-8601=seconds).json"
      sh "cargo run --release --bin chpot_weight_scaling -- #{osm_graph} > #{exp_dir}/scaled_weights/$(date --iso-8601=seconds).json"
    end
  end

  task building_blocks: [dimacs_graph + 'lower_bound_ch'] + ["#{exp_dir}/building_blocks"] do
    Dir.chdir "code/bmw_routing_engine/engine" do
      sh "cargo run --release --bin chpot_simple_scale --features 'chpot-only-topo chpot-no-scc chpot-no-deg2 chpot-no-deg3' -- #{dimacs_graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
      sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-only-topo chpot-no-deg2 chpot-no-deg3' -- #{dimacs_graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
      sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-only-topo chpot-no-deg3' -- #{dimacs_graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
      sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-only-topo' -- #{dimacs_graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
      sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-no-scc chpot-no-deg2 chpot-no-deg3' -- #{dimacs_graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
      sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-no-deg2 chpot-no-deg3' -- #{dimacs_graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
      sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-no-deg3' -- #{dimacs_graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
      sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale -- #{dimacs_graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
    end
  end

  task applications: graphs.map { |g| g + 'lower_bound_ch' } + ["#{exp_dir}/applications"] do
    Dir.chdir "code/bmw_routing_engine/engine" do
      td_graphs.each do |graph|
        sh "cargo run --release --bin chpot_td -- #{graph} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      end
      sh "cargo run --release --bin chpot_live -- #{osm_graph} #{live_dir} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      sh "cargo run --release --bin chpot_turns -- #{osm_graph} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      sh "cargo run --release --bin chpot_blocked -- #{osm_graph} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      sh "cargo run --release --bin chpot_simulated_live -- #{dimacs_graph} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
    end
  end
end

namespace 'build' do
  task :osm_import => "code/osm_import/build/import_osm"

  directory "code/osm_import/build"

  file "code/osm_import/build/import_osm" => ["code/osm_import/build", "code/osm_import/src/bin/import_osm.cpp"] do
    Dir.chdir "code/osm_import/build/" do
      sh "cmake -DCMAKE_BUILD_TYPE=Release .. && make"
    end
  end

  task :compute_ch => "code/compute_ch/build/compute_ch"

  directory "code/compute_ch/build"

  file "code/compute_ch/build/compute_ch" => ["code/compute_ch/build", "code/compute_ch/src/bin/compute_contraction_hierarchy_and_order.cpp"] do
    Dir.chdir "code/compute_ch/build/" do
      sh "cmake -DCMAKE_BUILD_TYPE=Release .. && make"
    end
  end
end

