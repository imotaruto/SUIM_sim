#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TKey.h>
#include <TIterator.h>
#include <TLeaf.h>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // コマンドライン引数のチェック
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_root_file>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];

    // ROOTファイルを開く
    TFile* file = TFile::Open(input_file.c_str(), "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Unable to open file " << input_file << std::endl;
        return 1;
    }

    // ツリーを取得
    TTree* tree = dynamic_cast<TTree*>(file->Get("eventtree"));
    if (!tree) {
        std::cerr << "Error: TTree 'eventtree' not found in the file." << std::endl;
        file->Close();
        return 1;
    }

    // ブランチ 'pixelX' が配列であることを確認して読み込む
    TBranch* branch = tree->GetBranch("centerHitEnergy_keV");
    if (!branch) {
        std::cerr << "Error: Branch 'pixelX' not found in TTree 'eventtree'." << std::endl;
        file->Close();
        return 1;
    }

    // pixelX[9] を格納するint配列を定義
    const int array_size = 9;
    int pixelX[array_size];

    // ブランチのアドレスを設定
    tree->SetBranchAddress("hitEnergies_keV", pixelX);

    // エントリ数を取得
    Long64_t nEntries = tree->GetEntries();
    std::cout << "Number of entries: " << nEntries << std::endl;

    // 各エントリごとにpixelXの値を出力
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        std::cout << "Entry " << i << ": pixelX = [ ";
        for (int j = 0; j < array_size; j++) {
            std::cout << pixelX[j] << " ";
        }
        std::cout << "]" << std::endl;
    }

    // ファイルを閉じる
    file->Close();

    return 0;
}
