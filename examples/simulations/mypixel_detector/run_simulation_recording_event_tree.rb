#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num, random, output)


  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.print_detector_info
  sim.set_database(detector_configuration: "database/detector_configuration.xml",
                   detector_parameters: "database/detector_parameters.xml",
                   channel_map: "database/channel_map.xml",
                   channel_properties: "database/channel_properties.xml")
  sim.set_gdml "database/mass_model.gdml"
  sim.set_physics(hadron_hp: false, cut_value: 0.001)

  sim.set_primary_generator :PlaneWavePrimaryGen, {
    particle: "gamma",
    position: vec(0.0, 0.0, 10.0),
    direction: vec(0.0, 0.0, -1.0),
    radius: 2.3
  }

  sim.use_tree_format("eventtree", notice_undetected: true)

  sim.run(num)
end

### main ###
num = 100000
output = "simulation_eventtree.root"
random = 0
run_simulation(num, random, output)
