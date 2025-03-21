/*************************************************************************
 *                                                                       *
 * Copyright (c) 2013 Tamotsu Sato, Hirokazu Odaka                       *
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

#ifndef COMPTONSOFT_AHRADIATIONBACKGROUNDPRIMARYGENSELECTEXPOSURETIME_HH
#define COMPTONSOFT_AHRADIATIONBACKGROUNDPRIMARYGENSELECTEXPOSURETIME_HH

#include "IsotropicPrimaryGen.hh"
#include "AstroUnits.hh"
#include <memory>
#include <string>
#include "TH1.h"
#include "TFile.h"

namespace comptonsoft
{

class AHRadiationBackgroundPrimaryGenSelectExposureTime : public anlgeant4::IsotropicPrimaryGen
{
public:
  AHRadiationBackgroundPrimaryGenSelectExposureTime();
  virtual ~AHRadiationBackgroundPrimaryGenSelectExposureTime();

  virtual anlnext::ANLStatus mod_define() override;
  virtual anlnext::ANLStatus mod_initialize() override;
  virtual anlnext::ANLStatus mod_analyze() override;
  virtual anlnext::ANLStatus mod_end_run() override;

protected:
  virtual G4double sampleEnergy() override;
  // 生成位置と運動量方向を設定するメソッド（ベースクラスの実装をオーバーライド）
  virtual void makePrimarySetting() override;

private:
  std::string m_Filename;
  std::unique_ptr<TFile> m_File;
  TH1D* m_Hist;

  double exposure_;  // 露光時間（秒）
  double m_TotalParticlesToGenerate;
  double m_NumParticlesGenerated;

  // 積分強度を格納するメンバー変数
  double m_integralParticleIntensity;
  double m_integralEnergyIntensity;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_AHRADIATIONBACKGROUNDPRIMARYGENSELECTEXPOSURETIME_HH */
