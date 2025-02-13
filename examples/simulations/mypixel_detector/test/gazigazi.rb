#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num, random, output)
    energy = 5.9 # keV
  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.set_database(detector_configuration: "database/detector_configuration.xml",
                   detector_parameters: "database/detector_parameters_with_pedestal_generation.xml")
  sim.set_gdml("database/mass_model.gdml")

  # 標準的なハドロン相互作用を使用（BICモデル）
  sim.set_physics(
    hadron_model: "BIC",
    hadron_hp: false,
    cut_value: 0.001,
    customized_em: false,
    radioactive_decay: true
  )

  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "gamma",
    photon_index: 0.0,
    energy_min: energy,
    energy_max: energy,
    position: vec(0.0, 0.0, 1.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 0.1,
  }

  sim.print_detector_info
  sim.run(num)
end

### main ###
num = 2000000
output = "simulation_Efeaild55to15_test.root"
random = 0
run_simulation(num, random, output)

