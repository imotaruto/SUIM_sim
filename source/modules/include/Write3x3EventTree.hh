#ifndef WRITE3X3EVENTTREE_HH
#define WRITE3X3EVENTTREE_HH

#include "VCSModule.hh"
#include "CSHitCollection.hh"
#include "InitialInformation.hh"
#include "DetectorHit.hh"
#include "AstroUnits.hh"

#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include <functional>
#include <map>
#include <set>
#include <unordered_map>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <cstdint>

namespace comptonsoft {

/**
 * @brief Write3x3EventTree モジュール
 *
 * 3x3のヒットデータ+時間情報を含むイベントツリーを作成します。
 * - `InitialInformation` からイベントIDや初期粒子エネルギーを取得し、出力ツリーにブランチを作成。
 * - 入力ROOTから "ini_energy" ブランチを読む（オプション）→ enable_energy_correlation_ フラグで切替。
 * - 3x3ピクセルのヒットパターン解析 (pattern_ / pattern_type_) や PH 合算などを行い、結果を TTree & ヒストグラムに保存。
 *
 * さらに、以下の追加機能を実装:
 * 1. 横軸: iniEnergyFromInitialInfo [1..300 keV, 1keV刻み],
 *    縦軸: PHsum が 3–15 keV のイベント数 (X-ray判定) → グラフ (1Dヒスト)
 * 2. 横軸: iniEnergyFromInitialInfo [1..300 keV, 1keV刻み], 縦軸: 全イベント数 → グラフ (1Dヒスト)
 * 3. 横軸: iniEnergyFromInitialInfo [1..300 keV, 1keV刻み], 縦軸: PHsum [1..300 keV, 1keV刻み], 高さ=カウント → 2Dマップ
 * 4. 特定の iniEnergyFromInitialInfo (pivot_energy_) ±0.5 keV 範囲に入るイベントの PHsum 分布 → 1Dヒスト
 */
class Write3x3EventTree : public VCSModule {
    DEFINE_ANL_MODULE(Write3x3EventTree, 1.0);

public:
    Write3x3EventTree();
    virtual ~Write3x3EventTree();

    virtual anlnext::ANLStatus mod_define() override;
    virtual anlnext::ANLStatus mod_initialize() override;
    virtual anlnext::ANLStatus mod_analyze() override;
    virtual anlnext::ANLStatus mod_finalize() override;

private:
    //--------------------------------------------------------------------------
    // モジュールポインタ
    //--------------------------------------------------------------------------
    CSHitCollection* hitCollection_;
    anlgeant4::InitialInformation* initialInfo_;

    //--------------------------------------------------------------------------
    // ROOT 入出力
    //--------------------------------------------------------------------------
    TFile* inputFile_;
    TTree* inputTree_;
    TFile* outputFile_;
    TTree* outputTree_;

    //--------------------------------------------------------------------------
    // 3x3ヒット情報 (中心ヒットや9ピクセルの配列)
    //--------------------------------------------------------------------------
    double centerHitEnergy_; ///< 中心ヒットのエネルギー (keV)
    double maxEnergy_;       ///< 最大エネルギー (keV)
    double hitEnergies_[9];  ///< 3x3ヒットエネルギー配列 (keV)
    int pixelX_[9];          ///< 3x3ヒットのX座標
    int pixelY_[9];          ///< 3x3ヒットのY座標

    // 中心ピクセルの座標
    int Center_Hit_pixelX_;
    int Center_Hit_pixelY_;

    //--------------------------------------------------------------------------
    // 時間情報やイベントID関連
    //--------------------------------------------------------------------------
    double real_exposuretime_;
    int    Select_Event_ID_;
    double select_Event_real_time_;
    double real_time_;
    double frame_exposure_time_;
    double delay_time_;

    //--------------------------------------------------------------------------
    // ファイル名とツリー名
    //--------------------------------------------------------------------------
    std::string input_filename_;
    std::string output_filename_;
    std::string tree_name_;

    //--------------------------------------------------------------------------
    // EVENT_ID
    //--------------------------------------------------------------------------
    int EVENT_ID_;        ///< 再割り当てされたイベントID (連番)

    //--------------------------------------------------------------------------
    // time/Real_time
    //--------------------------------------------------------------------------
    double event_time_;   ///< 選択イベントの real_time を格納
    double Real_time_;    ///< real_time_ を格納する新しい変数

    //--------------------------------------------------------------------------
    // スレッショルド
    //--------------------------------------------------------------------------
    double sprit_threshold_; ///< パターン判定用エネルギースレッショルド (keV)
    double event_threshold_; ///< イベント判定用スレッショルド (keV)

    //--------------------------------------------------------------------------
    // ヒットパターンやピクセルパターン判定用
    //--------------------------------------------------------------------------
    uint8_t pattern_;
    int split_cnt_;
    int pattern_type_;
    int pixel_pattern_;

    //--------------------------------------------------------------------------
    // vortex_ 変数（現在のパターン状態）
    //--------------------------------------------------------------------------
    uint8_t vortex_; // 現在のパターン状態

    //--------------------------------------------------------------------------
    // ヒストグラム類(既存)
    //--------------------------------------------------------------------------
    TH1D* hEnergySpectrum;
    TH1D* hXrayEventSpectrum;
    TH1D* hNonXrayEventSpectrum;
    std::map<int, TH1D*> hPatternEnergySpectra;
    std::map<int, TH1D*> hSplitCountSpectra;

    TH1D* hPatternDistribution;
    TH1D* hXrayPatternDistribution;

    TH1D* hXrayEventSpectrum_0_1keVbins;
    TH1D* hXrayEventSpectrum_0_25keVbins;
    TH1D* hXrayEventSpectrum_0_5keVbins;
    TH1D* hXrayEventSpectrum_1keVbins;

    //--------------------------------------------------------------------------
    // パターンカウント
    //--------------------------------------------------------------------------
    std::unordered_map<int, int> patternCounts;
    std::unordered_map<int, int> xrayPatternCounts;

    //--------------------------------------------------------------------------
    // exposure_time
    //--------------------------------------------------------------------------
    double exposure_time_;

    //--------------------------------------------------------------------------
    // パターン番号や split_cnt の有効集合
    //--------------------------------------------------------------------------
    std::set<int> validPatterns;
    std::set<int> validSplitCounts;

    //--------------------------------------------------------------------------
    // 初期エネルギー(粒子発生時)との相関解析
    //--------------------------------------------------------------------------
    bool  enable_energy_correlation_;
    float ini_Energy_;
    TH2D* hIniEnergyVsPH_;
    TH1D* hXrayCountVsIniEnergy_;

    //--------------------------------------------------------------------------
    // initialInfo_->EventID() / InitialEnergy() 取得用
    //--------------------------------------------------------------------------
    Long64_t eventIDFromInitialInfo_;
    double   iniEnergyFromInitialInfo_;

    //--------------------------------------------------------------------------
    // ★ 追加機能用の変数
    //--------------------------------------------------------------------------
    // 1) X軸 iniEnergyFromInitialInfo(1-300keV)、Y軸 "PHsum=3-15keV"イベント数 → 1D
    TH1D* hXrayEvtPH3_15_IniE_;

    // 2) X軸 iniEnergyFromInitialInfo(1-300keV)、Y軸 全イベント数 → 1D
    TH1D* hIniEnergyAll_;

    // 3) X軸 iniEnergyFromInitialInfo(1-300keV)、Y軸 PHsum(1-300keV)、高さ=カウント → 2D
    TH2D* hMapIniEvsPHsum_;

    // 4) pivot_energy_ ±0.5 keV のイベントの PHsum → 1D
    double pivot_energy_;  ///< ユーザーが指定するピボットエネルギー
    TH1D* hPivotPHsum_;    ///< pivot_energy_付近の PHsum 分布

    // ★ 追加: 特定の iniEnergy でフィルタするための変数
    bool enable_target_ini_energy_;          ///< 特定の初期エネルギーでフィルタするかどうか
    double target_ini_energy_;               ///< フィルタの対象とする初期エネルギー(keV)
    TH1D* hXrayEventSpectrum_0_1keVbins_targetIniE_; ///< 指定 iniEnergy のイベントだけ Fill するヒストグラム

private:
    //--------------------------------------------------------------------------
    // 内部関数
    //--------------------------------------------------------------------------
    void SetupBranches();

    bool get3x3HitData(const std::vector<std::shared_ptr<DetectorHit>>& hits);
    bool IsWithin3x3(const std::shared_ptr<DetectorHit>& centerHit,
                     const std::shared_ptr<DetectorHit>& hit);
    double CalculateDelayTime();

    // パターン生成
    uint8_t GeneratePattern(const double hitEnergies[9]);
    int CountSplitBits(uint8_t pattern);
    int VortexToPattern(int split_cnt, uint8_t vortex);
    uint8_t rotate_right_2bits(uint8_t vortex);
    uint8_t rotate_vortex(uint8_t vortex, int rotations);
    bool bitcheck(uint8_t vortex, int bit_position);
    int GetRotateNum(int split_cnt, uint8_t current_vortex);
    double PHMerge(int pattern, int rotate_num);
    uint8_t vortex_rotate_counterclockwise(uint8_t vortex);

    // パターン統計出力
    void OutputPatternStatistics();

    //--------------------------------------------------------------------------
    // パターン定数
    //--------------------------------------------------------------------------
    static const uint8_t VORTEX_SIDE_MASK                      = 0x55; // 01010101
    static const uint8_t VORTEX_CORNER_MASK                    = 0xAA; // 10101010

    static const uint8_t NON_XRAY                              = 100;
    static const uint8_t PATTERN_ONE_PERFECT                   = 10;
    static const uint8_t PATTERN_ONE_DETOUCHED_CORNER          = 11;
    static const uint8_t PATTERN_TWO_SINGLE_SIDE               = 20;
    static const uint8_t PATTERN_TWO_DETOUCHED_CORNER          = 21;
    static const uint8_t PATTERN_THREE_L                       = 30;
    static const uint8_t PATTERN_THREE_L_DETOUCHED_CORNER      = 31;
    static const uint8_t PATTERN_FOUR_TRIPLE_SIDE              = 40;
    static const uint8_t PATTERN_FOUR_SQUARE                   = 41;
    static const uint8_t PATTERN_FOUR_SQUARE_DETOUCHED_CORNER  = 42;
    static const uint8_t PATTERN_FIVE_QUADRUPLE_SIDE           = 50;
    static const uint8_t PATTERN_FIVE_LEFT_CORNER              = 51;
    static const uint8_t PATTERN_FIVE_RIGHT_CORNER             = 52;
    static const uint8_t PATTERN_SIX_RECTANGLE                 = 60;
    static const uint8_t PATTERN_SEVEN_RECTANGLE_SINGLE_SIDE   = 70;
    static const uint8_t PATTERN_NINE_ALL_PIXELS               = 90;
};

} // namespace comptonsoft

#endif // WRITE3X3EVENTTREE_HH
