#include "Write3x3EventTree.hh"
#include "AstroUnits.hh"
#include "DetectorHit.hh"
#include "InitialInformation.hh"

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <bitset>
#include <cstdint>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraph.h>
#include <TROOT.h>

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

// ----------------------------------------------------------------------------
// コンストラクタ
// ----------------------------------------------------------------------------
Write3x3EventTree::Write3x3EventTree()
    : hitCollection_(nullptr),
      initialInfo_(nullptr),
      inputFile_(nullptr),
      inputTree_(nullptr),
      outputFile_(nullptr),
      outputTree_(nullptr),

      // 3x3ヒット情報
      centerHitEnergy_(0.0),
      maxEnergy_(0.0),
      hitEnergies_{0.0},
      pixelX_{-1},
      pixelY_{-1},
      Center_Hit_pixelX_(0),
      Center_Hit_pixelY_(0),

      // 時間情報
      real_exposuretime_(0.0),
      Select_Event_ID_(-1),
      select_Event_real_time_(0.0),
      real_time_(0.0),
      frame_exposure_time_(0.0),
      delay_time_(0.0),

      // ファイル名
      input_filename_("input.root"),
      output_filename_("output.root"),
      tree_name_("EventTree"),

      // event ID
      EVENT_ID_(0),
      event_time_(0.0),
      Real_time_(0.0),

      // スレッショルド
      sprit_threshold_(1.0),
      event_threshold_(1.0),

      // pattern関連
      pattern_(0),
      split_cnt_(0),
      pattern_type_(0),
      pixel_pattern_(0),
      vortex_(0),

      // exposure
      exposure_time_(0.0),

      // ヒストグラム(既存)
      hEnergySpectrum(nullptr),
      hXrayEventSpectrum(nullptr),
      hNonXrayEventSpectrum(nullptr),
      hXrayEventSpectrum_0_1keVbins(nullptr),
      hXrayEventSpectrum_0_25keVbins(nullptr),
      hXrayEventSpectrum_0_5keVbins(nullptr),
      hXrayEventSpectrum_1keVbins(nullptr),
      hPatternDistribution(nullptr),
      hXrayPatternDistribution(nullptr),

      // エネルギー相関
      enable_energy_correlation_(true),
      ini_Energy_(0.0f),
      hIniEnergyVsPH_(nullptr),
      hXrayCountVsIniEnergy_(nullptr),

      // initialInfo_ の eventid / 初期エネルギー
      eventIDFromInitialInfo_(-1),
      iniEnergyFromInitialInfo_(0.0),

      // ★ 追加機能用ヒストグラム等をnullptr初期化
      hXrayEvtPH3_15_IniE_(nullptr),
      hIniEnergyAll_(nullptr),
      hMapIniEvsPHsum_(nullptr),
      pivot_energy_(10.0),
      hPivotPHsum_(nullptr),

      // ▼ 追加 ▼
      enable_target_ini_energy_(true),
      target_ini_energy_(46.0),
      hXrayEventSpectrum_0_1keVbins_targetIniE_(nullptr)
{
    // 有効パターン一覧を初期化
    validPatterns = {
        PATTERN_ONE_PERFECT,
        PATTERN_ONE_DETOUCHED_CORNER,
        PATTERN_TWO_SINGLE_SIDE,
        PATTERN_TWO_DETOUCHED_CORNER,
        PATTERN_THREE_L,
        PATTERN_THREE_L_DETOUCHED_CORNER,
        PATTERN_FOUR_TRIPLE_SIDE,
        PATTERN_FOUR_SQUARE,
        PATTERN_FOUR_SQUARE_DETOUCHED_CORNER,
        PATTERN_FIVE_QUADRUPLE_SIDE,
        PATTERN_FIVE_LEFT_CORNER,
        PATTERN_FIVE_RIGHT_CORNER,
        PATTERN_SIX_RECTANGLE,
        PATTERN_SEVEN_RECTANGLE_SINGLE_SIDE,
        PATTERN_NINE_ALL_PIXELS
    };

    // split_cnt のセットを初期化
    validSplitCounts = {0, 1, 2, 3, 4};
}

// ----------------------------------------------------------------------------
// デストラクタ
// ----------------------------------------------------------------------------
Write3x3EventTree::~Write3x3EventTree()
{
    if (inputFile_) {
        inputFile_->Close();
        delete inputFile_;
        inputFile_ = nullptr;
    }
    if (outputFile_) {
        outputFile_->Close();
        delete outputFile_;
        outputFile_ = nullptr;
    }

    // 既存ヒストグラムの削除
    if (hEnergySpectrum) {
        delete hEnergySpectrum; hEnergySpectrum = nullptr;
    }
    if (hXrayEventSpectrum) {
        delete hXrayEventSpectrum; hXrayEventSpectrum = nullptr;
    }
    if (hNonXrayEventSpectrum) {
        delete hNonXrayEventSpectrum; hNonXrayEventSpectrum = nullptr;
    }
    if (hPatternDistribution) {
        delete hPatternDistribution; hPatternDistribution = nullptr;
    }
    if (hXrayPatternDistribution) {
        delete hXrayPatternDistribution; hXrayPatternDistribution = nullptr;
    }

    if (hXrayEventSpectrum_0_1keVbins) {
        delete hXrayEventSpectrum_0_1keVbins; hXrayEventSpectrum_0_1keVbins = nullptr;
    }
    if (hXrayEventSpectrum_0_25keVbins) {
        delete hXrayEventSpectrum_0_25keVbins; hXrayEventSpectrum_0_25keVbins = nullptr;
    }
    if (hXrayEventSpectrum_0_5keVbins) {
        delete hXrayEventSpectrum_0_5keVbins; hXrayEventSpectrum_0_5keVbins = nullptr;
    }
    if (hXrayEventSpectrum_1keVbins) {
        delete hXrayEventSpectrum_1keVbins; hXrayEventSpectrum_1keVbins = nullptr;
    }

    for (auto& pair : hPatternEnergySpectra) {
        if (pair.second) {
            delete pair.second;
            pair.second = nullptr;
        }
    }
    for (auto& pair : hSplitCountSpectra) {
        if (pair.second) {
            delete pair.second;
            pair.second = nullptr;
        }
    }

    if (hIniEnergyVsPH_) {
        delete hIniEnergyVsPH_;
        hIniEnergyVsPH_ = nullptr;
    }
    if (hXrayCountVsIniEnergy_) {
        delete hXrayCountVsIniEnergy_;
        hXrayCountVsIniEnergy_ = nullptr;
    }

    // ★ 追加機能ヒストグラムの削除
    if (hXrayEvtPH3_15_IniE_) {
        delete hXrayEvtPH3_15_IniE_;
        hXrayEvtPH3_15_IniE_ = nullptr;
    }
    if (hIniEnergyAll_) {
        delete hIniEnergyAll_;
        hIniEnergyAll_ = nullptr;
    }
    if (hMapIniEvsPHsum_) {
        delete hMapIniEvsPHsum_;
        hMapIniEvsPHsum_ = nullptr;
    }
    if (hPivotPHsum_) {
        delete hPivotPHsum_;
        hPivotPHsum_ = nullptr;
    }

    // ▼ 追加ヒストグラムの削除 ▼
    if (hXrayEventSpectrum_0_1keVbins_targetIniE_) {
        delete hXrayEventSpectrum_0_1keVbins_targetIniE_;
        hXrayEventSpectrum_0_1keVbins_targetIniE_ = nullptr;
    }
}

// ----------------------------------------------------------------------------
// mod_define
// ----------------------------------------------------------------------------
ANLStatus Write3x3EventTree::mod_define()
{
    define_parameter("input_filename", &Write3x3EventTree::input_filename_);
    define_parameter("output_filename", &Write3x3EventTree::output_filename_);
    define_parameter("tree_name", &Write3x3EventTree::tree_name_);
    define_parameter("sprit_threshold", &Write3x3EventTree::sprit_threshold_);
    define_parameter("event_threshold", &Write3x3EventTree::event_threshold_);
    define_parameter("exposure_time", &Write3x3EventTree::exposure_time_);

    // "ini_energy" ブランチを読むかどうか
    define_parameter("enable_energy_correlation", &Write3x3EventTree::enable_energy_correlation_);

    // ★ 追加: ピボットエネルギーをパラメータ化
    define_parameter("pivot_energy", &Write3x3EventTree::pivot_energy_);

    // ▼ 追加: 特定の iniEnergy でフィルターするかどうか、およびその値
    define_parameter("enable_target_ini_energy", &Write3x3EventTree::enable_target_ini_energy_);
    define_parameter("target_ini_energy", &Write3x3EventTree::target_ini_energy_);

    return AS_OK;
}

// ----------------------------------------------------------------------------
// mod_initialize
// ----------------------------------------------------------------------------
ANLStatus Write3x3EventTree::mod_initialize()
{
    // 他モジュール取得
    get_module_NC("CSHitCollection", &hitCollection_);
    define_evs("Write3x3EventTree:Fill");

    if (exist_module("InitialInformation")) {
        get_module_IFNC("InitialInformation", &initialInfo_);
    }

    // 入力ROOTファイルを開く
    inputFile_ = TFile::Open(input_filename_.c_str(), "READ");
    if (!inputFile_ || inputFile_->IsZombie()) {
        std::cerr << "入力ROOTファイルのオープンに失敗: " << input_filename_ << std::endl;
        return AS_QUIT_ERROR;
    }

    // 入力ツリーを取得
    inputTree_ = dynamic_cast<TTree*>(inputFile_->Get(tree_name_.c_str()));
    if (!inputTree_) {
        std::cerr << "入力ツリー '" << tree_name_ << "' が見つかりません。" << std::endl;
        return AS_QUIT_ERROR;
    }

    // ini_energy ブランチの設定
    if (enable_energy_correlation_) {
        if (inputTree_->FindBranch("ini_energy")) {
            inputTree_->SetBranchAddress("ini_energy", &ini_Energy_);
            std::cerr << "[INFO] 'ini_energy' branch found. Will read it.\n";
        } else {
            std::cerr << "[WARNING] 'ini_energy' branch NOT found!\n";
        }
    }

    // 出力ファイルを作成
    outputFile_ = TFile::Open(output_filename_.c_str(), "RECREATE");
    if (!outputFile_ || outputFile_->IsZombie()) {
        std::cerr << "出力ROOTファイルの作成に失敗: " << output_filename_ << std::endl;
        return AS_QUIT_ERROR;
    }

    // 入力ツリー構造をクローン (空ツリー)
    outputTree_ = inputTree_->CloneTree(0);

    // 出力ツリーブランチをセットアップ
    SetupBranches();

    // (既存) ヒストグラム初期化
    hEnergySpectrum      = new TH1D("hEnergySpectrum",       "Energy Spectrum",            1000, 0, 100);
    hXrayEventSpectrum   = new TH1D("hXrayEventSpectrum",    "X-ray Event Spectrum",       1000, 0, 100);
    hNonXrayEventSpectrum= new TH1D("hNonXrayEventSpectrum", "Non X-ray Event Spectrum",   1000, 0, 100);

    hPatternDistribution     = new TH1D("hPatternDistribution",     "Pattern Distribution (All Events)",   100, 0, 100);
    hXrayPatternDistribution = new TH1D("hXrayPatternDistribution", "Pattern Distribution (X-ray Events)", 100, 0, 100);

    // パターン番号ごとのヒストグラム
    for (const int pattern : validPatterns) {
        hPatternEnergySpectra[pattern] = new TH1D(Form("hPattern_%d", pattern),
                                                  Form("Energy Spectrum for Pattern %d", pattern),
                                                  1000, 0, 100);
    }
    // split_cnt ごとのヒストグラム
    for (const int sc : validSplitCounts) {
        hSplitCountSpectra[sc] = new TH1D(Form("hSplitCount_%d", sc),
                                          Form("Energy Spectrum for Split Count %d", sc),
                                          1000, 0, 100);
    }

    hXrayEventSpectrum_0_1keVbins  = new TH1D("hXrayEventSpectrum_0_1keVbins",  "X-ray Event Spectrum (0.1 keV bins)",  200, 0, 20);
    hXrayEventSpectrum_0_25keVbins = new TH1D("hXrayEventSpectrum_0_25keVbins", "X-ray Event Spectrum (0.25 keV bins)", 80,  0, 20);
    hXrayEventSpectrum_0_5keVbins  = new TH1D("hXrayEventSpectrum_0_5keVbins",  "X-ray Event Spectrum (0.5 keV bins)",  40,  0, 20);
    hXrayEventSpectrum_1keVbins    = new TH1D("hXrayEventSpectrum_1keVbins",    "X-ray Event Spectrum (1 keV bins)",    20,  0, 20);

    // (既存) 2Dヒスト (ini_Energy vs PH)
    if (enable_energy_correlation_) {
        hIniEnergyVsPH_ = new TH2D("hIniEnergyVsPH", "ini_Energy vs PH",
                                   300, 0.0, 300.0,  // x=ini_Energy
                                   300, 0.0, 300.0); // y=PH sum
        hIniEnergyVsPH_->GetXaxis()->SetTitle("ini_Energy (keV)");
        hIniEnergyVsPH_->GetYaxis()->SetTitle("Measured PH (keV)");
    }

    // (既存) ini_Energyごとの X-ray(3–15 keV)イベント数カウント用
    hXrayCountVsIniEnergy_ = new TH1D("hXrayCountVsIniEnergy",
                                      "X-ray(3-15 keV) Count vs ini_Energy;ini_Energy [keV];Counts",
                                      101, 0.0, 101.0);

    patternCounts.clear();
    xrayPatternCounts.clear();

    //--------------------------------------------------------------------------
    // ★ 追加機能ヒストグラムの初期化
    //--------------------------------------------------------------------------
    // 1) X軸= iniEnergyFromInitialInfo(1-300keV), Y= X-ray(PH sum 3-15)イベント数
    hXrayEvtPH3_15_IniE_ = new TH1D("hXrayEvtPH3_15_IniE",
        "X-ray(PH=3-15 keV) events vs iniEnergyFromInitialInfo;iniEnergyFromInitialInfo [keV];Counts",
        300, 0.5, 300.5);

    // 2) X軸= iniEnergyFromInitialInfo(1-300keV), Y= 全イベント数
    hIniEnergyAll_ = new TH1D("hIniEnergyAll",
        "All events vs iniEnergyFromInitialInfo;iniEnergyFromInitialInfo [keV];Counts",
        300, 0.5, 300.5);

    // 3) 2Dマップ (x=iniEnergyFromInitialInfo [1..300], y=PHsum [1..300])
    hMapIniEvsPHsum_ = new TH2D("hMapIniEvsPHsum",
        "Map: iniEnergyFromInitialInfo vs PHsum;iniEnergy[keV];PHsum[keV]",
        300, 0.5, 300.5,
        300, 0.5, 300.5);

    // 4) pivot_energy_±0.5 keV のイベントの PHsumヒスト
    hPivotPHsum_ = new TH1D("hPivotPHsum",
        "PHsum for pivot_energy_ ±0.5 keV;PHsum [keV];Counts",
        300, 0.5, 300.5);

    // ▼ 追加: 特定 iniEnergy での X-ray Spectrum (0.1 keV bins)
    if (enable_target_ini_energy_) {
        hXrayEventSpectrum_0_1keVbins_targetIniE_ = new TH1D(
            "hXrayEventSpectrum_0_1keVbins_targetIniE",
            Form("X-ray Event Spectrum (0.1 keV bins) [iniE=%.2f keV]", target_ini_energy_),
            200, 0, 20
        );
    }

    return AS_OK;
}

// ----------------------------------------------------------------------------
// SetupBranches
// ----------------------------------------------------------------------------
void Write3x3EventTree::SetupBranches()
{
    // 既存ブランチ
    outputTree_->Branch("centerHitEnergy_keV", &centerHitEnergy_, "centerHitEnergy_keV/D");
    outputTree_->Branch("maxEnergy_keV", &maxEnergy_, "maxEnergy_keV/D");
    outputTree_->Branch("hitEnergies_keV", hitEnergies_, "hitEnergies_keV[9]/D");
    outputTree_->Branch("pixelX", pixelX_, "pixelX[9]/I");
    outputTree_->Branch("pixelY", pixelY_, "pixelY[9]/I");

    outputTree_->Branch("real_time", &real_time_, "real_time/D");
    outputTree_->Branch("frame_exposure_time", &frame_exposure_time_, "frame_exposure_time/D");
    outputTree_->Branch("delay_time", &delay_time_, "delay_time/D");

    outputTree_->Branch("Center_Hit_pixelX", &Center_Hit_pixelX_, "Center_Hit_pixelX/I");
    outputTree_->Branch("Center_Hit_pixelY", &Center_Hit_pixelY_, "Center_Hit_pixelY/I");

    outputTree_->Branch("real_exposuretime", &real_exposuretime_, "real_exposuretime/D");
    outputTree_->Branch("Select_Event_ID", &Select_Event_ID_, "Select_Event_ID/I");
    outputTree_->Branch("select_Event_real_time", &select_Event_real_time_, "select_Event_real_time/D");

    outputTree_->Branch("EVENT_ID", &EVENT_ID_, "EVENT_ID/I");
    outputTree_->Branch("time", &event_time_, "time/D");
    outputTree_->Branch("Real_time", &Real_time_, "Real_time/D");

    // pattern関連
    outputTree_->Branch("pattern", &pattern_, "pattern/b");
    outputTree_->Branch("split_cnt", &split_cnt_, "split_cnt/I");
    outputTree_->Branch("pattern_type", &pattern_type_, "pattern_type/I");
    outputTree_->Branch("pixel_pattern", &pixel_pattern_, "pixel_pattern/I");

    // enable_energy_correlation_ が true のとき、ini_Energyを出力する
    if (enable_energy_correlation_) {
        outputTree_->Branch("ini_Energy", &ini_Energy_, "ini_Energy/F");
    }

    // initialInfo_由来の eventID / 初期エネルギー(keV)
    outputTree_->Branch("eventIDFromInitialInfo", &eventIDFromInitialInfo_, "eventIDFromInitialInfo/L");
    outputTree_->Branch("iniEnergyFromInitialInfo", &iniEnergyFromInitialInfo_, "iniEnergyFromInitialInfo/D");
}

// ----------------------------------------------------------------------------
// mod_analyze
// ----------------------------------------------------------------------------
ANLStatus Write3x3EventTree::mod_analyze()
{
    // InitialInformation から eventID と初期粒子エネルギー(keV)を取得
    int64_t eventID = -1;
    double  initialEnergy_keV = 0.0;

    if (initialInfo_) {
        eventID = initialInfo_->EventID();
        // 例: InitialEnergy() が MeV なら ×1000.0 で keV 換算
        initialEnergy_keV = initialInfo_->InitialEnergy() * 1000.0;

        // 時間情報なども取得
        real_time_ = initialInfo_->RealTime();
        frame_exposure_time_ = initialInfo_->FrameExposureTime();
        delay_time_ = CalculateDelayTime();
        real_exposuretime_ = initialInfo_->EndOfDeadResetTime() + frame_exposure_time_;

        Select_Event_ID_ = initialInfo_->SelectEventID();
        select_Event_real_time_ = initialInfo_->SelectEventRealTime();
    }
    else {
        // InitialInformation が無い場合は何もしないか、エラー扱いでも良い
        return AS_QUIT_ERROR;
    }

    // eventID<0 → 無効イベントの場合はスキップ
    if (eventID < 0) {
        return AS_OK;
    }

    // "ini_energy" ブランチを読むために inputTree_ の該当 Entry をロード
    inputTree_->GetEntry(eventID);

    // eventIDFromInitialInfo_ と iniEnergyFromInitialInfo_ を代入
    eventIDFromInitialInfo_   = eventID;
    iniEnergyFromInitialInfo_ = initialEnergy_keV;

    // ヒットコレクションから全ヒット取得
    std::vector<std::shared_ptr<DetectorHit>> allHits;
    int NumTimeGroups = hitCollection_->NumberOfTimeGroups();
    for (int tg = 0; tg < NumTimeGroups; tg++) {
        const auto& hits = hitCollection_->getHits(tg);
        allHits.insert(allHits.end(), hits.begin(), hits.end());
    }

    // 3x3ヒットをまとめる
    bool success = get3x3HitData(allHits);
    if (!success) {
        return AS_OK;
    }

    // パターン生成
    pattern_ = GeneratePattern(hitEnergies_);
    vortex_  = pattern_;
    split_cnt_ = CountSplitBits(pattern_);

    int rotate_num = GetRotateNum(split_cnt_, vortex_);
    uint8_t rotated_vortex = rotate_vortex(vortex_, rotate_num);
    int rotated_split_cnt = CountSplitBits(rotated_vortex);
    split_cnt_ = rotated_split_cnt;

    pattern_type_ = VortexToPattern(split_cnt_, rotated_vortex);
    pixel_pattern_ = pattern_type_;

    // エネルギー合算 (PH sum)
    double phSum = PHMerge(pattern_type_, rotate_num);

    // (既存) 2Dヒスト: (ini_Energy_, phSum) Fill
    if (enable_energy_correlation_ && hIniEnergyVsPH_) {
        hIniEnergyVsPH_->Fill(ini_Energy_, phSum);
    }

    // (既存) X-ray(3–15 keV) カウント
    bool is_xray = (phSum >= 3.0 && phSum < 15.0);
    if (enable_energy_correlation_ && is_xray) {
        hXrayCountVsIniEnergy_->Fill(ini_Energy_);
    }

    // (既存) パターンカウント等
    patternCounts[pattern_type_]++;
    hPatternDistribution->Fill(pattern_type_);
    hEnergySpectrum->Fill(phSum);

    if (pattern_type_ != NON_XRAY) {
        hXrayEventSpectrum->Fill(phSum);
        hXrayEventSpectrum_0_1keVbins->Fill(phSum);
        hXrayEventSpectrum_0_25keVbins->Fill(phSum);
        hXrayEventSpectrum_0_5keVbins->Fill(phSum);
        hXrayEventSpectrum_1keVbins->Fill(phSum);

        xrayPatternCounts[pattern_type_]++;
        hXrayPatternDistribution->Fill(pattern_type_);
    }
    else {
        hNonXrayEventSpectrum->Fill(phSum);
    }

    // validPatterns, validSplitCounts のスペクトル加算
    if (validPatterns.find(pattern_type_) != validPatterns.end()) {
        hPatternEnergySpectra[pattern_type_]->Fill(phSum);
    }
    if (validSplitCounts.find(split_cnt_) != validSplitCounts.end()) {
        hSplitCountSpectra[split_cnt_]->Fill(phSum);
    }

    // EVENT_ID (連番)
    EVENT_ID_++;
    event_time_ = real_time_;
    Real_time_  = real_time_;

    //--------------------------------------------------------------------------
    // ★ 追加機能の実装
    //--------------------------------------------------------------------------

    // 1) X軸 iniEnergyFromInitialInfo [1..300], 縦軸は "PHsum=3..15" でカウント
    if (phSum >= 3.0 && phSum < 15.0) {
        hXrayEvtPH3_15_IniE_->Fill(iniEnergyFromInitialInfo_);
    }

    // 2) X軸 iniEnergyFromInitialInfo [1..300], 縦軸= 全イベント数
    hIniEnergyAll_->Fill(iniEnergyFromInitialInfo_);

    // 3) 2Dマップ: (x=iniEnergyFromInitialInfo, y=phSum)
    hMapIniEvsPHsum_->Fill(iniEnergyFromInitialInfo_, phSum);

    // 4) pivot_energy_±0.5 keV のイベントの PHsum
    if ( (iniEnergyFromInitialInfo_ >= (pivot_energy_ - 0.5)) &&
         (iniEnergyFromInitialInfo_ <  (pivot_energy_ + 0.5)) ) {
        hPivotPHsum_->Fill(phSum);
    }

    // ▼ 追加: 特定 iniEnergy での X-ray Event Spectrum (0.1 keV bins) への Fill
    if (enable_target_ini_energy_) {
        // ここでは厳密に「指定エネルギーと一致」かどうかを判定
        // 必要に応じて ±0.5 keV など、トレランスを設定しても OK
        if (std::fabs(iniEnergyFromInitialInfo_ - target_ini_energy_) < 0.5) {
            // X-ray パターン(NON_XRAY 以外)であればヒストグラムに Fill
            if (pattern_type_ != NON_XRAY) {
                hXrayEventSpectrum_0_1keVbins_targetIniE_->Fill(phSum);
            }
        }
    }

    //--------------------------------------------------------------------------
    // 出力ツリーに書き込む
    //--------------------------------------------------------------------------
    outputTree_->Fill();
    evs("Write3x3EventTree:Fill");

    return AS_OK;
}

// ----------------------------------------------------------------------------
// mod_finalize
// ----------------------------------------------------------------------------
ANLStatus Write3x3EventTree::mod_finalize()
{
    // 出力ツリーの書き込み
    if (outputFile_) {
        outputFile_->cd();
        outputTree_->Write();

        // ヒストグラム(既存)書き込み
        hEnergySpectrum->Write();
        hXrayEventSpectrum->Write();
        hNonXrayEventSpectrum->Write();

        double binWidth_0_1keV   = hXrayEventSpectrum_0_1keVbins->GetXaxis()->GetBinWidth(1);
        double binWidth_0_25keV  = hXrayEventSpectrum_0_25keVbins->GetXaxis()->GetBinWidth(1);
        double binWidth_0_5keV   = hXrayEventSpectrum_0_5keVbins->GetXaxis()->GetBinWidth(1);
        double binWidth_1keV     = hXrayEventSpectrum_1keVbins->GetXaxis()->GetBinWidth(1);

        // スケーリング (exposure_time_ によるレート換算)
        // 例: counts / (exposure_time * binWidth)
        if (exposure_time_ > 0.0) {
            hXrayEventSpectrum_0_1keVbins->Scale(1.0 / (exposure_time_ * binWidth_0_1keV));
            hXrayEventSpectrum_0_25keVbins->Scale(1.0 / (exposure_time_ * binWidth_0_25keV));
            hXrayEventSpectrum_0_5keVbins->Scale(1.0 / (exposure_time_ * binWidth_0_5keV));
            hXrayEventSpectrum_1keVbins->Scale(1.0 / (exposure_time_ * binWidth_1keV));
        }

        hXrayEventSpectrum_0_1keVbins->Write();
        hXrayEventSpectrum_0_25keVbins->Write();
        hXrayEventSpectrum_0_5keVbins->Write();
        hXrayEventSpectrum_1keVbins->Write();

        // パターンごとのヒストグラム
        for (auto& pair : hPatternEnergySpectra) {
            if (pair.second) {
                pair.second->Write();
            }
        }
        // split_cnt ごとのヒストグラム
        for (auto& pair : hSplitCountSpectra) {
            if (pair.second) {
                pair.second->Write();
            }
        }

        hPatternDistribution->Write();
        hXrayPatternDistribution->Write();

        // 2Dヒスト (ini_Energy vs PH)
        if (enable_energy_correlation_ && hIniEnergyVsPH_) {
            hIniEnergyVsPH_->Write();
        }

        // X-ray(3–15 keV)イベントカウント vs ini_Energy
        if (hXrayCountVsIniEnergy_) {
            hXrayCountVsIniEnergy_->Write();

            // TGraph に変換して保存 (オプション)
            TGraph* gXrayCountVsIniEnergy = new TGraph();
            gXrayCountVsIniEnergy->SetName("gXrayCountVsIniEnergy");
            gXrayCountVsIniEnergy->SetTitle("X-ray(3-15 keV) Count vs ini_Energy;ini_Energy [keV];Counts");

            int nBins = hXrayCountVsIniEnergy_->GetNbinsX();
            for (int iBin = 1; iBin <= nBins; ++iBin) {
                double x = hXrayCountVsIniEnergy_->GetBinCenter(iBin);
                double y = hXrayCountVsIniEnergy_->GetBinContent(iBin);
                gXrayCountVsIniEnergy->SetPoint(gXrayCountVsIniEnergy->GetN(), x, y);
            }
            gXrayCountVsIniEnergy->Write();
            delete gXrayCountVsIniEnergy;
        }

        // ★ 追加機能ヒストグラムの書き出し
        hXrayEvtPH3_15_IniE_->Write();
        hIniEnergyAll_->Write();
        hMapIniEvsPHsum_->Write();
        hPivotPHsum_->Write();

        // ▼ 追加: 特定 iniEnergy ヒストグラムの書き出し
        if (hXrayEventSpectrum_0_1keVbins_targetIniE_) {
            // スケーリング
            if (exposure_time_ > 0.0) {
                double bw = hXrayEventSpectrum_0_1keVbins_targetIniE_->GetXaxis()->GetBinWidth(1);
                hXrayEventSpectrum_0_1keVbins_targetIniE_->Scale(1.0 / (exposure_time_ * bw));
            }
            hXrayEventSpectrum_0_1keVbins_targetIniE_->Write();
        }

        // ファイルを閉じる
        outputFile_->Close();
    }

    // パターン統計を表示
    OutputPatternStatistics();

    return AS_OK;
}

// ----------------------------------------------------------------------------
// 以下、ヘルパー関数 (get3x3HitData, IsWithin3x3, CalculateDelayTime, GeneratePattern,
//                     CountSplitBits, rotate_right_2bits, rotate_vortex, bitcheck,
//                     VortexToPattern, GetRotateNum, PHMerge, vortex_rotate_counterclockwise,
//                     OutputPatternStatistics) は省略なくそのまま
// ----------------------------------------------------------------------------

bool Write3x3EventTree::get3x3HitData(const std::vector<std::shared_ptr<DetectorHit>>& hits)
{
    if (hits.empty()) return false;

    // 中心ヒット（最大EPIを持つヒット）を探す
    auto itMax = std::max_element(hits.begin(), hits.end(),
        [](const std::shared_ptr<DetectorHit>& a, const std::shared_ptr<DetectorHit>& b){
            return a->EPI() < b->EPI();
        });
    if (itMax == hits.end()) {
        return false;
    }

    const auto& centerHit = *itMax;
    centerHitEnergy_ = centerHit->EPI() * 1000.0;

    Center_Hit_pixelX_ = centerHit->PixelX();
    Center_Hit_pixelY_ = centerHit->PixelY();

    maxEnergy_ = 0.0;
    std::fill(std::begin(hitEnergies_), std::end(hitEnergies_), 0.0);
    std::fill(std::begin(pixelX_), std::end(pixelX_), -1);
    std::fill(std::begin(pixelY_), std::end(pixelY_), -1);

    int cx = centerHit->PixelX();
    int cy = centerHit->PixelY();

    for (auto& h : hits) {
        if (IsWithin3x3(centerHit, h)) {
            int dx = h->PixelX() - cx;
            int dy = h->PixelY() - cy;
            int index = -1;
            if      (dx==-1 && dy== 1) index = 0;
            else if (dx==-1 && dy== 0) index = 1;
            else if (dx==-1 && dy==-1) index = 2;
            else if (dx== 0 && dy==-1) index = 3;
            else if (dx== 1 && dy==-1) index = 4;
            else if (dx== 1 && dy== 0) index = 5;
            else if (dx== 1 && dy== 1) index = 6;
            else if (dx== 0 && dy== 1) index = 7;
            else if (dx== 0 && dy== 0) index = 8;
            else continue;

            if (index>=0 && index<9) {
                double keV = h->EPI() * 1000.0;
                hitEnergies_[index] += keV;

                if (pixelX_[index]<0 && pixelY_[index]<0) {
                    pixelX_[index] = h->PixelX();
                    pixelY_[index] = h->PixelY();
                }
                if (keV > maxEnergy_) maxEnergy_ = keV;
            }
        }
    }

    return true;
}

bool Write3x3EventTree::IsWithin3x3(const std::shared_ptr<DetectorHit>& centerHit,
                                    const std::shared_ptr<DetectorHit>& hit)
{
    int cx = centerHit->PixelX();
    int cy = centerHit->PixelY();
    int x  = hit->PixelX();
    int y  = hit->PixelY();
    return (std::abs(cx - x)<=1) && (std::abs(cy - y)<=1);
}

double Write3x3EventTree::CalculateDelayTime()
{
    if (initialInfo_) {
        return initialInfo_->DelayTime();
    }
    return 0.0;
}

uint8_t Write3x3EventTree::GeneratePattern(const double hitEnergies[9])
{
    uint8_t pat = 0;
    for (int i=0; i<8; i++){
        if (hitEnergies[i] > sprit_threshold_) {
            pat |= (1<<(7-i));
        }
    }
    return pat;
}

int Write3x3EventTree::CountSplitBits(uint8_t pattern)
{
    int cnt=0;
    int bitPos[4] = {0,2,4,6};
    for (int i=0;i<4;i++){
        if (pattern & (1<<bitPos[i])) cnt++;
    }
    return cnt;
}

bool Write3x3EventTree::bitcheck(uint8_t vortex, int bit_position)
{
    return (vortex & (1<<bit_position))!=0;
}

uint8_t Write3x3EventTree::rotate_right_2bits(uint8_t vortex)
{
    return ((vortex >> 2) | ((vortex << 6) & 0xFF));
}

uint8_t Write3x3EventTree::rotate_vortex(uint8_t vortex, int rotations)
{
    rotations = rotations%4;
    for(int i=0;i<rotations;i++){
        vortex = rotate_right_2bits(vortex);
    }
    return vortex;
}

uint8_t Write3x3EventTree::vortex_rotate_counterclockwise(uint8_t vortex)
{
    return ((vortex<<2)|(vortex>>6)) & 0xFF;
}

int Write3x3EventTree::GetRotateNum(int split_cnt, uint8_t current_vortex)
{
    int rotate_num=0;
    uint8_t v = current_vortex;

    switch(split_cnt){
    case 1:
        for(int i=0;i<4;i++){
            if (v & 1) break;
            v = vortex_rotate_counterclockwise(v);
            rotate_num++;
        }
        break;
    case 2:
        for(int i=0;i<4;i++){
            if ((v & 0x41)==0x41) break;
            v = vortex_rotate_counterclockwise(v);
            rotate_num++;
        }
        break;
    case 3:
        for(int i=0;i<4;i++){
            if ((v & 0x45)==0x45) break;
            v = vortex_rotate_counterclockwise(v);
            rotate_num++;
        }
        break;
    case 4:
        if (v==VORTEX_SIDE_MASK){
            break;
        } else {
            int corner_cnt=0;
            uint8_t tmp=v;
            for(int i=0;i<4;i++){
                if (tmp & VORTEX_CORNER_MASK) corner_cnt++;
                tmp = vortex_rotate_counterclockwise(tmp);
            }
            switch(corner_cnt){
            case 1:
                for(int i=0;i<4;i++){
                    if(v & VORTEX_CORNER_MASK) break;
                    v = vortex_rotate_counterclockwise(v);
                    rotate_num++;
                }
                break;
            case 2:
                if(((v & VORTEX_CORNER_MASK)==0x22) || ((v & VORTEX_CORNER_MASK)==0x88)){
                    for(int i=0;i<2;i++){
                        if((v & 0x88)==0x88) break;
                        v = vortex_rotate_counterclockwise(v);
                        rotate_num++;
                    }
                } else {
                    for(int i=0;i<4;i++){
                        if((v & 0x82)==0x82) break;
                        v = vortex_rotate_counterclockwise(v);
                        rotate_num++;
                    }
                }
                break;
            case 3:
                for(int i=0;i<4;i++){
                    if(!(v & 0x08)) break;
                    v = vortex_rotate_counterclockwise(v);
                    rotate_num++;
                }
                break;
            case 4:
                break;
            }
        }
        break;
    default:
        rotate_num=0;
        break;
    }
    return rotate_num;
}

int Write3x3EventTree::VortexToPattern(int split_cnt, uint8_t vortex)
{
    int pattern = NON_XRAY;
    switch(split_cnt){
    case 0:
        if (vortex & VORTEX_CORNER_MASK){
            pattern = PATTERN_ONE_DETOUCHED_CORNER;
        } else {
            pattern = PATTERN_ONE_PERFECT;
        }
        break;
    case 1:
        if (!(vortex & VORTEX_CORNER_MASK)){
            pattern = PATTERN_TWO_SINGLE_SIDE;
        }
        else if (!(vortex & 0x82)){
            pattern = PATTERN_TWO_DETOUCHED_CORNER;
        }
        else {
            pattern = NON_XRAY;
        }
        break;
    case 2:
        if ((bitcheck(vortex,2) && bitcheck(vortex,6)) || (bitcheck(vortex,0)&&bitcheck(vortex,4))){
            pattern = NON_XRAY;
        }
        else if (vortex==0x41){
            pattern = PATTERN_THREE_L;
        }
        else if (!(vortex & 0xA2)){
            pattern = PATTERN_THREE_L_DETOUCHED_CORNER;
        }
        else if (vortex==0xC1){
            pattern = PATTERN_FOUR_SQUARE;
        }
        else if (vortex==0xC9){
            pattern = PATTERN_FOUR_SQUARE_DETOUCHED_CORNER;
        }
        else {
            pattern = NON_XRAY;
        }
        break;
    case 3:
        if (vortex==0x45){
            pattern = PATTERN_FOUR_TRIPLE_SIDE;
        }
        else if(vortex==0xC5){
            pattern = PATTERN_FIVE_LEFT_CORNER;
        }
        else if(vortex==0x47){
            pattern = PATTERN_FIVE_RIGHT_CORNER;
        }
        else if(vortex==0xC7){
            pattern = PATTERN_SIX_RECTANGLE;
        }
        else{
            pattern = NON_XRAY;
        }
        break;
    case 4:
        if (vortex==VORTEX_SIDE_MASK){
            pattern = PATTERN_FIVE_QUADRUPLE_SIDE;
        }
        else if(vortex==0xD7){
            pattern = PATTERN_SEVEN_RECTANGLE_SINGLE_SIDE;
        }
        else if(vortex==0xFF){
            pattern = PATTERN_NINE_ALL_PIXELS;
        }
        else {
            pattern = NON_XRAY;
        }
        break;
    default:
        pattern = NON_XRAY;
        break;
    }
    return pattern;
}

double Write3x3EventTree::PHMerge(int pattern, int rotate_num)
{
    double vortex_ph_[9];
    vortex_ph_[0] = hitEnergies_[8];
    vortex_ph_[1] = hitEnergies_[0];
    vortex_ph_[2] = hitEnergies_[1];
    vortex_ph_[3] = hitEnergies_[2];
    vortex_ph_[4] = hitEnergies_[3];
    vortex_ph_[5] = hitEnergies_[4];
    vortex_ph_[6] = hitEnergies_[5];
    vortex_ph_[7] = hitEnergies_[6];
    vortex_ph_[8] = hitEnergies_[7];

    static std::vector<std::vector<int>> rotation_map = {
        {0,1,2,3,4,5,6,7,8},
        {0,7,8,1,2,3,4,5,6},
        {0,5,6,7,8,1,2,3,4},
        {0,3,4,5,6,7,8,1,2}
    };

    auto get_rotated_index = [&](int i){ return rotation_map[rotate_num%4][i]; };

    double sum=0.0;
    if (pattern==PATTERN_ONE_PERFECT || pattern==PATTERN_ONE_DETOUCHED_CORNER){
        sum = vortex_ph_[0];
    }
    else if (pattern==PATTERN_TWO_SINGLE_SIDE || pattern==PATTERN_TWO_DETOUCHED_CORNER){
        sum = vortex_ph_[0] + vortex_ph_[ get_rotated_index(8) ];
    }
    else if (pattern==PATTERN_THREE_L || pattern==PATTERN_THREE_L_DETOUCHED_CORNER){
        sum = vortex_ph_[0]
            + vortex_ph_[ get_rotated_index(2) ]
            + vortex_ph_[ get_rotated_index(8) ];
    }
    else if (pattern==PATTERN_FOUR_TRIPLE_SIDE){
        sum = vortex_ph_[0]
            + vortex_ph_[ get_rotated_index(8) ]
            + vortex_ph_[ get_rotated_index(2) ]
            + vortex_ph_[ get_rotated_index(6) ];
    }
    else if (pattern==PATTERN_FOUR_SQUARE || pattern==PATTERN_FOUR_SQUARE_DETOUCHED_CORNER){
        sum = vortex_ph_[0]
            + vortex_ph_[ get_rotated_index(8) ]
            + vortex_ph_[ get_rotated_index(2) ]
            + vortex_ph_[ get_rotated_index(1) ];
    }
    else if (pattern==PATTERN_FIVE_QUADRUPLE_SIDE){
        sum = vortex_ph_[0] + vortex_ph_[2] + vortex_ph_[4] + vortex_ph_[6] + vortex_ph_[8];
    }
    else if (pattern==PATTERN_FIVE_LEFT_CORNER){
        sum = vortex_ph_[0]
            + vortex_ph_[ get_rotated_index(8) ]
            + vortex_ph_[ get_rotated_index(2) ]
            + vortex_ph_[ get_rotated_index(6) ]
            + vortex_ph_[ get_rotated_index(1) ];
    }
    else if (pattern==PATTERN_FIVE_RIGHT_CORNER){
        sum = vortex_ph_[0]
            + vortex_ph_[ get_rotated_index(8) ]
            + vortex_ph_[ get_rotated_index(2) ]
            + vortex_ph_[ get_rotated_index(6) ]
            + vortex_ph_[ get_rotated_index(7) ];
    }
    else if (pattern==PATTERN_SIX_RECTANGLE){
        sum = vortex_ph_[0]
            + vortex_ph_[ get_rotated_index(8) ]
            + vortex_ph_[ get_rotated_index(2) ]
            + vortex_ph_[ get_rotated_index(6) ]
            + vortex_ph_[ get_rotated_index(1) ]
            + vortex_ph_[ get_rotated_index(7) ];
    }
    else if (pattern==PATTERN_SEVEN_RECTANGLE_SINGLE_SIDE){
        sum = vortex_ph_[0] + vortex_ph_[2] + vortex_ph_[4] + vortex_ph_[6] + vortex_ph_[8]
            + vortex_ph_[ get_rotated_index(1) ] + vortex_ph_[ get_rotated_index(7) ];
    }
    else if (pattern==PATTERN_NINE_ALL_PIXELS){
        sum = vortex_ph_[0] + vortex_ph_[2] + vortex_ph_[4] + vortex_ph_[6] + vortex_ph_[8]
            + vortex_ph_[1] + vortex_ph_[3] + vortex_ph_[5] + vortex_ph_[7];
    }
    else {
        sum=0.0;
    }
    return sum;
}

void Write3x3EventTree::OutputPatternStatistics()
{
    int totalEvents = 0;
    for (const auto& p : patternCounts) {
        totalEvents += p.second;
    }
    int totalXrayEvents = 0;
    for (const auto& p : xrayPatternCounts) {
        totalXrayEvents += p.second;
    }

    std::cout << "\n--- Pattern Statistics (All Events) ---\n";
    for (const auto& p : patternCounts) {
        int patt = p.first;
        int count = p.second;
        double percentage = (totalEvents>0) ? (100.0*count/totalEvents) : 0.0;
        std::cout << "Pattern " << patt << ": " << count << " events ("
                  << percentage << " %)\n";
    }

    std::cout << "\n--- Pattern Statistics (X-ray Events) ---\n";
    for (const auto& p : xrayPatternCounts) {
        int patt = p.first;
        int count = p.second;
        double percentage = (totalXrayEvents>0) ? (100.0*count/totalXrayEvents) : 0.0;
        std::cout << "Pattern " << patt << ": " << count << " events ("
                  << percentage << " %)\n";
    }
}

} // namespace comptonsoft
