#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num, random, output)
#  energy = 59.5 # keV

  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.set_database(detector_configuration: "database/detector_configuration.xml",
                   detector_parameters: "database/detector_parameters.xml")
  sim.set_gdml "database/mass_model.gdml"
#  sim.set_physics(hadron_hp: false, cut_value: 0.001)
  sim.set_physics(hadron_model: "BIC",
                hadron_hp: true)

  sim.set_primary_generator :AHRadiationBackgroundPrimaryGen, {
    particle: "neutron",
    filename: "converted_gcr_flux.root",
    center_position: vec(0.0, 0.0, 0.0),
    radius: 0.1,
  }

  sim.print_detector_info
  sim.run(num)
end

### main ###
num = 20000
output = "simulation.root"
random = 0
run_simulation(num, random, output)

