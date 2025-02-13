// print_real_time.cpp
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TKey.h>
#include <TIterator.h>
#include <TLeaf.h>        // 追加
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

    // ブランチの型を確認
    TBranch* branch = tree->GetBranch("frame_exposure_time");
    if (!branch) {
        std::cerr << "Error: Branch 'real_time' not found in TTree 'eventtree'." << std::endl;
        file->Close();
        return 1;
    }

    // ブランチの型を取得
    TLeaf* leaf = branch->GetLeaf("frame_exposure_time");
    if (!leaf) {
        std::cerr << "Error: Leaf 'real_time' not found in branch 'real_time'." << std::endl;
        file->Close();
        return 1;
    }

    std::string branch_type = leaf->GetTypeName();
    std::cout << "Branch 'real_time' type: " << branch_type << std::endl;

    // ブランチの型に応じて読み取り
    if (branch_type == "Double_t") {
        double real_time;
        tree->SetBranchAddress("frame_exposure_time", &real_time);
        Long64_t nEntries = tree->GetEntries();
        std::cout << "Number of entries: " << nEntries << std::endl;
        for (Long64_t i = 0; i < nEntries; i++) {
            tree->GetEntry(i);
            std::cout << "Entry " << i << ": frame_exposure_time = " << real_time << " s" << std::endl;
        }
    }
    else if (branch_type == "Float_t") {
        float real_time;
        tree->SetBranchAddress("frame_exposure_time", &real_time);
        Long64_t nEntries = tree->GetEntries();
        std::cout << "Number of entries: " << nEntries << std::endl;
        for (Long64_t i = 0; i < nEntries; i++) {
            tree->GetEntry(i);
            std::cout << "Entry " << i << ": frame_exposure_time = " << real_time << " s" << std::endl;
        }
    }
    else if (branch_type == "Int_t") {
        int real_time;
        tree->SetBranchAddress("frame_exposure_time", &real_time);
        Long64_t nEntries = tree->GetEntries();
        std::cout << "Number of entries: " << nEntries << std::endl;
        for (Long64_t i = 0; i < nEntries; i++) {
            tree->GetEntry(i);
            std::cout << "Entry " << i << ": frame_exposure_time = " << real_time << " s" << std::endl;
        }
    }
    else {
        std::cerr << "Error: Unsupported branch type '" << branch_type << "'." << std::endl;
        file->Close();
        return 1;
    }

    // ファイルを閉じる
    file->Close();

    return 0;
}
