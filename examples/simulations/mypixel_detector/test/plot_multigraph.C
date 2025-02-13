// plot_multigraph.C
#include <TFile.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <iostream>
#include <vector>
#include <string> // std::stringを使用するために追加
#include <algorithm>

void plot_multigraph() {
    // ROOTファイルのリストを指定（凡例順を「proton」「electron」「gamma」に並べる）
    std::vector<std::string> root_files = {
        "mizuno_GCR_PS_proton.root",
        "mizuno_GCR_PS_electron.root",
        "GCR_gamma_10keVto1000GeV.root"
        // 必要に応じて他のファイルを追加
    };

    // TMultiGraphの作成
    TMultiGraph *mg = new TMultiGraph();

    // カラーパレットの設定
    int colors[] = {kRed, kBlue, kGreen+2, kMagenta, kCyan, kOrange, kViolet};
    int color_index = 0;

    // 凡例の作成（順序に注意して手動で追加）
    TLegend *legend = new TLegend(0.7,0.7,0.9,0.9);
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);

    // 各ROOTファイルからTGraphを取得してTMultiGraphに追加
    for(const auto &file_name : root_files) {
        TFile *file = TFile::Open(file_name.c_str(), "READ");
        if (!file || file->IsZombie()) {
            std::cerr << "Error: Cannot open file " << file_name << std::endl;
            continue;
        }

        TGraph *graph = dynamic_cast<TGraph*>(file->Get("Graph"));
        if (!graph) {
            std::cerr << "Error: Cannot find TGraph 'Graph' in file " << file_name << std::endl;
            file->Close();
            continue;
        }

        // グラフの色と線の太さを設定
        graph->SetLineColor(colors[color_index % (sizeof(colors)/sizeof(int))]);
        graph->SetLineWidth(2); // 線の太さを大きくする
        graph->SetMarkerColor(colors[color_index % (sizeof(colors)/sizeof(int))]);
        graph->SetMarkerSize(0.8); // マーカーのサイズを小さくする

        // マーカーを表示しない場合は以下のコメントを外す
        // graph->SetMarkerStyle(0);

        // TMultiGraphに追加
        mg->Add(graph, "L"); // "L"はラインのみの表示

        // 凡例に追加
        // ファイル名に基づいてラベルを設定
        std::string label;
        if (file_name.find("proton") != std::string::npos) {
            label = "Proton";
        }
        else if (file_name.find("electron") != std::string::npos) {
            label = "Electron";
        }
        else if (file_name.find("gamma") != std::string::npos) {
            label = "Gamma";
        }
        else {
            label = file_name; // 該当しない場合はファイル名を使用
        }
        legend->AddEntry(graph, label.c_str(), "l"); // "l"はラインのみの凡例エントリ

        color_index++;
        file->Close();
    }

    // キャンバスの作成
    TCanvas *c1 = new TCanvas("c1", "Energy vs Flux", 800, 600);
    c1->SetLogx();
    c1->SetLogy();

    // TMultiGraphの描画
    mg->SetTitle(" ;Energy (GeV);Flux (m^{-2} s^{-1} sr^{-1} GeV^{-1})");
    mg->Draw("A"); // "A"は軸を描画

    // 凡例の描画
    legend->Draw();

    // グラフの更新
    c1->Update();

    // 画像ファイルとして保存（例：PNG形式）
    c1->SaveAs("Energy_vs_Flux.png");
}
