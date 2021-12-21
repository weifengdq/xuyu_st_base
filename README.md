# xuyu_st_base



## hardware

如下:

- MCU: STM32H743VIT6, LQFP100, 2MB Flash, 1MB RAM, 480 MHz

- 外部晶振 25MHz
- 指示灯: LED0~5(PE2, PE3, PE4, PE5, PA15, PC10), 偶数红灯, 奇数绿灯, 低电平点亮, 用于电源和CAN总线状态指示
- USB, PA11(DP), PA12(DM), 暂时用于虚拟串口的调试
- ETH, 百兆网口, PHY型号LAN8720
- CANFD

## software

STM32CubeMX 6.4.0

FW.H7.1.9.1



CANFD:

- 80MHz时钟
- 仲裁段: 80M/500K=160, NTseg1=160*0.8-1=127, NTseg2=NSJW=160-127-1=32
- 数据段: 80M/2M=40, DTseg1=40*0.8-1=31, DTseg2=DSJW=40-31-1=8
- Message Ram 2560 Words 对半分, 各自 1280 Words
- 标准帧滤波器 + 扩展帧滤波器 + RxFIFO0 + TxFIFO
- 64x1 + 64x2 + 28x18 + 32x18 = 1272 < 1280, ok
- GPIO速率改High, 开interrput0中断

组播每次Cube生成都要添加

```bash
//stm32h7xx_hal_eth.c的 ETH_MACDMAConfig() 函数添加
  //开启混杂模式, PassAllMulticast保持默认的就可以
  ETH_MACFilterConfigTypeDef pFilterConfig;
  HAL_ETH_GetMACFilterConfig(heth, &pFilterConfig);
  pFilterConfig.PromiscuousMode = ENABLE;
  HAL_ETH_SetMACFilterConfig(heth, &pFilterConfig);
  /*--------------- ETHERNET MAC registers default Configuration --------------*/
```

如果不分配IP地址或者DHCP获取不到IP地址, 则组播数据发布出来?
