# 适用于STM32F429IGT6(HAL)的LVGL空白例程
## 简介
本例程基于STM32F429IGT6，使用HAL库开发，且已经移植8.5版本LVGL GUI。工程部分外设使用STM32 CubeMX初始化。
## 外设使用注意事项
### 时钟
使用CubeMX时钟树初始化，外部HSE为25MHz，sysclk为180MHz，LTDCclk为33MHz。具体参见ioc文件。
### ADC
使用CubeMX初始化，已开放ADC1，通道未配置。
### LTDC
客制化驱动，未通过cubemx配置，在Cubemx中配置GPIO时应注意IO占用，具体见lcd.c和ltdc.c。

*注意*：若使用IAR EW编译，ltdc.c中在定义帧缓存地址时应采用以下写法：
```
#if LTDC_PIXFORMAT == LTDC_PIXFORMAT_ARGB8888 || LTDC_PIXFORMAT == LTDC_PIXFORMAT_RGB888
    uint32_t ltdc_lcd_framebuf[1280][800] __attribute__((at(LTDC_FRAME_BUF_ADDR)));   
#else
    #pragma location = LTDC_FRAME_BUF_ADDR          /* 注意：这种写法只在IAR EW中有效，使用MDK-ARM时参见手册 */
    uint16_t ltdc_lcd_framebuf[1280][800];          /* 注意：未正确转译该语句可能导致SDRAM无法访问！ */
//    uint16_t ltdc_lcd_framebuf1[1280][800] __attribute__((at(LTDC_FRAME_BUF_ADDR + 1280 * 800 * 2)));   
#endif
```
不能采用`__attribute__`的方式进行内存分配，否则报错。
### TIM
默认开启TIM6，PSC为`10-1`，ARR为`9000-1`。当TIM6为LVGL提供timebase时需开启TIM6的NVIC，并且于tim.c中重写中断服务函数：
```
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &htim6)
  {
    lv_tick_inc(1);   /* 1ms心跳 */
  }
}
```
