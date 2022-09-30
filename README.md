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

## アドレスマップ
```
Memory
ROM   0x0000 - 0x3FFF 32kバイト
RAM   0x8000 - 0x9FFF 8kバイト

I/O
8251通信レジスタ 0x00
8251制御レジスタ 0x01
```

## Software UART
![MEZ8085](https://github.com/satoshiokue/EMUZ80-8085/blob/main/imgs/IMG_8085_proto2.jpeg)  
8085 Software UART  

コネクタ CON4 からSID、SOD信号を取り出すことができます。
