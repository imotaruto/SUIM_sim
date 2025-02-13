// plot_three_spectra.C

#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TFile.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TRandom.h>
#include <iostream> // デバッグ用

void plot_three_spectra() {
    // ROOTファイルのパスを指定
    const char* file_proton = "proton_detector.root";
    const char* file_electron = "electron_detector.root";
    const char* file_gamma = "gamma_detector.root";

    // 各ファイルからTGraphErrorsを読み込む
    TFile *f_proton = TFile::Open(file_proton, "READ");
    if (!f_proton || f_proton->IsZombie()) {
        printf("Error: ファイル %s を開けません。\n", file_proton);
        return;
    }
    TGraphErrors *gr_proton = (TGraphErrors*)f_proton->Get("spectrum;1");
    if (!gr_proton) {
        printf("Error: グラフ 'spectrum;1' が %s に存在しません。\n", file_proton);
        return;
    }
    std::cout << "Proton Graph Points: " << gr_proton->GetN() << std::endl;

    TFile *f_electron = TFile::Open(file_electron, "READ");
    if (!f_electron || f_electron->IsZombie()) {
        printf("Error: ファイル %s を開けません。\n", file_electron);
        return;
    }
    TGraphErrors *gr_electron = (TGraphErrors*)f_electron->Get("spectrum;1");
    if (!gr_electron) {
        printf("Error: グラフ 'spectrum;1' が %s に存在しません。\n", file_electron);
        return;
    }
    std::cout << "Electron Graph Points: " << gr_electron->GetN() << std::endl;

    TFile *f_gamma = TFile::Open(file_gamma, "READ");
    if (!f_gamma || f_gamma->IsZombie()) {
        printf("Error: ファイル %s を開けません。\n", file_gamma);
        return;
    }
    TGraphErrors *gr_gamma = (TGraphErrors*)f_gamma->Get("spectrum;1");
    if (!gr_gamma) {
        printf("Error: グラフ 'spectrum;1' が %s に存在しません。\n", file_gamma);
        return;
    }
    std::cout << "Gamma Graph Points: " << gr_gamma->GetN() << std::endl;

    // 各グラフのポイントを確認（最初の5ポイントのみ表示）
    for (int i = 0; i < gr_proton->GetN() && i < 5; i++) {
        double x, y;
        gr_proton->GetPoint(i, x, y);
        std::cout << "Proton Point " << i << ": (" << x << ", " << y << ")" << std::endl;
    }
    for (int i = 0; i < gr_electron->GetN() && i < 5; i++) {
        double x, y;
        gr_electron->GetPoint(i, x, y);
        std::cout << "Electron Point " << i << ": (" << x << ", " << y << ")" << std::endl;
    }
    for (int i = 0; i < gr_gamma->GetN() && i < 5; i++) {
        double x, y;
        gr_gamma->GetPoint(i, x, y);
        std::cout << "Gamma Point " << i << ": (" << x << ", " << y << ")" << std::endl;
    }

    // 【新規追加】各データ点を3763.8で割る処理
    const double scale = 15055.2;
    // プロトングラフの変換
    for (int i = 0; i < gr_proton->GetN(); i++) {
        double x, y;
        gr_proton->GetPoint(i, x, y);
        gr_proton->SetPoint(i, x, y / scale);
        double ex = gr_proton->GetErrorX(i);
        double ey = gr_proton->GetErrorY(i);
        gr_proton->SetPointError(i, ex, ey / scale);
    }
    // 電子グラフの変換
    for (int i = 0; i < gr_electron->GetN(); i++) {
        double x, y;
        gr_electron->GetPoint(i, x, y);
        gr_electron->SetPoint(i, x, y / scale);
        double ex = gr_electron->GetErrorX(i);
        double ey = gr_electron->GetErrorY(i);
        gr_electron->SetPointError(i, ex, ey / scale);
    }
    // ガンマグラフの変換
    for (int i = 0; i < gr_gamma->GetN(); i++) {
        double x, y;
        gr_gamma->GetPoint(i, x, y);
        gr_gamma->SetPoint(i, x, y / scale);
        double ex = gr_gamma->GetErrorX(i);
        double ey = gr_gamma->GetErrorY(i);
        gr_gamma->SetPointError(i, ex, ey / scale);
    }

    // グラフのスタイル設定
    // プロトン：赤
    gr_proton->SetMarkerStyle(20); // 点
    gr_proton->SetMarkerColor(kRed+1); // 明るめの赤
    gr_proton->SetLineColor(kRed+1);
    gr_proton->SetMarkerSize(1.0); // マーカーサイズを大きく
    gr_proton->SetTitle("Combined Spectra;Energy (keV);Counts/s/keV (scaled)");

    // 電子：青
    gr_electron->SetMarkerStyle(21); // 円
    gr_electron->SetMarkerColor(kBlue+1); // 明るめの青
    gr_electron->SetLineColor(kBlue+1);
    gr_electron->SetMarkerSize(1.0);

    // ガンマ：緑
    gr_gamma->SetMarkerStyle(22); // 三角形
    gr_gamma->SetMarkerColor(kGreen+2); // 明るめの緑
    gr_gamma->SetLineColor(kGreen+2);
    gr_gamma->SetMarkerSize(1.0);

    // キャンバスの作成
    TCanvas *c1 = new TCanvas("c1", "Combined Spectra", 800, 600);
    c1->SetLogx(); // X軸を対数スケール
    c1->SetLogy(); // Y軸を対数スケール

    // グリッドの追加
    c1->SetGridx();
    c1->SetGridy();

    // グラフの描画（ポイントのみ描画）
    gr_proton->Draw("AP"); // A: 軸を描画, P: ポイントを描画
    gr_electron->Draw("P SAME"); // SAME: 同じキャンバスに追加
    gr_gamma->Draw("P SAME");

    // 軸範囲の設定
    c1->Update(); // 範囲変更後に更新
    gr_proton->GetXaxis()->SetLimits(3, 15);
    // Y軸の範囲を 1e-8 ～ 3.8e-4 に設定
    gr_proton->GetYaxis()->SetRangeUser(1e-9, 1.0e-5);

    // 凡例の作成（ラインの表示を除外）
    TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9); // 位置は適宜調整
    leg->AddEntry(gr_proton, "Proton", "p");    // "p" はポイントのみを表示
    leg->AddEntry(gr_electron, "Electron", "p");
    leg->AddEntry(gr_gamma, "Gamma", "p");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();

    // グラフの装飾
    gStyle->SetOptTitle(1); // タイトルを表示
    gStyle->SetOptStat(0);  // 統計ボックスを非表示

    // キャンバスの保存（オプション）
    c1->SaveAs("combined_spectra_loglog.png");

    // ROOTファイルへの保存（オプション）
    TFile *f_out = new TFile("combined_spectra.root", "RECREATE");
    gr_proton->Write("proton");
    gr_electron->Write("electron");
    gr_gamma->Write("gamma");
    f_out->Close();

    // メモリの解放
    f_proton->Close();
    f_electron->Close();
    f_gamma->Close();
}
