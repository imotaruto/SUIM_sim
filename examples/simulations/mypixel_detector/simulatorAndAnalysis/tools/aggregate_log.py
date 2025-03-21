#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
このスクリプトは、ComptonSoftのログファイル（log.txt）から
各イベントごとに、シールド側での散乱（検出器以外での散乱）と
検出器内（SiLayer1）での散乱を集計する例です。

【前提】
- ログはテキストファイル（例：log.txt）として保存されている。
- 各イベントは「G4PrimaryTransformer::PrimaryVertex」で始まる行で始まるブロックとする。
- 各ステップは、ヘッダー行に示されるフォーマットに従い、
  10個程度の項目があり、tokens[8]がNextVolume、tokens[9]がProcNameとする。
- SiLayer1が検出器であり、それ以外（例：SiLayer3, SiO2Layer, World.log_PVなど）はシールドやその他とみなす。

【動作】
- ファイルを1行ずつ読み込み、"G4PrimaryTransformer::PrimaryVertex"でイベントブロックを抽出（ストリーミング方式）。
- 各イベントブロック内のステップ行を解析し、
  ・SiLayer1に入る前に散乱プロセス（compt, phot, eioni, msc）が発生していれば「シールド散乱」
  ・SiLayer1内で散乱プロセスが発生していれば「検出器内散乱」
  ・さらに、シールド領域でのコンプトン散乱（"compt"）と検出器内での光電吸収（"phot"）が起こったイベントを個別にカウントする
- 各イベント毎にカテゴリをカウントし、最終的に件数を出力する
"""

import re

def iter_events(filename):
    """
    ファイルを1行ずつ読み込み、"G4PrimaryTransformer::PrimaryVertex"で始まる各イベントブロックを
    ストリーミング方式で生成するジェネレータ
    """
    current_event = None
    with open(filename, 'r', encoding='utf-8') as f:
        for line in f:
            if line.startswith("G4PrimaryTransformer::PrimaryVertex"):
                if current_event is not None:
                    yield current_event
                current_event = line  # 新たなイベントの開始
            else:
                if current_event is not None:
                    current_event += line
        if current_event is not None:
            yield current_event

def analyze_event(event_text):
    """
    1つのイベントブロック内で、各ステップ行を解析し、
      ・SiLayer1に入る前に発生した散乱（shield_scattering）
      ・SiLayer1内で発生した散乱（detector_scattering）
      ・シールド領域でのコンプトン散乱（shield_compt）
      ・検出器内での光電吸収（detector_phot）
    のフラグを返す
    """
    shield_scattering = False
    detector_scattering = False
    first_detector_encountered = False

    shield_compt = False    # シールド側で"compt"が発生したか
    detector_phot = False   # 検出器内で"phot"が発生したか

    # 解析対象とする散乱プロセス（必要に応じて追加してください）
    scattering_processes = {"compt", "phot", "eioni", "msc"}

    for line in event_text.splitlines():
        line = line.strip()
        # ステップ行は数字で始まると仮定（例："0   -765   468 ..."）
        if re.match(r'^\d+', line):
            tokens = line.split()
            if len(tokens) < 10:
                continue  # フォーマットが合わない行はスキップ
            # tokens[8] がNextVolume、tokens[9] がProcName
            volume = tokens[8]
            process = tokens[9].lower()  # 小文字に統一

            if volume == "SiLayer1":
                first_detector_encountered = True
                # SiLayer1内での散乱（全般）
                if process in scattering_processes:
                    detector_scattering = True
                # 検出器内での光電吸収をチェック
                if process == "phot":
                    detector_phot = True
            else:
                # 検出器に入る前の領域（シールドなど）
                if not first_detector_encountered:
                    if process in scattering_processes:
                        shield_scattering = True
                    # シールド内でコンプトン散乱かチェック
                    if process == "compt":
                        shield_compt = True

    return shield_scattering, detector_scattering, shield_compt, detector_phot

def aggregate_events(filename):
    """
    ファイル内の全イベントについて、以下のカテゴリごとにカウントする
      - シールドでのみ散乱（shield_scatteringのみ）
      - 検出器内でのみ散乱（detector_scatteringのみ）
      - 両方で散乱（both）
      - いずれも散乱が検出されなかった（none）
    さらに、シールドでコンプトン散乱が起こり、検出器内で光電吸収が起こったイベントもカウントする
    """
    total = 0
    shield_only = 0
    detector_only = 0
    both = 0
    none = 0
    shield_compt_then_detector_phot = 0

    for event in iter_events(filename):
        total += 1
        shield_scattering, detector_scattering, shield_compt, detector_phot = analyze_event(event)
        if shield_scattering and detector_scattering:
            both += 1
        elif shield_scattering:
            shield_only += 1
        elif detector_scattering:
            detector_only += 1
        else:
            none += 1

        # シールドでのコンプトン散乱と検出器内での光電吸収の両方が起こったイベント
        if shield_compt and detector_phot:
            shield_compt_then_detector_phot += 1

    return total, shield_only, detector_only, both, none, shield_compt_then_detector_phot

def main():
    filename = "log.txt"  # ログファイルのパス（適宜変更してください）
    total, shield_only, detector_only, both, none, shield_compt_then_detector_phot = aggregate_events(filename)
    
    print("総イベント数:", total)
    print("シールドでのみ散乱が発生したイベント:", shield_only)
    print("検出器内（SiLayer1）でのみ散乱が発生したイベント:", detector_only)
    print("シールドと検出器の両方で散乱が発生したイベント:", both)
    print("散乱が検出されなかったイベント:", none)
    print("【追加】シールドでコンプトン散乱し、検出器内で光電吸収が起こったイベント:", shield_compt_then_detector_phot)

if __name__ == "__main__":
    main()
