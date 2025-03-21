void testOneFile() {
   // ファイルを開く
   TFile *f = TFile::Open("simulation_out_Efeild_fullpix_sield_20mm_mizuno_proton_event.root", "READ");
   if (!f || f->IsZombie()) {
      std::cout << "Error: ファイルが開けません" << std::endl;
      return;
   }
   // ヒストグラムの取得（キー名が正しいか確認）
   TH1D *h = (TH1D*) f->Get("hXrayEventSpectrum_0_1keVbins;1");
   if (!h) {
      std::cout << "Error: ヒストグラムが見つかりません" << std::endl;
      return;
   }
   // キャンバス作成
   TCanvas *c = new TCanvas("c", "Test One File", 800, 600);
   h->Draw();
   c->Update();
}

