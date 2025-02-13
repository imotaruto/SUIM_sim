#!/usr/bin/env ruby

require 'comptonsoft'

# AssignEventTime クラスの定義
class AssignEventTime < ANL::ANLApp
  attr_accessor :inputs, :output

  def set_time_info(t0, exposure)
    @time_start = t0
    @exposure = exposure
  end

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadEventTree
    with_parameters(file_list: @inputs)
    chain :AssignTime
    with_parameters(
      number_of_events: -1,
      time_start: @time_start,
      exposure: @exposure,
      sort: true,
      random_seed: 0
    )
    chain :WriteEventTree
    chain :SaveData
    with_parameters(output: @output)
  end
end

# DefineEventDrivenApp クラスの定義
class DefineEventDrivenApp < ANL::ANLApp
  attr_accessor :inputs, :output
  attr_accessor :event_threshold, :exposure_time
  attr_accessor :sprit_threshold, :delay_time # sprit_threshold を追加

  def setup()
    add_namespace ComptonSoft

    chain :CSHitCollection
    chain :ReadEventTree
    with_parameters(file_list: @inputs)
    chain :DefineEventDriven
    with_parameters(
      event_threshold: @event_threshold,
      exposure_time: @exposure_time
    )
    chain :Write3x3EventTree
    with_parameters(
      input_filename: @inputs[0],
      output_filename: @output,
      tree_name: 'eventtree',
      sprit_threshold: @sprit_threshold, # sprit_threshold を追加
    exposure_time: @exposure_time      # exposure_time を追加
      # event_threshold は DefineEventDriven に渡すため、ここでは不要
    )
    # SaveData モジュールは不要なため削除
  end
end



# メインスクリプト部分
time_start = 0.0
input_file = "simulation_out_Efeild_fullpix_sield_20mm_mizuno_proton_rad2.root"
##input_file = "simulation.root"
exposure = 157680.000
delay_time = 0.0 # 例えば0.1秒の遅延時間を設定（必要に応じて使用）
filename_base = input_file.gsub(".root","")

# AssignEventTime の実行
a1 = AssignEventTime.new
a1.inputs = [input_file]
a1.output = "#{filename_base}_time.root"
a1.set_time_info(time_start, exposure)
a1.run(:all)

# DefineEventDrivenApp の実行
a2 = DefineEventDrivenApp.new
a2.inputs = [a1.output]
a2.output = "#{filename_base}_event.root"
a2.event_threshold = 1.671 # イベント判定用スレッショルド (keV)
a2.sprit_threshold = 0.5   # パターン判定用スレッショルド (keV) 任意の値に設定
a2.exposure_time = exposure # s
a2.run(:all,1)
