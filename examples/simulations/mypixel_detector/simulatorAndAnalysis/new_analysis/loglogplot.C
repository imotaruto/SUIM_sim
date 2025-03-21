// loglogplot.C
#include "TFile.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TAxis.h"
#include <iostream>

void loglogplot() {
   // ROOT ファイルを読み込み
   TFile *file = TFile::Open("graph_albedogamma_graph.root", "READ");
   if (!file || file->IsZombie()) {
      std::cerr << "Error: ファイル graph_albedogamma_graph.root を開けません" << std::endl;
      return;
   }
   
   // TGraphErrors "myGraph" を取得
   TGraphErrors *graph = (TGraphErrors*) file->Get("myGraph");
   if (!graph) {
      std::cerr << "Error: myGraph が見つかりません" << std::endl;
      return;
   }
   
   // キャンバスの作成と両軸を対数スケールに設定
   TCanvas *c = new TCanvas("c", "Log-Log Plot", 800, 600);
   c->SetLogx();
   c->SetLogy();
   
   // グラフの描画設定（マーカーの種類・サイズなど）
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.0);
   graph->Draw("AP"); // 軸（A）とデータ点（P）を描画
   
   // 軸ラベルの設定（必要に応じて変更してください）
   graph->GetXaxis()->SetTitle("X axis");
   graph->GetYaxis()->SetTitle("Y axis");
   
   // キャンバスの更新
   c->Update();
   
   // 必要なら画像として保存
   c->SaveAs("loglog_plot.png");
}
