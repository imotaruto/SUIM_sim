#include "TGraphErrors.h"
#include "TFile.h"
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

int main() {
    // データ格納用のコンテナ
    vector<double> x, y, ey;
    double xx, yy, err;

    // データファイルを開く（data.txt にデータがあるものとする）
    ifstream infile("cxb_2detector.txt");
    if (!infile.is_open()) {
        cerr << "Error: data.txt が開けません" << endl;
        return 1;
    }
    
    // ファイルから1行ずつ読み込み
    while (infile >> xx >> yy >> err) {
        x.push_back(xx);
        y.push_back(yy);
        ey.push_back(err);
    }
    infile.close();

    int n = x.size();
    // x の配列はそのままでよく、x の誤差は 0 とする
    vector<double> ex(n, 0.0);

    // TGraphErrors の作成（引数はデータ点数、x配列、y配列、x誤差配列、y誤差配列）
    TGraphErrors *graph = new TGraphErrors(n, &x[0], &y[0], &ex[0], &ey[0]);

    // ROOT ファイルに保存
    TFile *outfile = new TFile("cxb_2dec_graph.root", "RECREATE");
    graph->Write("myGraph"); // "myGraph" という名前で保存
    outfile->Close();

    cout << "graph.root に TGraphErrors が保存されました。" << endl;
    return 0;
}

