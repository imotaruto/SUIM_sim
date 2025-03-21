// export_hist.C
#include <TFile.h>
#include <TH1D.h>
#include <iostream>
#include <fstream>

void export_hist(const char* root_filename, const char* hist_name, const char* output_txt) {
    // ROOTファイルを開く
    TFile *file = TFile::Open(root_filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Cannot open file " << root_filename << std::endl;
        return;
    }

    // ヒストグラムを取得
    TH1D *hist = dynamic_cast<TH1D*>(file->Get(hist_name));
    if (!hist) {
        std::cerr << "Error: Cannot find histogram " << hist_name << " in file " << root_filename << std::endl;
        file->Close();
        return;
    }

    // 出力用テキストファイルを開く
    std::ofstream outfile(output_txt);
    if (!outfile.is_open()) {
        std::cerr << "Error: Cannot open output file " << output_txt << std::endl;
        file->Close();
        return;
    }

    // ヒストグラムの各ビンをループして書き出す
    outfile << "# Bin\tCenter\tContent\tError" << std::endl;
    for (int i = 1; i <= hist->GetNbinsX(); ++i) {
        double center = hist->GetBinCenter(i);
        double content = hist->GetBinContent(i);
        double error = hist->GetBinError(i);
        outfile << i << "\t" << center << "\t" << content << "\t" << error << std::endl;
    }

    // ファイルを閉じる
    outfile.close();
    file->Close();

    std::cout << "Histogram " << hist_name << " has been exported to " << output_txt << std::endl;
}

