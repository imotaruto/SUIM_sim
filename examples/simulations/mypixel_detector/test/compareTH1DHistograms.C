#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <iostream>
#include <TStyle.h> // gStyleを使用するために追加

// 関数の定義
void compareTH1DHistograms(bool normalize, bool proportions) {
    // ファイル名とヒストグラムの名前を設定
    const char* file1Name = "check_fulltime_no_photoabs_event.root";
    const char* file2Name = "check_fulltime_no_scattaring_event.root";
    const char* histName1 = "hXrayEvtPH3_15_IniE"; // 実際のヒストグラム名に置き換えてください
    const char* histName2 = "hXrayEvtPH3_15_IniE"; // 実際のヒストグラム名に置き換えてください

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

    // TH1Dヒストグラムを取得
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

    // ヒストグラムのx軸が同じか確認
    bool sameBins = (hist1->GetNbinsX() == hist2->GetNbinsX()) &&
                    (hist1->GetXaxis()->GetXmin() == hist2->GetXaxis()->GetXmin()) &&
                    (hist1->GetXaxis()->GetXmax() == hist2->GetXaxis()->GetXmax());

    if (!sameBins) {
        std::cerr << "ヒストグラムのビン設定が異なります。比較を中止します。" << std::endl;
        file1->Close();
        file2->Close();
        return;
    }

    // ヒストグラムのスタイル設定
    hist1->SetLineColor(kRed);
    hist1->SetLineWidth(2);
    hist1->SetMarkerColor(kRed);
    hist1->SetMarkerStyle(20);
    hist1->SetStats(false); // 統計ボックスを非表示

    hist2->SetLineColor(kBlue);
    hist2->SetLineWidth(2);
    hist2->SetMarkerColor(kBlue);
    hist2->SetMarkerStyle(21);
    hist2->SetStats(false); // 統計ボックスを非表示

    // 軸タイトルの設定
    hist1->GetXaxis()->SetTitle("Initial Energy (keV)");
    if (normalize) {
        hist1->GetYaxis()->SetTitle("Normalized Counts");
    } else {
        hist1->GetYaxis()->SetTitle("Counts");
    }

    if (normalize) {
        // ヒストグラムのエラーバーの計算を有効にする
        hist1->Sumw2();
        hist2->Sumw2();

        // ヒストグラムを規格化（正規化）
        if (hist1->Integral() != 0) {
            hist1->Scale(1.0 / hist1->Integral());
        }
        if (hist2->Integral() != 0) {
            hist2->Scale(1.0 / hist2->Integral());
        }
    }

    // キャンバスを作成
    TString canvasName;
    TString canvasTitle;
    if (proportions) {
        canvasName = "c_proportions";
        canvasTitle = "Proportion Histogram Comparison";
    }
    else {
        canvasName = normalize ? "c_norm" : "c";
        canvasTitle = normalize ? "Normalized Histogram Comparison" : "Histogram Comparison";
    }
    TCanvas *c = new TCanvas(canvasName, canvasTitle, 800, 600);
    c->SetGrid();

    // ログスケールの設定
    if (proportions) {
        // 比率プロットの場合、y軸を対数スケールにするのは適切でない場合が多いため、ここではx軸のみ対数スケールに設定
        c->SetLogx();
        // 必要に応じてy軸をログスケールに設定する場合は以下を有効にしてください。ただし、0を含むとエラーになります。
        // c->SetLogy();
    } else {
        c->SetLogx();
        c->SetLogy();
    }

    if (proportions) {
        // 比率ヒストグラムの作成
        // hist1_prop: Compton Scatteringの割合
        // hist2_prop: Photo Absorptionの割合

        // hist1とhist2をクローン
        TH1D *hist1_prop = (TH1D*)hist1->Clone("hist1_prop");
        TH1D *hist2_prop = (TH1D*)hist2->Clone("hist2_prop");

        // 各ビンごとに割合を計算
        for (int bin = 1; bin <= hist1->GetNbinsX(); ++bin) {
            double count1 = hist1->GetBinContent(bin);
            double count2 = hist2->GetBinContent(bin);
            double total = count1 + count2;
            if (total > 0) {
                hist1_prop->SetBinContent(bin, (count1 / total) * 100.0); // パーセント表示
                hist1_prop->SetBinError(bin, 0.0); // エラーバーを無効化
                hist2_prop->SetBinContent(bin, (count2 / total) * 100.0);
                hist2_prop->SetBinError(bin, 0.0); // エラーバーを無効化
            } else {
                hist1_prop->SetBinContent(bin, 0);
                hist1_prop->SetBinError(bin, 0.0);
                hist2_prop->SetBinContent(bin, 0);
                hist2_prop->SetBinError(bin, 0.0);
            }
        }

        // スタイル設定
        hist1_prop->SetLineColor(kRed);
        hist1_prop->SetLineWidth(2);
        hist1_prop->SetMarkerColor(kRed);
        hist1_prop->SetMarkerStyle(20);
        hist1_prop->SetTitle("");
        hist1_prop->GetYaxis()->SetTitle("Percentage of Counts (%)");
        hist1_prop->GetYaxis()->SetRangeUser(0.1, 120); // 0を含まない最小値に設定（対数スケール用）

        hist2_prop->SetLineColor(kBlue);
        hist2_prop->SetLineWidth(2);
        hist2_prop->SetMarkerColor(kBlue);
        hist2_prop->SetMarkerStyle(21);
        hist2_prop->SetTitle("");

        // x軸の下限を10 keVに設定
        hist1_prop->GetXaxis()->SetRangeUser(10, hist1_prop->GetXaxis()->GetXmax());

        // 比率ヒストグラムを描画（エラーバーを表示しない）
        hist1_prop->Draw("P"); // "P" はポイントのみ表示
        hist2_prop->Draw("P SAME"); // "P SAME" はポイントのみ表示し、同じキャンバスに重ねて描画

        // 凡例を削除（以下のコードをコメントアウトまたは削除）
        /*
        TLegend *legend = new TLegend(0.30, 0.40, 0.70, 0.60);
        legend->AddEntry(hist1_prop, "Compton Scattering", "lep");
        legend->AddEntry(hist2_prop, "Photo Absorption", "lep");
        legend->Draw();
        */

        // キャンバスを保存
        c->SaveAs("hist_comparison_proportions.png");

        // メモリのクリーンアップ
        delete hist1_prop;
        delete hist2_prop;
        // delete legend; // 凡例を削除したため不要
    }
    else {
        // ヒストグラムを描画
        // y軸に0が含まれる場合、対数スケールが適用できないため、0を除外するか、小さな値に置き換える必要があります
        // ここでは、0のカウントを小さな値（例: 0.1）に置き換えます
        hist1->SetMinimum(0.1);
        hist2->SetMinimum(0.1);

        hist1->Draw("E1"); // エラーバー付きで描画
        hist2->Draw("E1 SAME");

        // x軸の下限を10 keVに設定
        hist1->GetXaxis()->SetRangeUser(10, hist1->GetXaxis()->GetXmax());

        // 凡例を削除（以下のコードをコメントアウトまたは削除）
        /*
        TLegend *legend = new TLegend(0.7, 0.7, 0.9, 0.9);
        if (normalize) {
            legend->AddEntry(hist1, "Compton Scattering (Normalized)", "lep");
            legend->AddEntry(hist2, "Photo Absorption (Normalized)", "lep");
        } else {
            legend->AddEntry(hist1, "Compton Scattering", "lep");
            legend->AddEntry(hist2, "Photo Absorption", "lep");
        }
        legend->Draw();
        */

        // キャンバスを保存
        TString saveName = normalize ? "hist_comparison_normalized.png" : "hist_comparison.png";
        c->SaveAs(saveName);

        // 規格化バージョンで差分ヒストグラムを作成する場合
        if (normalize) {
            // 差分ヒストグラムを作成
            TH1D *diffHist = (TH1D*)hist1->Clone("diffHist");
            diffHist->SetTitle("Difference: Compton - Photo Absorption");
            diffHist->SetLineColor(kGreen+2);
            diffHist->SetMarkerColor(kGreen+2);
            diffHist->SetMarkerStyle(22);
            diffHist->SetStats(false); // 統計ボックスを非表示
            diffHist->Add(hist2, -1);

            // x軸の下限を10 keVに設定
            diffHist->GetXaxis()->SetRangeUser(10, diffHist->GetXaxis()->GetXmax());

            // y軸に0が含まれる場合、対数スケールが適用できないため、最小値を設定
            diffHist->SetMinimum(0.1);

            // 新しいキャンバスを作成
            TCanvas *c_diff = new TCanvas("c_diff", "Difference Histogram", 800, 600);
            c_diff->SetGrid();
            c_diff->SetLogx();
            c_diff->SetLogy(); // 必要に応じてログスケールを設定
            diffHist->Draw("E1");

            // キャンバスを保存
            c_diff->SaveAs("hist_difference_normalized.png");

            // メモリのクリーンアップ
            delete diffHist;
            delete c_diff;
        }

        // 凡例を削除したため、以下の行をコメントアウトまたは削除
        // delete legend;
    }

    // 終了処理
    file1->Close();
    file2->Close();
    delete c;
}

// ラッパー関数の定義（引数なしで実行可能）
void runCompareTH1DHistograms() {
    // 各ビンごとの割合（パーセンテージ）の比較を行いたい場合
    // compareTH1DHistograms(false, true);

    // 絶対カウントの比較を行い、両軸を対数スケールに設定し、x軸を10から開始する場合
    compareTH1DHistograms(false, false);
}
