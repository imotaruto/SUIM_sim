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

#ifndef COMPTONSOFT_MYTEXTOUTPUTMODULE_H
#define COMPTONSOFT_MYTEXTOUTPUTMODULE_H 1

#include <anlnext/BasicModule.hh>
#include <fstream>
#include <string>

namespace comptonsoft {

  /**
   * MyTextOutputModule
   *
   * シミュレーション中の各イベントで、BasicComptonEvent に含まれる
   * ヒット情報（Hit1, Hit2）をテキストファイル (my_output_hits.txt) に出力するモジュールです.
   */
  class MyTextOutputModule : public anlnext::BasicModule {
    DEFINE_ANL_MODULE(MyTextOutputModule, 1.0);
  public:
    // コンストラクタは引数を BasicModule に渡さず、必要なら内部で名前を設定する
    MyTextOutputModule(const std::string &name = "MyTextOutputModule");
    virtual ~MyTextOutputModule();

    virtual anlnext::ANLStatus mod_define() override;
    virtual anlnext::ANLStatus mod_begin_run() override;
    virtual anlnext::ANLStatus mod_analyze() override;
    virtual anlnext::ANLStatus mod_end_run() override;

  private:
    std::ofstream* file_;
  };

} // namespace comptonsoft

#endif // COMPTONSOFT_MYTEXTOUTPUTMODULE_H
