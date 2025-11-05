#include "../include/Commands.hpp"

// ここにコマンドの実装を書いていって、include "../include/Commands.hpp"するだけでライブラリのように使えるようにしたい
//　ここに全部書くというより、いい感じに１ファイルに１関数作ってめっちゃここでimportする方がわかりやすい気がする。

// main dispatch
void dispatch(Server&, Client&)
{

}

// 各コマンドの実装(dispatch内で使う)