// draw_log_scale.C

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

// ROOT ヘッダーファイル
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>

void draw_log_scale() {
    // 出力ファイル名を指定
    const char *filename = "simulation_out_Efeild_fullpix_sield_20mm_albedo_photon_event.root";

    // ROOT ファイルを開く
    TFile *file = TFile::Open(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "ファイルのオープンに失敗しました: " << filename << std::endl;
        return;
    }

    // ファイル内のオブジェクトを一覧表示
    file->ls();

    // 描画したいヒストグラムのリスト
    std::vector<std::string> hist_names = {
        "hEnergySpectrum",
        "hXrayEventSpectrum",
        "hNonXrayEventSpectrum",
        "hXrayEventSpectrum_0_1keVbins",
        "hXrayEventSpectrum_0_25keVbins",
        "hXrayEventSpectrum_0_5keVbins",
        "hXrayEventSpectrum_1keVbins",
        "hPattern_10",
        "hPattern_11",
        "hPattern_20",
        "hPattern_21",
        "hPattern_30",
        "hPattern_31",
        "hPattern_40",
        "hPattern_41",
        "hPattern_42",
        "hPattern_50",
        "hPattern_51",
        "hPattern_52",
        "hPattern_60",
        "hPattern_70",
        "hPattern_90",
        "hSplitCount_0",
        "hSplitCount_1",
        "hSplitCount_2",
        "hSplitCount_3",
        "hSplitCount_4",
        "hPatternDistribution",
        "hXrayPatternDistribution"
    };

    // 各ヒストグラムを取得して描画
    for (const auto &name : hist_names) {
        TH1D *hist = (TH1D*)file->Get(name.c_str());
        if (!hist) {
            std::cerr << "ヒストグラムが見つかりません: " << name << std::endl;
            continue;
        }

        // ゼロ以下のビンが存在する場合、対数スケールでの描画ができないため警告
        bool has_zero = false;
        for (int i = 1; i <= hist->GetNbinsX(); ++i) {
            if (hist->GetBinContent(i) <= 0) {
                has_zero = true;
                break;
            }
        }
        if (has_zero) {
            std::cerr << "ヒストグラム " << name << " にゼロ以下のビンが含まれています。ログスケールでの描画には適していません。" << std::endl;
            // 必要に応じてビン内容を調整するコードをここに追加できます
            // 例: Y軸の最小値を1に設定
            hist->SetMinimum(1);
        }

        // キャンバスを作成
        std::string canvas_name = "c_" + name;
        std::string canvas_title = "Canvas for " + name;
        TCanvas *c = new TCanvas(canvas_name.c_str(), canvas_title.c_str(), 800, 600);

        // キャンバスの軸をログスケールに設定
        c->SetLogx(); // X軸を対数スケールに
        c->SetLogy(); // Y軸を対数スケールに

        // ヒストグラムを描画
        hist->Draw();

        // タイトルと軸ラベルの設定
        std::string new_title = std::string(hist->GetTitle()) + ";Energy (keV);Counts / s";
        hist->SetTitle(new_title.c_str());

        // キャンバスを画像として保存（PNG形式）
        std::string img_name = name + "_log.png";
        c->SaveAs(img_name.c_str());

        // キャンバスを閉じる（メモリ解放）
        delete c;
    }

    // ファイルを閉じる
    file->Close();
}
