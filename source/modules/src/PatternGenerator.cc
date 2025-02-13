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

#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

int main(int argc, char** argv) {
    // コマンドライン引数のチェック
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_root_file> <output_root_file>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    // スレッショルド値（keV）
    double threshold = 0.5; // 必要に応じて変更可能

    // 入力ROOTファイルを開く
    TFile* inFile = TFile::Open(input_file.c_str(), "READ");
    if (!inFile || inFile->IsZombie()) {
        std::cerr << "Error: Unable to open input file " << input_file << std::endl;
        return 1;
    }

    // TTreeを取得（モジュールコードに合わせて名前を変更）
    // 例: "EventTree" または "PatternTree"
    std::string input_tree_name = "EventTree"; // 必要に応じて変更
    TTree* inTree = dynamic_cast<TTree*>(inFile->Get(input_tree_name.c_str()));
    if (!inTree) {
        std::cerr << "Error: TTree '" << input_tree_name << "' not found in the file." << std::endl;
        inFile->Close();
        return 1;
    }

    // 必要なブランチを取得
    // 例では "hitEnergies_keV" と "EVENT_ID" を使用
    TBranch* branch_hitEnergies = inTree->GetBranch("hitEnergies_keV");
    if (!branch_hitEnergies) {
        std::cerr << "Error: Branch 'hitEnergies_keV' not found in TTree '" << input_tree_name << "'." << std::endl;
        inFile->Close();
        return 1;
    }

    TBranch* branch_EVENT_ID = inTree->GetBranch("EVENT_ID");
    if (!branch_EVENT_ID) {
        std::cerr << "Error: Branch 'EVENT_ID' not found in TTree '" << input_tree_name << "'." << std::endl;
        inFile->Close();
        return 1;
    }

    // ブランチのアドレスを設定
    const int array_size = 9;
    double hitEnergies_keV[array_size] = {0.0};
    int64_t EVENT_ID = 0;

    inTree->SetBranchAddress("hitEnergies_keV", hitEnergies_keV);
    inTree->SetBranchAddress("EVENT_ID", &EVENT_ID);

    // 出力ROOTファイルを作成
    TFile* outFile = TFile::Open(output_file.c_str(), "RECREATE");
    if (!outFile || outFile->IsZombie()) {
        std::cerr << "Error: Unable to create output file " << output_file << std::endl;
        inFile->Close();
        return 1;
    }

    // 新しいTTreeを作成
    TTree* outTree = new TTree("PatternTree", "Generated Patterns and EVENT_IDs");

    // 出力ブランチ用の変数
    std::string pattern;
    int64_t out_EVENT_ID;

    // ブランチを設定
    outTree->Branch("pattern", &pattern);
    outTree->Branch("EVENT_ID", &out_EVENT_ID);

    // エントリ数を取得
    Long64_t nEntries = inTree->GetEntries();
    std::cout << "Number of entries to process: " << nEntries << std::endl;

    // 各エントリを処理
    for (Long64_t i = 0; i < nEntries; ++i) {
        inTree->GetEntry(i);

        // hitEnergies_keVの最初の8要素に対してスレッショルドを適用
        // 8桁のバイナリパターンを生成
        pattern = "";
        for (int j = 0; j < 8; ++j) {
            if (j < array_size) {
                if (hitEnergies_keV[j] > threshold) {
                    pattern += "1";
                } else {
                    pattern += "0";
                }
            } else {
                // データが不足している場合は0とする
                pattern += "0";
            }
        }

        // EVENT_IDを設定
        out_EVENT_ID = EVENT_ID;

        // TTreeにエントリを追加
        outTree->Fill();

        // デバッグ出力（必要に応じてコメントアウト）
        /*
        std::cout << "Entry " << i << ": EVENT_ID = " << EVENT_ID
                  << ", pattern = " << pattern << std::endl;
        */
    }

    // 出力TTreeをファイルに書き込む
    outFile->cd();
    outTree->Write();
    std::cout << "PatternTree has been written to " << output_file << std::endl;

    // ファイルを閉じる
    outFile->Close();
    inFile->Close();

    return 0;
}
