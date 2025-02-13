/*************************************************************************
 *                                                                       *
 * Copyright (c) 2024 Your Name                                           *
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

#ifndef COMPTONSOFT_PatternGenerator_H
#define COMPTONSOFT_PatternGenerator_H 1

#include "VCSModule.hh"
#include "CSHitCollection.hh"
#include "anlgeant4/InitialInformation.hh"  // 正しいパスを指定
#include "DetectorHit.hh"
#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>

namespace comptonsoft
{

/**
 * @class PatternGenerator
 * @brief PatternGenerator モジュール
 *
 * このモジュールは指定されたブランチを抽出し、hitEnergies_keV 配列をスレッショルドと比較してパターンを生成します。
 * 生成されたパターンは新しいブランチ "pattern" と "EVENT_ID" としてROOTファイルに保存されます。
 */
class PatternGenerator : public VCSModule
{
  DEFINE_ANL_MODULE(PatternGenerator, 1.0);
public:
  PatternGenerator();
  ~PatternGenerator();

  // モジュールの定義
  anlnext::ANLStatus mod_define() override;

  // モジュールの初期化
  anlnext::ANLStatus mod_initialize() override;

  // ラン開始時の処理
  anlnext::ANLStatus mod_begin_run() override;

  // データ解析時の処理
  anlnext::ANLStatus mod_analyze() override;

  // ラン終了時の処理
  anlnext::ANLStatus mod_end_run() override;

private:
  // スレッショルド値 (デフォルト値を設定)
  double sprit_threshold_ = 0.5;  // keV

  // 出力ファイル名
  std::string output_filename_ = "pattern_output.root";

  // 入力データへのポインタ
  anlgeant4::InitialInformation* m_InitialInfo = nullptr;  // 名前空間を指定
  CSHitCollection* m_HitCollection = nullptr;

  // 出力ブランチ用の変数
  std::vector<std::string> patterns_;

  // イベントIDを格納するためのベクター
  std::vector<int64_t> event_ids_;

  // ROOTファイルとTTreeのハンドラ
  TFile* outFile_ = nullptr;
  TTree* outTree_ = nullptr;

  // TTreeのブランチ用変数
  std::vector<std::string> tree_patterns_;
  std::vector<int64_t> tree_event_ids_;
};

} /* namespace comptonsoft */

#endif /* COMPTONSOFT_PatternGenerator_H */
