void draw_hist_custom() {
    // ROOTファイルを開く
    TFile *file = TFile::Open("simulation_out_Efeild_fullpix_sield_20mm_newmodel_GCR_gamma_10keVto1000GeV_event.root");
    if (!file || file->IsZombie()) {
        std::cerr << "ファイルを開けません: data.root" << std::endl;
        return;
    }

    // ヒストグラムを取得
    TH2D *h = (TH2D*)file->Get("hIniEnergyVsPH");
    if (!h) {
        std::cerr << "ヒストグラムが見つかりません: hIniEnergyVsPH" << std::endl;
        return;
    }

    // カラーパレットの設定
    gStyle->SetPalette(kBird);

    // キャンバスの作成
    TCanvas *c = new TCanvas("c", "Initial Energy vs PH", 800, 600);

    // 表示範囲の設定
    h->GetXaxis()->SetRangeUser(0, 100);
    h->GetYaxis()->SetRangeUser(0, 100);

    // タイトルと軸ラベルの設定
    h->SetTitle("Initial Energy vs PH;Initial Energy [units];PH [units]");

    // グリッドの設定
    c->SetGridx();
    c->SetGridy();

    // ヒストグラムの描画
    h->Draw("COLZ");

    // ビンをループしてマーカーを描画
    for (Int_t i = 1; i <= h->GetNbinsX(); ++i) {
        for (Int_t j = 1; j <= h->GetNbinsY(); ++j) {
            Double_t z = h->GetBinContent(i, j);
            if (z > 0) { // 非ゼロのビンのみ
                Double_t x = h->GetXaxis()->GetBinCenter(i);
                Double_t y = h->GetYaxis()->GetBinCenter(j);
                TMarker *marker = new TMarker(x, y, 20); // マーカーのスタイルを選択
                marker->SetMarkerSize(1.5); // マーカーサイズを大きくする
                marker->SetMarkerColor(kRed); // マーカー色を設定
                marker->Draw("SAME");
            }
        }
    }

    // 画像として保存
    c->SaveAs("hIniEnergyVsPH_custom.png");

    // ファイルを閉じる
    file->Close();
}

