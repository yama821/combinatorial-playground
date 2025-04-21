# Schrijver-SDP

## 概要

以下の論文で行われている計算を行うためのプログラム

> A. Schrijver, "New code upper bounds from the Terwilliger algebra and semidefinite programming," in IEEE Transactions on Information Theory, vol. 51, no. 8, pp. 2859-2866, Aug. 2005, doi: 10.1109/TIT.2005.851748. 


## 使い方

* `generate_sdpa_input.cpp` をコンパイル・実行して SDPA ソルバーへの入力データを生成する
  * 符号の長さ `N` と最小距離 `d` を標準入力に与える

```sell:例
> g++ generate_sdpa_input.cpp -I ../lib/include -o main
> main > sample-n2-d1.dat-s
>> 2 1
```

* NEOS server の SDPA ソルバーにて生成したファイルを submit

## 参考リンク

[1] [SDPA の使い方](http://www.cc.u-ryukyu.ac.jp/~hide/sdpa.pdf)

[2] [NEOS server での SDPA format](https://plato.asu.edu/ftp/sdpa_format.txt)

[3] [半正定値計画問題に対する主双対内点法の実装と実験的解析](https://dl.ndl.go.jp/pid/3144617/1/78)
