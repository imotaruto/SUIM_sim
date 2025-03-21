#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TFile.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TStyle.h>
#include <iostream>

void plot_non_cxb_spectra() {
    // 各ROOTファイルのパスを指定
    const char* file_proton   = "proton_normalize.root";
    const char* file_electron = "electron_normalize.root";
    const char* file_gamma    = "gamma_normalize.root";

    // プロトンスペクトルの読み込み
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

    // 電子スペクトルの読み込み
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

    // ガンマスペクトルの読み込み
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

    // デバッグ用に各グラフの最初の5点を表示
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

    // 各データ点を共通のスケール（scale=1.0）で変換（必要に応じて調整）
    const double scale = 1.0;
    for (int i = 0; i < gr_proton->GetN(); i++) {
        double x, y;
        gr_proton->GetPoint(i, x, y);
        gr_proton->SetPoint(i, x, y / scale);
        double ex = gr_proton->GetErrorX(i);
        double ey = gr_proton->GetErrorY(i);
        gr_proton->SetPointError(i, ex, ey / scale);
    }
    for (int i = 0; i < gr_electron->GetN(); i++) {
        double x, y;
        gr_electron->GetPoint(i, x, y);
        gr_electron->SetPoint(i, x, y / scale);
        double ex = gr_electron->GetErrorX(i);
        double ey = gr_electron->GetErrorY(i);
        gr_electron->SetPointError(i, ex, ey / scale);
    }
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
    gr_proton->SetMarkerStyle(20);
    gr_proton->SetMarkerColor(kRed+1);
    gr_proton->SetLineColor(kRed+1);
    gr_proton->SetMarkerSize(1.0);
    gr_proton->SetTitle("Non-CXB Spectra;Energy (keV);Counts/s/keV ");

    // 電子：青
    gr_electron->SetMarkerStyle(21);
    gr_electron->SetMarkerColor(kBlue+1);
    gr_electron->SetLineColor(kBlue+1);
    gr_electron->SetMarkerSize(1.0);

    // ガンマ：緑
    gr_gamma->SetMarkerStyle(22);
    gr_gamma->SetMarkerColor(kGreen+2);
    gr_gamma->SetLineColor(kGreen+2);
    gr_gamma->SetMarkerSize(1.0);

    // キャンバスの作成と描画設定
    TCanvas *c1 = new TCanvas("c1", "Non-CXB Spectra", 800, 600);
    c1->SetLogx();
    c1->SetLogy();
//    c1->SetGridx();
//    c1->SetGridy();

    // 最初にプロトングラフを描画し，その後同一キャンバスに追加
    gr_proton->Draw("AP");
    gr_electron->Draw("P SAME");
    gr_gamma->Draw("P SAME");

    // 軸範囲の設定（必要に応じて調整）
    c1->Update();
    gr_proton->GetXaxis()->SetLimits(3, 15);
    gr_proton->GetYaxis()->SetRangeUser(1e-9, 1e-6);

    // 凡例の作成
    TLegend *leg = new TLegend(0.65, 0.65, 0.9, 0.9);
    leg->AddEntry(gr_proton,   "Proton",   "p");
    leg->AddEntry(gr_electron, "Electron", "p");
    leg->AddEntry(gr_gamma,    "Gamma",    "p");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();

    // キャンバスの装飾設定
    gStyle->SetOptTitle(1);
    gStyle->SetOptStat(0);

    // キャンバスの保存（PNG形式）
    c1->SaveAs("non_cxb_spectra_loglog.png");

    // ROOTファイルへグラフを保存（オプション）
    TFile *f_out = new TFile("non_cxb_spectra.root", "RECREATE");
    gr_proton->Write("proton");
    gr_electron->Write("electron");
    gr_gamma->Write("gamma");
    f_out->Close();

    // 使用したファイルをクローズ
    f_proton->Close();
    f_electron->Close();
    f_gamma->Close();
}

