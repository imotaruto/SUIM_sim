#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>

int main() {
    // ROOTファイルを開く
    TFile *inputFile = TFile::Open("simulation_out_Efeild.root");

    // ファイルが正常に開かれたかどうかを確認
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "ファイルのオープンに失敗しました。" << std::endl;
        return 1; // エラーステータスで終了
    }

    // hittreeにアクセス
    TTree *tree = (TTree*)inputFile->Get("hittree");

    // hittreeが存在するか確認
    if (!tree) {
        std::cerr << "'hittree'の取得に失敗しました。" << std::endl;
        inputFile->Close();
        return 1; // エラーステータスで終了
    }

    // "epi"ブランチが存在するか確認
    if (!tree->GetBranch("epi")) {
        std::cerr << "'hittree'に'epi'ブランチが存在しません。" << std::endl;
        inputFile->Close();
        return 1; // エラーステータスで終了
    }

    // 描画用のキャンバスを作成
    TCanvas *c1 = new TCanvas("c1", "EPI プロット", 800, 600);

    // epiのヒストグラムを描画 (横軸0から20まで、ビン数100)
    tree->Draw("epi>>h1(100, 0, 20)");

    // ヒストグラムを取得
    TH1F *h1 = (TH1F*)gDirectory->Get("h1");

    // ヒストグラムが作成されたか確認
    if (!h1) {
        std::cerr << "'epi'のヒストグラム作成に失敗しました。" << std::endl;
        inputFile->Close();
        return 1; // エラーステータスで終了
    }

    // 縦軸をログスケールに設定
    c1->SetLogy();

    // ヒストグラムを描画
    h1->Draw();

    // キャンバスを更新してプロットを表示
    c1->Update();

    // 画像ファイルに保存
    c1->SaveAs("epi_plot.png"); // PNG形式で保存
    // 他の形式もサポートしています: "epi_plot.pdf", "epi_plot.jpg", "epi_plot.root" など

    // ファイルを閉じる
    inputFile->Close();
    delete inputFile;

    return 0; // 正常終了
}

