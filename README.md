# Linux-Kernel-Module-Template

## 概要
Linuxカーネルモジュールのテンプレート。
モジュールのロード/アンロード時に，
カーネルのメッセージバッファに文字列を出力する。

## 使用方法

1. Linuxヘッダをダウンロード（sudo apt-get install linux-headers-3.13.0-100-generic）
1. Makefileの先頭行をダウンロード先に合わせて変更
1. makeを実行
1. モジュールをロード（sudo insmod hello.ko） 
1. モジュールをアンロード（sudo rmmod hello）
