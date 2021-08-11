exp_dir = Dir.pwd + '/exp'
data_dir = Dir.pwd + '/data'

only_public = !ENV['ONLY_PUBLIC'].nil?

file "paper/ch_potentials.pdf" => [
  "paper/ch_potentials.tex",
  "paper/fig/scaled_weights.pdf",
  "paper/fig/lazy_rphast_many_to_one.pdf",
  "paper/fig/lazy_rphast_many_to_many.pdf",
  "paper/table/applications.tex",
  "paper/table/graphs.tex",
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

  file "paper/fig/lazy_rphast_many_to_one.pdf" => FileList["#{exp_dir}/rphast/*.json"] + ["eval/rphast_comparison.py"] do
    sh "eval/rphast_comparison.py"
  end

  file "paper/fig/lazy_rphast_many_to_many.pdf" => FileList["#{exp_dir}/rphast/*.json"] + ["eval/rphast_comparison.py"] do
    sh "eval/rphast_comparison.py"
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

  file "paper/table/graphs.tex" => FileList[
    "#{exp_dir}/preprocessing/*.out",
    "#{exp_dir}/applications/*.json",
    ] + ["eval/graphs.py", "paper/table"] do

    sh "eval/graphs.py"
  end

  file "paper/table/building_blocks.tex" => FileList["#{exp_dir}/building_blocks/*.json"] + ["eval/building_blocks.py", "paper/table"] do
    sh "eval/building_blocks.py"
  end
end

osm_ger_src = 'https://download.geofabrik.de/europe/germany-200101.osm.pbf'
osm_ger_src_file = "#{data_dir}/germany-200101.osm.pbf"
osm_eur_src = 'https://download.geofabrik.de/europe-200101.osm.pbf'
osm_eur_src_file = "#{data_dir}/europe-200101.osm.pbf"

osm_ger = "#{data_dir}/osm_ger/"
osm_ger_td = "#{data_dir}/osm_ger_td/"
osm_eur = "#{data_dir}/osm_europe/"
dimacs_graph = "#{data_dir}/europe/"

# static_graphs = [osm_ger, osm_eur, dimacs_graph]
static_graphs = [osm_ger]
td_graphs = []
unless only_public
  td_graphs += [
    "#{data_dir}/ptv17/",
    "#{data_dir}/ger06/",
    osm_ger_td
  ]
end
graphs = static_graphs + td_graphs

live_dir = "#{data_dir}/mapbox/live-speeds/2019-08-02-15:41/"
typical_glob = "#{data_dir}/mapbox/typical-speeds/**/**/*.csv"
typical_file = "#{data_dir}/mapbox/typical-tuesday-cleaned.csv"

namespace "prep" do
  file typical_file do
    Dir.chdir "code/rust_road_router" do
      sh "cat #{typical_glob} | cargo run --release --bin scrub_td_mapbox -- 576 864 > #{typical_file}"
    end
  end

  file osm_ger_src_file => data_dir do
    sh "wget -O #{osm_ger_src_file} #{osm_ger_src}"
  end

  file osm_eur_src_file => data_dir do
    sh "wget -O #{osm_eur_src_file} #{osm_eur_src}"
  end

  directory osm_ger
  file osm_ger => ["code/osm_import/build/import_osm", osm_ger_src_file] do
    wd = Dir.pwd
    Dir.chdir osm_ger do
      if only_public
        sh "#{wd}/code/osm_import/build/import_osm #{osm_ger_src_file}"
      else
        sh "#{wd}/code/osm_import/build/import_osm #{osm_ger_src_file} #{Dir[live_dir + '*'].join(' ')} #{typical_file}"
      end
    end
  end

  directory osm_ger_td
  file osm_ger_td => [osm_ger, typical_file] do
    ['first_out', 'head', 'travel_time', 'geo_distance', 'osm_node_ids', 'arc_category', 'forbidden_turn_from_arc', 'forbidden_turn_to_arc', 'latitude', 'longitude', 'tail'].each do |file|
      sh "ln -s #{osm_ger}/#{file} #{osm_ger_td}/#{file}"
    end
    Dir.chdir "code/rust_road_router" do
      sh "cargo run --release --bin import_td_mapbox -- #{osm_ger_td} #{typical_file}"
    end
  end

  directory osm_eur
  file osm_eur => ["code/osm_import/build/import_osm", osm_eur_src_file] do
    wd = Dir.pwd
    Dir.chdir osm_eur do
      sh "#{wd}/code/osm_import/build/import_osm #{osm_eur_src_file}"
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
      sh "ln -s #{graph}/travel_time #{graph}/lower_bound" unless File.exist? "#{graph}/lower_bound"
    end
  end

  td_graphs.each do |graph|
    file graph + "lower_bound" => graph do
      Dir.chdir "code/rust_road_router" do
        sh "cargo run --release --bin td_lower_bound -- #{graph}"
      end
    end
  end
end

namespace "exp" do
  desc "Run all experiments"
  task all: [:preprocessing, :scaled_weights, :building_blocks, :rphast, :bidir_features, :applications]

  task queries: [:scaled_weights, :building_blocks, :applications]

  directory "#{exp_dir}/preprocessing"
  directory "#{exp_dir}/scaled_weights"
  directory "#{exp_dir}/building_blocks"
  directory "#{exp_dir}/rphast"
  directory "#{exp_dir}/bidir_features"
  directory "#{exp_dir}/applications"

  task preprocessing: ["code/compute_ch/build/compute_ch", "#{exp_dir}/preprocessing"] + graphs.map { |g| g + 'lower_bound' } do
    graphs.each do |graph|
      10.times do
        filename = "#{exp_dir}/preprocessing/" + `date --iso-8601=seconds`.strip + '.out'
        sh "echo '#{graph}' >> #{filename}"
        sh("code/compute_ch/build/compute_ch #{graph}/first_out #{graph}/head #{graph}/lower_bound " +
          "/dev/null " +
          "/dev/null /dev/null /dev/null " +
          "/dev/null /dev/null /dev/null " +
          ">> #{filename}")
      end
    end
  end

  task scaled_weights: static_graphs.map { |g| g + 'lower_bound_ch' } + ["#{exp_dir}/scaled_weights"] do
    Dir.chdir "code/rust_road_router" do
      static_graphs.each do |graph|
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_weight_scaling -- #{graph} > #{exp_dir}/scaled_weights/$(date --iso-8601=seconds).json"
      end
    end
  end

  task building_blocks: static_graphs.map { |g| g + 'lower_bound_ch' } + ["#{exp_dir}/building_blocks"] do
    Dir.chdir "code/rust_road_router" do
      static_graphs.each do |graph|
        sh "cargo run --release --bin chpot_simple_scale_dijkstra --features 'chpot-oracle' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "cargo run --release --bin chpot_simple_scale_dijkstra --features 'chpot-alt' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "cargo run --release --bin chpot_simple_scale_dijkstra --features '' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "cargo run --release --bin chpot_simple_scale_dijkstra --features 'chpot-only-topo' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-only-topo chpot-no-bcc chpot-no-deg2 chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-only-topo chpot-no-deg2 chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-only-topo chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-only-topo' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-no-bcc chpot-no-deg2 chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-no-deg2 chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-oracle chpot-no-bcc chpot-no-deg2 chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-oracle chpot-no-deg2 chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-oracle chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-oracle' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-alt chpot-no-bcc chpot-no-deg2 chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-alt chpot-no-deg2 chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-alt chpot-no-deg3' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
        sh "NUM_DIJKSTRA_QUERIES=0 cargo run --release --bin chpot_simple_scale --features 'chpot-alt' -- #{graph} > #{exp_dir}/building_blocks/$(date --iso-8601=seconds).json"
      end
    end
  end

  task rphast: static_graphs.map { |g| g + 'lower_bound_ch' } + ["#{exp_dir}/rphast"] do
    Dir.chdir "code/rust_road_router" do
      static_graphs.each do |graph|
        sh "cargo run --release --bin lazy_rphast -- #{graph} > #{exp_dir}/rphast/$(date --iso-8601=seconds).json"
      end
    end
  end

  task bidir_features: static_graphs.map { |g| g + 'lower_bound_ch' } + ["#{exp_dir}/bidir_features"] do
    Dir.chdir "code/rust_road_router" do
      static_graphs.each do |graph|
        [[], ['chpot-improved-pruning']].each do |pruning_feats|
          [[], ['chpot-alt'], ['chpot-oracle'], ['chpot-only-topo']].each do |pot_feats|
            sh "cargo run --release --bin chpot_bidir #{features(pruning_feats + pot_feats)} -- #{graph} > #{exp_dir}/bidir_features/$(date --iso-8601=seconds).json"
          end
        end
      end
    end
  end

  task applications: graphs.map { |g| g + 'lower_bound_ch' } + ["#{exp_dir}/applications"] do
    Dir.chdir "code/rust_road_router" do
      td_graphs.each do |graph|
        sh "cargo run --release --bin chpot_td -- #{graph} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      end

      unless only_public
        sh "cargo run --release --bin chpot_live -- #{osm_ger} #{live_dir} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
        sh "cargo run --release --bin chpot_td_live -- #{osm_ger_td} #{live_dir} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
        sh "cargo run --release --bin chpot_turns_td_live -- #{osm_ger_td} #{live_dir} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      end

      sh "cargo run --release --bin chpot_unmodified -- #{osm_ger} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      sh "cargo run --release --bin chpot_turns -- #{osm_ger} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      sh "cargo run --release --bin chpot_blocked -- #{osm_ger} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"

      # sh "cargo run --release --bin chpot_unmodified -- #{osm_eur} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      # sh "cargo run --release --bin chpot_turns -- #{osm_eur} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      # sh "cargo run --release --bin chpot_blocked -- #{osm_eur} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      # sh "cargo run --release --bin chpot_unmodified -- #{dimacs_graph} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
      # sh "cargo run --release --bin chpot_simulated_live -- #{dimacs_graph} > #{exp_dir}/applications/$(date --iso-8601=seconds).json"
    end
  end
end

def features(feats)
  if feats.empty?
    ""
  else
    "--features '#{feats.join(' ')}'"
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

