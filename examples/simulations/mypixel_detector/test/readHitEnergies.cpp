// readHitEnergies.cpp

#include <TFile.h>
#include <TTree.h>
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
    // モジュールコードでは "EventTree" と定義されているため、正確な名前を使用します。
    TTree* tree = dynamic_cast<TTree*>(file->Get("eventtree"));
    if (!tree) {
        std::cerr << "Error: TTree 'EventTree' not found in the file." << std::endl;
        file->Close();
        return 1;
    }

    // ブランチ 'hitEnergies_keV' を取得
    TBranch* branch = tree->GetBranch("hitEnergies_keV");
    if (!branch) {
        std::cerr << "Error: Branch 'hitEnergies_keV' not found in TTree 'EventTree'." << std::endl;
        file->Close();
        return 1;
    }

    // hitEnergies_keV[9] を格納するdouble配列を定義
    const int array_size = 9;
    double hitEnergies_keV[array_size];

    // ブランチのアドレスを設定
    // ブランチがdouble型配列であることを確認
    tree->SetBranchAddress("hitEnergies_keV", hitEnergies_keV);

    // エントリ数を取得
    Long64_t nEntries = tree->GetEntries();
    std::cout << "Number of entries: " << nEntries << std::endl;

    // 各エントリごとにhitEnergies_keVの値を出力
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        std::cout << "Entry " << i << ": hitEnergies_keV = [ ";
        for (int j = 0; j < array_size; j++) {
            std::cout << hitEnergies_keV[j] << " ";
        }
        std::cout << "]" << std::endl;
    }

    // ファイルを閉じる
    file->Close();

    return 0;
}

