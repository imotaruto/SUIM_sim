#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TFile.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TStyle.h>
#include <iostream>

void plot_four_spectra() {
    // ROOTファイルのパスを指定
    const char* file_cxb      = "cxb_2detector.root";
    const char* file_proton   = "proton_2detector.root";
    const char* file_electron = "electron_2detector.root";
    const char* file_gamma    = "gamma_2detector.root";

    // 各ファイルからTGraphErrorsを読み込む
    TFile *f_cxb = TFile::Open(file_cxb, "READ");
    if (!f_cxb || f_cxb->IsZombie()) {
        printf("Error: ファイル %s を開けません。\n", file_cxb);
        return;
    }
    TGraphErrors *gr_cxb = (TGraphErrors*)f_cxb->Get("spectrum;1");
    if (!gr_cxb) {
        printf("Error: グラフ 'spectrum;1' が %s に存在しません。\n", file_cxb);
        return;
    }
    std::cout << "CXB Graph Points: " << gr_cxb->GetN() << std::endl;

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

    // 各グラフの最初の5点を確認（デバッグ用）
    for (int i = 0; i < gr_cxb->GetN() && i < 5; i++) {
        double x, y;
        gr_cxb->GetPoint(i, x, y);
        std::cout << "CXB Point " << i << ": (" << x << ", " << y << ")" << std::endl;
    }
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

    // 各データ点を共通のスケールで変換（必要に応じて調整）
    const double scale = 1.0;
    // CXBグラフの変換
    for (int i = 0; i < gr_cxb->GetN(); i++) {
        double x, y;
        gr_cxb->GetPoint(i, x, y);
        gr_cxb->SetPoint(i, x, y / scale);
        double ex = gr_cxb->GetErrorX(i);
        double ey = gr_cxb->GetErrorY(i);
        gr_cxb->SetPointError(i, ex, ey / scale);
    }
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
    // CXB：黒
    gr_cxb->SetMarkerStyle(23); // 六角形など、識別しやすいマーカー
    gr_cxb->SetMarkerColor(kBlack);
    gr_cxb->SetLineColor(kBlack);
    gr_cxb->SetMarkerSize(1.0);
    gr_cxb->SetTitle("Combined Spectra;Energy (keV);Counts/s/keV ");

    // プロトン：赤
    gr_proton->SetMarkerStyle(20);
    gr_proton->SetMarkerColor(kRed+1);
    gr_proton->SetLineColor(kRed+1);
    gr_proton->SetMarkerSize(1.0);

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

    // キャンバスの作成
    TCanvas *c1 = new TCanvas("c1", "Combined Spectra", 800, 600);
    c1->SetLogx(); // X軸を対数スケール
    c1->SetLogy(); // Y軸を対数スケール
    c1->SetGridx();
    c1->SetGridy();

    // 最初はCXBグラフを軸付きで描画し，その後同一キャンバスに追加
    gr_cxb->Draw("AP");
    gr_proton->Draw("P SAME");
    gr_electron->Draw("P SAME");
    gr_gamma->Draw("P SAME");

    // 軸範囲の設定（必要に応じて調整）
    c1->Update();
    gr_cxb->GetXaxis()->SetLimits(3, 15);
    gr_cxb->GetYaxis()->SetRangeUser(1e-5, 1.0e-1);

    // 凡例の作成
    TLegend *leg = new TLegend(0.65, 0.65, 0.9, 0.9);
    leg->AddEntry(gr_cxb,      "CXB",      "p");
    leg->AddEntry(gr_proton,   "Proton",   "p");
    leg->AddEntry(gr_electron, "Electron", "p");
    leg->AddEntry(gr_gamma,    "Gamma",    "p");
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();

    // キャンバスの装飾
    gStyle->SetOptTitle(1);
    gStyle->SetOptStat(0);

    // キャンバスの保存（PNG形式）
    c1->SaveAs("combined_spectra_loglog.png");

    // ROOTファイルへグラフを保存（オプション）
    TFile *f_out = new TFile("combined_spectra.root", "RECREATE");
    gr_cxb->Write("cxb");
    gr_proton->Write("proton");
    gr_electron->Write("electron");
    gr_gamma->Write("gamma");
    f_out->Close();

    // 使用したファイルのクローズ
    f_cxb->Close();
    f_proton->Close();
    f_electron->Close();
    f_gamma->Close();
}
