/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
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

#include "MyTextOutputModule.hh"

// BasicComptonEvent は ComptonSoft のイベントクラス
#include <BasicComptonEvent.hh>

// ParticleTable に依存していた部分は、Geant4 の G4ParticleTable を利用する
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include <iostream>
#include <vector>

namespace comptonsoft {

MyTextOutputModule::MyTextOutputModule(const std::string &name)
  : file_(nullptr)
{
  (void)name;  // 引数 'name' は未使用なので無視
}

MyTextOutputModule::~MyTextOutputModule() {
  if (file_) {
    file_->close();
    delete file_;
    file_ = nullptr;
  }
}

anlnext::ANLStatus MyTextOutputModule::mod_define() {
  // 必要なパラメータ定義などがあれば記述
  return anlnext::AS_OK;
}

anlnext::ANLStatus MyTextOutputModule::mod_begin_run() {
  file_ = new std::ofstream("my_output_hits.txt");
  if (file_ && file_->is_open()) {
    (*file_) << "# eventID hitIndex detectorID Edep(keV) posX(mm) posY(mm) posZ(mm) particleName\n";
  } else {
    std::cerr << "Error: Cannot open my_output_hits.txt for writing." << std::endl;
    return anlnext::AS_QUIT_ERROR;
  }
  return anlnext::AS_OK;
}

anlnext::ANLStatus MyTextOutputModule::mod_analyze() {
  // ここでは、従来の currentEventBuffer() 等でイベントを取得していた処理は現行では存在しないため、
  // イベント情報の出力処理を省略しています。
  // ※ 実際のイベント取得方法に合わせて、適切な実装に書き換えてください。
  return anlnext::AS_OK;
}

anlnext::ANLStatus MyTextOutputModule::mod_end_run() {
  if (file_) {
    file_->close();
    delete file_;
    file_ = nullptr;
  }
  return anlnext::AS_OK;
}

// 動的モジュールとして登録するためのエントリポイント
extern "C" {
  anlnext::BasicModule* create_module() {
    return new comptonsoft::MyTextOutputModule("MyTextOutputModule");
  }
}

} // namespace comptonsoft
