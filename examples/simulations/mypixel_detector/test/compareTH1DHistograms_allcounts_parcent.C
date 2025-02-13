#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <iostream>
#include <TStyle.h> // gStyleを使用するために追加

void compareTH1DHistograms_percentage() {
    // ファイル名とヒストグラフの名前を設定
    const char* file1Name = "check_fulltime_no_photoabs_event.root";
    const char* file2Name = "check_fulltime_no_scattaring_event.root";
    const char* histName1 = "hXrayEvtPH3_15_IniE";
    const char* histName2 = "hXrayEvtPH3_15_IniE";

    // 統計ボックスを非表示に設定
    gStyle->SetOptStat(0);

    // ROOTファイルを開く
    TFile *file1 = TFile::Open(file1Name, "READ");
    TFile *file2 = TFile::Open(file2Name, "READ");

    if (!file1 || file1->IsZombie()) {
        std::cerr << "ファイル " << file1Name << " を開けませんでした。" << std::endl;
        return;
    }
    if (!file2 || file2->IsZombie()) {
        std::cerr << "ファイル " << file2Name << " を開けませんでした。" << std::endl;
        file1->Close();
        return;
    }

    // ヒストグラムを取得
    TH1D *hist1 = dynamic_cast<TH1D*>(file1->Get(histName1));
    TH1D *hist2 = dynamic_cast<TH1D*>(file2->Get(histName2));

    if (!hist1) {
        std::cerr << "ヒストグラム " << histName1 << " が " << file1Name << " に見つかりませんでした。" << std::endl;
        file1->Close();
        file2->Close();
        return;
    }
    if (!hist2) {
        std::cerr << "ヒストグラム " << histName2 << " が " << file2Name << " に見つかりませんでした。" << std::endl;
        file1->Close();
        file2->Close();
        return;
    }

    // ヒストグラムのビン設定が同じか確認（オプション）
    bool sameBins = (hist1->GetNbinsX() == hist2->GetNbinsX()) &&
                    (hist1->GetXaxis()->GetXmin() == hist2->GetXaxis()->GetXmin()) &&
                    (hist1->GetXaxis()->GetXmax() == hist2->GetXaxis()->GetXmax());
    if (!sameBins) {
        std::cerr << "ヒストグラムのビン設定が異なるため比較を中止します。" << std::endl;
        file1->Close();
        file2->Close();
        return;
    }

    // スタイル設定
    hist1->SetLineColor(kRed);
    hist1->SetLineWidth(2);
    hist1->SetMarkerColor(kRed);
    hist1->SetMarkerStyle(20);
    hist1->SetStats(false);

    hist2->SetLineColor(kBlue);
    hist2->SetLineWidth(2);
    hist2->SetMarkerColor(kBlue);
    hist2->SetMarkerStyle(21);
    hist2->SetStats(false);

    // エラーバーを正しく計算したい場合（必須ではないが推奨）
    hist1->Sumw2();
    hist2->Sumw2();

    // 2つのファイルのヒストグラム合計値を計算
    double integral1 = hist1->Integral();
    double integral2 = hist2->Integral();
    double total_integral = integral1 + integral2;

    // 全体(2ファイル合計)を分母にして"％"表示に変換
    // つまり hist1 + hist2 = 100 (%) となるようにスケールする
    if (total_integral > 0) {
        hist1->Scale(100.0 / total_integral);
        hist2->Scale(100.0 / total_integral);
    }

    // キャンバス作成
    TCanvas *c = new TCanvas("c_percentage", "Percentage Comparison", 800, 600);
    c->SetGrid();

    // 両対数スケール
    c->SetLogx();
    c->SetLogy();

    // X 軸を 10 keV から表示
    hist1->GetXaxis()->SetRangeUser(10, hist1->GetXaxis()->GetXmax());

    // Y 軸を 10^-3 ~ 10^2 に設定
    hist1->GetYaxis()->SetRangeUser(1e-3, 1e+1);

    // Y軸ラベル
    hist1->GetYaxis()->SetTitle("Percentage of Total Events (%)");
    // X軸ラベルを設定するなら（例: 初期エネルギー）
    hist1->GetXaxis()->SetTitle("Initial Energy (keV)");

    // ヒストグラムを描画
    hist1->Draw("E1");
    hist2->Draw("E1 SAME");

    // 凡例を追加（必要に応じて）
    /*
    auto legend = new TLegend(0.60, 0.70, 0.88, 0.88);
    legend->AddEntry(hist1, "no_photoabs", "lep");
    legend->AddEntry(hist2, "no_scattering", "lep");
    legend->Draw();
    */

    // 画像を保存
    c->SaveAs("hist_comparison_percentage.png");

    // 後処理
    file1->Close();
    file2->Close();
    delete c;
}
