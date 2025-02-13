/*************************************************************************
 *                                                                       *
 * Copyright (c) 2011 Hirokazu Odaka                                     *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
 *************************************************************************/

#include "DefineEventDriven.hh"
#include <cmath>
#include <iostream>
#include "DetectorHit.hh"
#include "AstroUnits.hh" // 単位を定義するヘッダーファイル
#include "InitialInformation.hh"

using namespace anlnext;
namespace unit = anlgeant4::unit;

namespace comptonsoft {

DefineEventDriven::DefineEventDriven()
    : m_HitCollection(nullptr),
      m_InitialInfo(nullptr),
      m_CurrentTime(0.0),
      m_DeadTime(0.0),
      m_NoEventTime(0.0),
      m_ExposureTime(1.0 * unit::s),
      m_EventThreshold(0.1 * unit::keV),
      m_TimeStep(1e-6 * unit::s),
      m_EventIndex(0),
      m_EventFrame(),
      m_FrameExposureTime(0.0),
      m_EndOfDeadResetTime(0.0),
      m_CurrentState(State::IDLE)
{
}

DefineEventDriven::~DefineEventDriven() = default;

ANLStatus DefineEventDriven::mod_define() {
    define_parameter("event_threshold", &DefineEventDriven::m_EventThreshold, anlgeant4::unit::keV, "keV");
    define_parameter("exposure_time", &DefineEventDriven::m_ExposureTime, anlgeant4::unit::s, "s");
    return AS_OK;
}

ANLStatus DefineEventDriven::mod_initialize() {
    // モジュールの取得
    get_module_IFNC("InitialInformation", &m_InitialInfo);
    get_module_NC("CSHitCollection", &m_HitCollection);

    // 初期化
    m_CurrentTime = 0.0;
    m_DeadTime = 0.0;
    m_NoEventTime = 0.0;
    m_EventIndex = 0;
    m_EventFrame.clear();
    m_FrameExposureTime = 0.0;
    m_EndOfDeadResetTime = 0.0;
    m_CurrentState = State::IDLE;

    return AS_OK;
}

ANLStatus DefineEventDriven::mod_begin_run() {
    // ラン開始時の初期化
    m_CurrentTime = 0.0;
    m_DeadTime = 0.0;
    m_NoEventTime = 0.0;
    m_EventIndex = 0;
    m_EventFrame.clear();
    m_FrameExposureTime = 0.0;
    m_EndOfDeadResetTime = 0.0;
    m_CurrentState = State::IDLE;

    return AS_OK;
}

ANLStatus DefineEventDriven::mod_analyze() {
    if (m_CurrentTime >= m_ExposureTime) {
        return AS_QUIT;
    }

    switch (m_CurrentState) {
    case State::IDLE:
        {
            const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
            if (NumTimeGroups != 1) {
                return AS_QUIT_ERROR;
            }

            std::vector<std::shared_ptr<DetectorHit>>& hits = m_HitCollection->getHits(0);

            bool eventDetected = false;
            std::shared_ptr<DetectorHit> triggerHit;

            for (auto& hit : hits) {
                double epi = hit->EPI();
                if (epi >= m_EventThreshold) {
                    eventDetected = true;
                    triggerHit = hit;
                    break;
                }
            }

            if (eventDetected) {
                double event_time = m_CurrentTime;
                double frame_delay = event_time - m_EndOfDeadResetTime;
                double calculated_exposure = frame_delay + m_ExposureTime;

                if (calculated_exposure > MAX_FRAME_EXPOSURE_TIME) {
                    m_FrameExposureTime = MAX_FRAME_EXPOSURE_TIME;
                    CreateEventFrame(nullptr);
                    m_EventIndex++;
                    m_DeadTime = 400.0e-6;
                    m_CurrentState = State::DEAD_TIME;

                    if (m_InitialInfo) {
                        m_InitialInfo->setDelayTime(m_DeadTime);
                    }
                } else {
                    m_FrameExposureTime = m_ExposureTime;

                    if (m_InitialInfo) {
                        m_InitialInfo->setEventID(m_EventIndex);
                        m_InitialInfo->setRealTime(event_time);
                        m_InitialInfo->setFrameExposureTime(m_FrameExposureTime);
                        m_InitialInfo->setDelayTime(m_DeadTime);

                        // SelectEvent 系の更新
                        m_InitialInfo->setSelectEventID(m_EventIndex);
                        m_InitialInfo->setSelectEventRealTime(event_time);
                    }

                    CreateEventFrame(triggerHit);
                    m_EventIndex++;
                    m_DeadTime = 400.0e-6;
                    m_CurrentState = State::DEAD_TIME;

                    if (m_InitialInfo) {
                        m_InitialInfo->setDelayTime(m_DeadTime);
                    }
                }
            } else {
                m_NoEventTime += m_TimeStep;

                // 初期情報をリセット (イベントが無かった場合)
                if (m_InitialInfo) {
                    m_InitialInfo->setEventID(-1);
                    m_InitialInfo->setFrameExposureTime(0.0);
                    m_InitialInfo->setRealTime(m_CurrentTime);
                    m_InitialInfo->setDelayTime(0.0);
                    m_InitialInfo->setEndOfDeadResetTime(m_CurrentTime);
                    m_InitialInfo->setSelectEventID(-1);
                    m_InitialInfo->setSelectEventRealTime(m_CurrentTime);
                }

                if (m_NoEventTime >= 1.0e-3) {
                    m_DeadTime = 100.0e-6;
                    m_NoEventTime = 0.0;
                    m_EndOfDeadResetTime = m_CurrentTime + m_TimeStep;
                    m_CurrentState = State::RESET_TIME;

                    if (m_InitialInfo) {
                        m_InitialInfo->setDelayTime(m_DeadTime);
                    }
                }
            }

            m_CurrentTime += m_TimeStep;
            break;
        }

    case State::DEAD_TIME:
        {
            const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
            for (int timeGroup = 0; timeGroup < NumTimeGroups; ++timeGroup) {
                std::vector<std::shared_ptr<DetectorHit>>& hits = m_HitCollection->getHits(timeGroup);
                hits.clear();
            }

            if (m_InitialInfo) {
                m_InitialInfo->setEventID(-1);
                m_InitialInfo->setFrameExposureTime(0.0);
                m_InitialInfo->setRealTime(m_CurrentTime);
                m_InitialInfo->setDelayTime(0.0);
                m_InitialInfo->setEndOfDeadResetTime(m_CurrentTime);
                m_InitialInfo->setSelectEventID(-1);
                m_InitialInfo->setSelectEventRealTime(m_CurrentTime);
            }

            if (m_DeadTime > 0.0) {
                m_CurrentTime += m_TimeStep;
                m_DeadTime -= m_TimeStep;
            } else {
                m_CurrentState = State::IDLE;
                m_EndOfDeadResetTime = m_CurrentTime;
            }
            break;
        }

    case State::RESET_TIME:
        {
            const int NumTimeGroups = m_HitCollection->NumberOfTimeGroups();
            for (int timeGroup = 0; timeGroup < NumTimeGroups; ++timeGroup) {
                std::vector<std::shared_ptr<DetectorHit>>& hits = m_HitCollection->getHits(timeGroup);
                hits.clear();
            }

            if (m_InitialInfo) {
                m_InitialInfo->setEventID(-1);
                m_InitialInfo->setFrameExposureTime(0.0);
                m_InitialInfo->setRealTime(m_CurrentTime);
                m_InitialInfo->setDelayTime(0.0);
                m_InitialInfo->setEndOfDeadResetTime(m_CurrentTime);
                m_InitialInfo->setSelectEventID(-1);
                m_InitialInfo->setSelectEventRealTime(m_CurrentTime);
            }

            if (m_DeadTime > 0.0) {
                m_CurrentTime += m_TimeStep;
                m_DeadTime -= m_TimeStep;
            } else {
                m_CurrentState = State::IDLE;
                m_EndOfDeadResetTime = m_CurrentTime;
            }
            break;
        }

    case State::COLLECTING:
        {
            // 未使用
            break;
        }

    default:
        {
            return AS_QUIT_ERROR;
        }
    }

    return AS_OK;
}

void DefineEventDriven::CreateEventFrame(const std::shared_ptr<DetectorHit>& triggerHit) {
    m_EventFrame.clear();

    if (!triggerHit) {
        return;
    }

    std::shared_ptr<DetectorHit> maxEnergyHit = triggerHit;
    double maxEpi = triggerHit->EPI();

    std::vector<std::shared_ptr<DetectorHit>>& hits = m_HitCollection->getHits(0);
    for (auto& hit : hits) {
        if (IsWithin3x3(triggerHit, hit)) {
            m_EventFrame.push_back(hit);
            double epi_energy = hit->EPI();
            if (epi_energy > maxEpi) {
                maxEpi = epi_energy;
                maxEnergyHit = hit;
            }
        }
    }

    ProcessEventFrame(maxEnergyHit);
}

bool DefineEventDriven::IsWithin3x3(const std::shared_ptr<DetectorHit>& centerHit,
                                    const std::shared_ptr<DetectorHit>& hit) {
    int cx = centerHit->PixelX();
    int cy = centerHit->PixelY();
    int x = hit->PixelX();
    int y = hit->PixelY();

    return (std::abs(cx - x) <= 1) && (std::abs(cy - y) <= 1);
}

void DefineEventDriven::ProcessEventFrame(const std::shared_ptr<DetectorHit>& centerHit) {
    // 3x3の全ヒットに同じeventIDを割り当てる
    for (auto& hit : m_EventFrame) {
        hit->setEventID(m_EventIndex - 1); // m_EventIndexがインクリメント後なので -1
    }

    // ================================
    // ▼▼ 追加: 3x3中心イベント情報をセット ▼▼
    // ================================
    if (centerHit && m_InitialInfo) {
        // 3x3中心イベントの eventid (Hit側)
        auto center_eventid = centerHit->EventID();

        // 例: もし「生成器で与えた初期エネルギー」を紐付けたいなら
        //     m_InitialInfo->InitialEnergy() を使う
        double ini_energy = m_InitialInfo->InitialEnergy();

        // InitialInformation に書き込む
        m_InitialInfo->setCenterEventID(center_eventid);
        m_InitialInfo->setCenterIniEnergy(ini_energy);

        // デバッグ:
        // std::cerr << "[ProcessEventFrame] center_eventid=" << center_eventid
        //           << ", center_ini_energy=" << ini_energy << std::endl;
    }
    // ================================

    // （デバッグ用）
//  std::cerr << "Processed EventFrame: Number of Hits = " << m_EventFrame.size()
//            << ", FrameExposureTime = " << m_FrameExposureTime << " s" << std::endl;
}

ANLStatus DefineEventDriven::mod_finalize() {
    // 必要に応じて後処理
    return AS_OK;
}

} // namespace comptonsoft
