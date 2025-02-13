#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num, random, output)
  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose =  0
  sim.set_database(detector_configuration: "database/detector_configuration.xml",
            detector_parameters: "database/detector_parameters_with_pedestal_generation.xml")
            sim.set_gdml ("database/mass_model.gdml")

                   # 標準的なハドロン相互作用を使用（BICモデル）
    sim.set_physics(
        hadron_model: "BIC",        # BIC (Binary Cascade) モデルを使用
        hadron_hp: false,           # High Precision Physicsはオフ
        cut_value: 0.001,           # カット値を設定
        customized_em: false,       # Livermore Physicsを使用するためfalseに設定
        radioactive_decay: true     # 放射性崩壊を追加
    )

    sim.set_primary_generator :PlaneWavePrimaryGen, {
      particle: "gamma",
      photon_index: 1.0,
      energy_min: 1.0,
      energy_max: 100.0,
      position: vec(0.0, 0.0, 1.0),
      direction: vec(0.0, 0.0, -1.0),
      radius: 0.0,
    }
  }
  sim.visualize(mode: 'OGLSQt')
  sim.print_detector_info
  sim.run(num)
end


### main ###
num = 200
output = "simulation_hit.root"
random = 0
run_simulation(num, random, output)
