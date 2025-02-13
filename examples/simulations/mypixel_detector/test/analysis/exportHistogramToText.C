// exportHistogramToText.C
#include <TFile.h>
#include <TH1D.h>
#include <iostream>
#include <fstream>

// 関数の定義
void exportHistogramToText(const char* rootFileName, const char* histName, const char* outputFileName) {
    // ROOTファイルを開く
    TFile *file = TFile::Open(rootFileName, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "エラー: ファイル " << rootFileName << " を開けませんでした。" << std::endl;
        return;
    }
    
    // ヒストグラムを取得
    TH1D *hist = dynamic_cast<TH1D*>(file->Get(histName));
    if (!hist) {
        std::cerr << "エラー: ヒストグラム " << histName << " がファイル " << rootFileName << " に見つかりませんでした。" << std::endl;
        file->Close();
        return;
    }
    
    // 出力テキストファイルを開く
    std::ofstream outFile(outputFileName);
    if (!outFile.is_open()) {
        std::cerr << "エラー: 出力ファイル " << outputFileName << " を開けませんでした。" << std::endl;
        file->Close();
        return;
    }
    
    // ヘッダーの書き込み
    outFile << "# BinLowEdge\tBinHighEdge\tBinContent\tBinError" << std::endl;
    
    // 各ビンの情報を書き込む
    for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
        double low = hist->GetBinLowEdge(bin);
        double high = low + hist->GetBinWidth(bin);
        double content = hist->GetBinContent(bin);
        double error = hist->GetBinError(bin);
        outFile << low << "\t" << high << "\t" << content << "\t" << error << std::endl;
    }
    
    // ファイルを閉じる
    outFile.close();
    file->Close();
    
    std::cout << "ヒストグラム " << histName << " の情報を " << outputFileName << " に出力しました。" << std::endl;
}

// メイン関数
void runExportHistogram() {
    // パラメータの設定
    const char* rootFileName = "sum_scaring_NXB.root";
    const char* histName = "hXrayEventSpectrum_0_1keVbins";
    const char* outputFileName = "hXrayEventSpectrum_0_1keVbins.txt";
    
    // 関数の呼び出し
    exportHistogramToText(rootFileName, histName, outputFileName);
}

