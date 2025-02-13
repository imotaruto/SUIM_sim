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

#ifndef ANLGEANT4_INITIALINFORMATION_H
#define ANLGEANT4_INITIALINFORMATION_H 1

#include <cstdint>
#include "G4ThreeVector.hh"

namespace anlnext {
class BasicModule;
}

namespace anlgeant4 {

/**
 * @brief 初期粒子情報を保存するクラス
 */
class InitialInformation
{
public:
    explicit InitialInformation(bool stored, anlnext::BasicModule* mod = nullptr);

    // 初期情報のストアフラグ
    bool InitialInformationStored() const { return stored_; }
    void setInitialInformationStored(bool v = true) { stored_ = v; }

    // 重みのストアフラグ
    bool WeightStored() const { return weight_stored_; }
    void setWeightStored(bool v = true) { weight_stored_ = v; }

    // 初期エネルギー
    double InitialEnergy() const { return energy_; }
    void setInitialEnergy(double v) { energy_ = v; }

    // 初期方向
    G4ThreeVector InitialDirection() const { return direction_; }
    void setInitialDirection(G4ThreeVector v) { direction_ = v; }
    void setInitialDirection(double x, double y, double z) { direction_.set(x, y, z); }

    // 初期時間
    double InitialTime() const { return time_; }
    void setInitialTime(double v) { time_ = v; }

    // 初期位置
    G4ThreeVector InitialPosition() const { return position_; }
    void setInitialPosition(G4ThreeVector v) { position_ = v; }
    void setInitialPosition(double x, double y, double z) { position_.set(x, y, z); }

    // 初期偏光
    G4ThreeVector InitialPolarization() const { return polarization_; }
    void setInitialPolarization(G4ThreeVector v) { polarization_ = v; }
    void setInitialPolarization(double x, double y, double z) { polarization_.set(x, y, z); }

    // イベントID
    int64_t EventID() const { return event_id_; }
    void setEventID(int64_t i) { event_id_ = i; }

    // 重み
    double Weight() const { return weight_; }
    void setWeight(double v) { weight_ = v; }

    // リアルタイム
    double RealTime() const { return real_time_; }
    void setRealTime(double t) { real_time_ = t; }

    // フレームごとの露光時間
    double FrameExposureTime() const { return frame_exposure_time_; }
    void setFrameExposureTime(double t) { frame_exposure_time_ = t; }

    // 遅延時間の設定と取得
    double DelayTime() const { return delay_time_; }
    void setDelayTime(double t) { delay_time_ = t; }

    // デッドタイム終了時刻
    double EndOfDeadResetTime() const { return end_of_dead_reset_time_; }
    void setEndOfDeadResetTime(double t) { end_of_dead_reset_time_ = t; }

    // 選択イベントID
    int SelectEventID() const { return select_event_id_; }
    void setSelectEventID(int id) { select_event_id_ = id; }

    // 選択イベントRealTime
    double SelectEventRealTime() const { return select_event_real_time_; }
    void setSelectEventRealTime(double t) { select_event_real_time_ = t; }

    //---------------------------------------------------------
    // ▼▼▼ ここから新規追加: 3x3中心イベント情報 ▼▼▼
    //---------------------------------------------------------
    int64_t CenterEventID() const { return center_event_id_; }
    void setCenterEventID(int64_t v) { center_event_id_ = v; }

    double CenterIniEnergy() const { return center_ini_energy_; }
    void setCenterIniEnergy(double v) { center_ini_energy_ = v; }
    //---------------------------------------------------------

private:
    bool stored_;
    bool weight_stored_;

    double energy_;
    G4ThreeVector direction_;
    double time_;
    G4ThreeVector position_;
    G4ThreeVector polarization_;
    int64_t event_id_;
    double weight_;

    // リアルタイムを保持
    double real_time_;

    // フレームごとの露光時間を保持
    double frame_exposure_time_;

    // 遅延時間を保持
    double delay_time_;

    // デッドタイム終了時刻
    double end_of_dead_reset_time_;

    // 選択イベントのIDとReal Time
    int select_event_id_;
    double select_event_real_time_;

    //---------------------------------------------------------
    // 新規追加: 3x3中心イベントの eventid と 初期粒子エネルギー
    //---------------------------------------------------------
    int64_t center_event_id_;
    double center_ini_energy_;
};

} /* namespace anlgeant4 */

#endif /* ANLGEANT4_INITIALINFORMATION_H */
