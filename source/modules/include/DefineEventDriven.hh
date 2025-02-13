#ifndef DEFINEEVENTDRIVEN_HH
#define DEFINEEVENTDRIVEN_HH

#include "VCSModule.hh"
#include "CSHitCollection.hh"
#include "InitialInformation.hh"
#include "DetectorHit.hh"
#include "AstroUnits.hh"

namespace comptonsoft {

/**
 * @brief DefineEventDriven モジュール
 *
 * イベント駆動型のイベント検出と処理を行います。
 */
class DefineEventDriven : public VCSModule {
    DEFINE_ANL_MODULE(DefineEventDriven, 1.0);

public:
    DefineEventDriven();
    virtual ~DefineEventDriven();

    virtual anlnext::ANLStatus mod_define() override;
    virtual anlnext::ANLStatus mod_initialize() override;
    virtual anlnext::ANLStatus mod_begin_run() override;
    virtual anlnext::ANLStatus mod_analyze() override;
    virtual anlnext::ANLStatus mod_finalize() override;

private:
    // モジュールポインタ
    CSHitCollection* m_HitCollection;
    anlgeant4::InitialInformation* m_InitialInfo;

    // シミュレーション時間管理
    double m_CurrentTime;
    double m_DeadTime;
    double m_NoEventTime;
    double m_ExposureTime;
    double m_EventThreshold;
    double m_TimeStep;

    // イベント管理
    int m_EventIndex; // Select_Event_ID として使用
    std::vector<std::shared_ptr<DetectorHit>> m_EventFrame;
    double m_FrameExposureTime;
    double m_EndOfDeadResetTime;

    // 状態管理
    enum class State { IDLE, DEAD_TIME, RESET_TIME, COLLECTING };
    State m_CurrentState;

    // 定数
    static constexpr double MAX_FRAME_EXPOSURE_TIME = 1.0; // 適切な値に設定

    // 内部関数
    void CreateEventFrame(const std::shared_ptr<DetectorHit>& triggerHit);
    bool IsWithin3x3(const std::shared_ptr<DetectorHit>& centerHit, const std::shared_ptr<DetectorHit>& hit);
    void ProcessEventFrame(const std::shared_ptr<DetectorHit>& centerHit);
};

} // namespace comptonsoft

#endif // DEFINEEVENTDRIVEN_HH
