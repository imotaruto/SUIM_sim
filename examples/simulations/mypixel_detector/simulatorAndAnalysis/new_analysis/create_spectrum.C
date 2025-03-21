// create_spectrum.C
void create_spectrum() {
    // データファイルのパスを指定
    const char* filename = "gamma_normalize.txt";

    // TGraphErrorsを作成
    TGraphErrors *gr = new TGraphErrors(filename, "%lf %lf %lf");
    gr->SetTitle("Spectrum;Energy (keV);Counts");
    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(0.8);
    gr->SetMarkerColor(kBlue);
    gr->SetLineColor(kBlue);

    // キャンバスの作成
    TCanvas *c1 = new TCanvas("c1", "Spectrum", 800, 600);
    gr->Draw("AP"); // A: 軸を描画, P: ポイントを描画

    // グラフを保存（オプション）
    c1->SaveAs("spectrum.png");

    // ROOTファイルに保存（オプション）
    TFile *f = new TFile("gamma_normalize.root", "RECREATE");
    gr->Write("spectrum");
    f->Close();
}

