// integrate.C
void integrate() {
    // ROOTファイルを開く
    TFile *file = TFile::Open("sum_scaring_NXB.root");
    if (!file || file->IsZombie()) {
        std::cerr << "ファイルのオープンに失敗しました。" << std::endl;
        return;
    }
    
    // ヒストグラムを取得
    TH1D *hEnergySpectrum = (TH1D*) file->Get("hEnergySpectrum");
    if (!hEnergySpectrum) {
        std::cerr << "ヒストグラム 'hEnergySpectrum' が見つかりません。" << std::endl;
        file->Close();
        return;
    }
    
    // エネルギー範囲を定義
    double low = 3.0;
    double high = 15.0;
    
    // ビン番号を取得
    int bin_low = hEnergySpectrum->FindBin(low);
    int bin_high = hEnergySpectrum->FindBin(high);
    
    // 積分を計算（ビン幅無視）
    double integral = hEnergySpectrum->Integral(bin_low, bin_high);
    
    // 積分を計算（ビン幅考慮）
    double integral_with_width = hEnergySpectrum->Integral(bin_low, bin_high, "width");
    
    // 結果を表示
    std::cout << "hEnergySpectrum の 3〜15 keV の積分値: " << integral << std::endl;
    std::cout << "hEnergySpectrum の 3〜15 keV の積分値（ビン幅考慮）: " << integral_with_width << std::endl;
    
    // ファイルを閉じる
    file->Close();
}

