# EMUZ80-8085

![MEZ8085](https://github.com/satoshiokue/EMUZ80-8085/blob/main/imgs/IMG_8085_proto1.jpeg)  
8085 Single-Board Computer

電脳伝説さん(@vintagechips)のEMUZ80の信号を組み替え、uPD8085AHCを動作させることができます。  
アドレスマップをSBC8085に準じることで同データパックのサンプルコードを体験する目的で作成しました。  

SBC8080 CPUルーズキット    
https://vintagechips.wordpress.com/2018/06/24/sbc8080-cpuルーズキット/

「インテル8080伝説」を手元に用意することを強く推奨します。  
https://www.amazon.co.jp/dp/4899774532

uPD8085AHCとPIC18F47Q84の組み合わせで動作確認しています。  

Maximum clock 1.1MHz(550kHz)  
PIC18F47Q84 / PIC18F47Q83  


動作確認済みMPU  
uPD8085AHC  

このソースコードはGazelleさんのEMUZ80用main_cpz.cを元に改変してGPLライセンスに基づいて公開するものです。

https://drive.google.com/drive/folders/1NaIIpzsUY3lptekcrJjwyvWTBNHIjUf1

## メザニンボード
https://github.com/satoshiokue/MEZ8085

## 回路図
https://github.com/satoshiokue/MEZ8085/blob/main/MEZ8085.pdf

## ファームウェア

EMUZ80で配布されているフォルダemuz80.X下のmain.cと置き換えて使用してください。
* emuz80_8085.c

## アドレスマップ
```
Memory
ROM   0x0000 - 0x3FFF 16kバイト
RAM   0x8000 - 0x9FFF 8kバイト

I/O
8251通信レジスタ 0x00
8251制御レジスタ 0x01
```
RAMの開始位置はSBC8080に準じています。  
メモリサイズが異なるため、RAMのスタックポインタやワークエリアを確認してください。

## 8080/8085プログラムの格納
インテルHEXデータを配列データ化して配列rom[]に格納するとuPD8085AHCで実行できます。インテルHEXデータを手作業で変換するのもいいかもしれません。

テキスト変換例
```
hex2bin -p 00 MOASORT.HEX
xxd -i -c16 MOASORT.bin > MOASORT.txt
```

使用ツール例[E3V3A / hex2bin]  
https://github.com/E3V3A/hex2bin

## Software UART
![MEZ8085](https://github.com/satoshiokue/EMUZ80-8085/blob/main/imgs/IMG_8085_proto2.jpeg)  
8085 Software UART  

コネクタ CON4 からSID、SOD信号を取り出すことができます。


## 謝辞
思い入れのあるCPUを動かすことのできるシンプルで美しいEMUZ80を開発された電脳伝説さんに感謝いたします。

そしてEMUZ80の世界を発展させている開発者の皆さんから刺激を受けて8085に挑戦しています。

## 参考）EMUZ80
EUMZ80はZ80CPUとPIC18F47Q43のDIP40ピンIC2つで構成されるシンプルなコンピュータです。

![EMUZ80](https://github.com/satoshiokue/EMUZ80-6502/blob/main/imgs/IMG_Z80.jpeg)

電脳伝説 - EMUZ80が完成  
https://vintagechips.wordpress.com/2022/03/05/emuz80_reference  
EMUZ80専用プリント基板 - オレンジピコショップ  
https://store.shopping.yahoo.co.jp/orangepicoshop/pico-a-051.html

## 参考）phemu6809
comonekoさん(@comoneko)さんがEMUZ80にMC6809を搭載できるようにする変換基板とファームウェアphemu6809を発表されました。

![phemu6809](https://github.com/satoshiokue/EMUZ80-6502/blob/main/imgs/IMG_6809.jpeg)

https://github.com/comoneko-nyaa/phemu6809conversionPCB  
phemu6809専用プリント基板 - オレンジピコショップ  
https://store.shopping.yahoo.co.jp/orangepicoshop/pico-a-056.html
