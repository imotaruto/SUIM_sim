#! /usr/bin/env ruby

require 'comptonsoft'

def run_simulation(num, random, output)
  sim = ComptonSoft::Simulation.new
  sim.output = output
  sim.random_seed = random
  sim.verbose = 0
  sim.set_database(detector_configuration: "../database/detector_configuration_fullpix.xml",
            detector_parameters: "database/detector_parameters_with_pedestal_generation3.xml")
            sim.set_gdml ("../database/mass_model_fullpix_slit.gdml")

                   # 標準的なハドロン相互作用を使用（BICモデル）
    sim.set_physics(
        hadron_model: "BIC",        # BIC (Binary Cascade) モデルを使用
        hadron_hp: true,           # High Precision Physicsはオフ
        cut_value: 0.000001,           # カット値を設定
        customized_em: true,       # Livermore Physicsを使用するためfalseに設定
        radioactive_decay: true     # 放射性崩壊を追加
    )

  sim.set_primary_generator :AHRadiationBackgroundPrimaryGenSelectExposureTime, {
    particle: "gamma",
    filename: "GCR_gamma_10keVto1000GeV.root",
    center_position: vec(0.0, 0.0, 0.0),
    radius: 1.0,
    exposure: 157680.000
  }
  sim.use_tree_format("eventtree", notice_undetected: true)
  sim.print_detector_info
 # sim.visualize(mode: 'OGLSQt')
  sim.run(num)
end

### main ###
num = -1
output = "fullpix_slit_gamma_newmodel10keV-1000GeV.root"
random = 0
run_simulation(num, random, output)



