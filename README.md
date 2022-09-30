# EMUZ80-8085

![MEZ8085](https://github.com/satoshiokue/EMUZ80-8085/blob/main/imgs/IMG_8085_proto1.jpeg)  
8085 Single-Board Computer

μPD8085AHC  

Maximum clock 1.1MHz(550kHz)

PIC18F47Q84 / PIC18F47Q83

## 回路図
https://github.com/satoshiokue/MEZ8085

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
