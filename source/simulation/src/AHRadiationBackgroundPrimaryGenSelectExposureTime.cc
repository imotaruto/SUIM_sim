/*************************************************************************
 *                                                                       *
 * Copyright (c) 2013 Tamotsu Sato, Hiro  kazu Odaka                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * any later version.                                                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#include "AHRadiationBackgroundPrimaryGenSelectExposureTime.hh"
#include "AstroUnits.hh"
#include "TFile.h"
#include "TH1.h"
#include "TGraph.h"
#include "Randomize.hh"
#include <iostream>

namespace unit = anlgeant4::unit;

namespace comptonsoft
{

AHRadiationBackgroundPrimaryGenSelectExposureTime::AHRadiationBackgroundPrimaryGenSelectExposureTime()
  : m_Filename("trapped_proton_spectrum.root"),
    m_File(nullptr),
    m_Hist(nullptr),
    exposure_(1000.0),  // デフォルトの露光時間を1000秒に設定
    m_TotalParticlesToGenerate(0),
    m_NumParticlesGenerated(0),
    m_integralParticleIntensity(0.0),
    m_integralEnergyIntensity(0.0)
{
  add_alias("AHRadiationBackgroundPrimaryGenSelectExposureTime");
}

AHRadiationBackgroundPrimaryGenSelectExposureTime::~AHRadiationBackgroundPrimaryGenSelectExposureTime()
{
  delete m_Hist;
}

anlnext::ANLStatus AHRadiationBackgroundPrimaryGenSelectExposureTime::mod_define()
{
  anlgeant4::IsotropicPrimaryGen::mod_define();
  disableDefaultEnergyInput();
  register_parameter(&m_Filename, "filename");
  set_parameter_description("ROOT file of background radiation spectrum.");

  // 露光時間のパラメータを登録
  register_parameter(&exposure_, "exposure", unit::s, "s");
  set_parameter_description("Exposure time in seconds.");

  return anlnext::AS_OK;
}

anlnext::ANLStatus AHRadiationBackgroundPrimaryGenSelectExposureTime::mod_initialize()
{
  anlgeant4::IsotropicPrimaryGen::mod_initialize();

  // ファイルを開く
  m_File.reset(new TFile(m_Filename.c_str()));
  if (m_File->IsZombie()) {
    std::cerr << "Cannot open " << m_Filename << "!" << std::endl;
    return anlnext::AS_QUIT_ERROR;
  }

  // グラフを取得
  TGraph* graph = (TGraph*) m_File->Get("Graph");
  if (!graph) {
    std::cerr << "Error: Graph not found in file " << m_Filename << std::endl;
    return anlnext::AS_QUIT_ERROR;
  }

  // エネルギー配列を取得し、ヒストグラムを作成
  const int N = graph->GetN();
  double* x_array = graph->GetX(); // GeV単位
  std::vector<double> energies(N);
  for (int i = 0; i < N; i++) {
    energies[i] = x_array[i] * unit::GeV;
  }

  // ヒストグラムを初期化
  m_Hist = new TH1D("spectrum", "spectrum", N - 1, &energies[0]);
  m_integralParticleIntensity = 0.0;
  m_integralEnergyIntensity = 0.0;
  std::cout << "** Output spectral information of particles **" << std::endl;

  // 各エネルギービンごとに粒子強度を計算
  for (int bin = 1; bin <= N; bin++) {
    const double energy = m_Hist->GetBinCenter(bin);
    const double differentialIntensity = graph->Eval(energy / unit::GeV) * (1.0 / unit::s / unit::m2 / unit::sr / unit::GeV);
    const double deltaE = m_Hist->GetBinWidth(bin);
    const double particleIntensity = differentialIntensity * deltaE;
    const double energyIntensity = differentialIntensity * deltaE * energy;
    m_Hist->SetBinContent(bin, particleIntensity);
    m_integralParticleIntensity += particleIntensity;
    m_integralEnergyIntensity += energyIntensity;
    std::cout << energy / unit::MeV << " [MeV] : "
              << differentialIntensity / (1.0 / unit::s / unit::cm2 / unit::sr / unit::MeV)
              << " [#/s/cm2/sr/MeV] " << std::endl;
  }

  // カバー範囲と積分を用いて、粒子生成率を計算
  const double circleArea = Radius() * Radius() * CLHEP::pi;
  const double rate = m_integralParticleIntensity * circleArea * (4.0 * CLHEP::pi * CoveringFactor());

  setIntensity(m_integralEnergyIntensity);

  std::cout << " -> Normalization factor: " << rate / (1.0 / unit::s) << " particles/s\n" << std::endl;

  // 露光時間に対応する期待粒子数を計算
  const double expectedParticles = rate * exposure_;  // 修正箇所

  std::cout << "Expected number of particles for exposure time " << exposure_ / unit::s << " s: " << expectedParticles << std::endl;

  // シミュレーションで生成する粒子数を、期待粒子数に設定
  m_TotalParticlesToGenerate = static_cast<int64_t>(expectedParticles);

  // 粒子生成カウンタの初期化
  m_NumParticlesGenerated = 0;

  return anlnext::AS_OK;
}

G4double AHRadiationBackgroundPrimaryGenSelectExposureTime::sampleEnergy()
{
  return m_Hist->GetRandom();
}

anlnext::ANLStatus AHRadiationBackgroundPrimaryGenSelectExposureTime::mod_analyze()
{
  // 粒子生成カウンタを増加させ、指定された数に達したらシミュレーションを終了
  if (m_NumParticlesGenerated >= m_TotalParticlesToGenerate) {
    return anlnext::AS_QUIT;
  }

  // 進捗状況をパーセンテージで表示
  double progress = (static_cast<double>(m_NumParticlesGenerated) / m_TotalParticlesToGenerate) * 100.0;
  std::cout << "Particle generation progress: " << progress << " %" << "\r" << std::flush;

  m_NumParticlesGenerated++;

  return anlgeant4::IsotropicPrimaryGen::mod_analyze();
}

anlnext::ANLStatus AHRadiationBackgroundPrimaryGenSelectExposureTime::mod_end_run()
{
  // 終了時に改行を追加
  std::cout << std::endl;

  // 面積と立体角の計算
  const double area = CLHEP::pi * Radius() * Radius(); // 単位は cm^2
  const double solidAngle = 4 * CLHEP::pi * CoveringFactor(); // 単位は sr

  // 総エネルギー強度の計算
  const double totalEnergyIntensity = m_integralEnergyIntensity * area * solidAngle; // 単位は erg/s

  // シミュレーションで生成された総エネルギーを取得
  const double totalSimulatedEnergy = TotalEnergy(); // 単位は erg

  // リアルタイムの計算
  const double realTime = exposure_; // 修正箇所：露光時間をそのまま使用

  // 結果の表示
  std::cout << "Total simulated energy: " << totalSimulatedEnergy / unit::erg << " erg" << std::endl;
  std::cout << "Total energy intensity: " << totalEnergyIntensity / (unit::erg / unit::s) << " erg/s" << std::endl;
  std::cout << "Real time corresponding to simulation: " << realTime / unit::s << " s" << std::endl;

  // リアルタイムを設定
  setRealTime(realTime);

  return anlnext::AS_OK;
}

} /* namespace comptonsoft */







