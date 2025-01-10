/**
 * @file lv_port_disp_templ.c
 *
 */

 /*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp_template.h"
#include "../../lvgl.h"
/* LCD驱动 */
#include "lcd.h"

/*********************
 *      DEFINES
 *********************/
#define USE_SRAM        0       
#ifdef USE_SRAM
#endif

/* 屏幕分辨率 */
#define MY_DISP_HOR_RES (1280)   /* 屏幕宽度 */
#define MY_DISP_VER_RES (800)    /* 屏幕高度 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * @brief       LCD加速绘制
 * @param       (sx,sy),(ex,ey):填充矩形对角坐标，区域大小:(ex - sx + 1) * (ey - sy + 1)
 * @param       color:待填充颜色
 * @retval      无
 */
void lcd_draw_fast_rgb_color(int16_t sx, int16_t sy,int16_t ex, int16_t ey, uint16_t *color)
{
    uint16_t w = ex-sx+1;
    uint16_t h = ey-sy+1;

    lcd_set_window(sx, sy, w, h);
    uint32_t draw_size = w * h;
    lcd_write_ram_prepare();

    for(uint32_t i = 0; i < draw_size; i++)
    {
        lcd_wr_data(color[i]);
    }
}

/**
 * @brief       初始化并注册显示设备
 * @param       无
 * @retval      无
 */
void lv_port_disp_init(void)
{
    /*-------------------------
     * 初始化显示设备
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * 创建一个绘图缓冲区
     *----------------------------*/

    /* 单缓冲区 */
    static lv_disp_draw_buf_t draw_buf_dsc_1;
#if USE_SRAM
    static lv_color_t buf_1 = mymalloc(SRAMEX, MY_DISP_HOR_RES * MY_DISP_VER_RES);              
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * MY_DISP_VER_RES);     
#else
    static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                                              
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);                  
#endif

    /*-----------------------------------
     *  LVGL 注册显示设备
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                  /* 显示设备的描述符 */
    lv_disp_drv_init(&disp_drv);                    /* 初始化为默认值 */

    /* 显示设备分辨率，动态获取 */
    disp_drv.hor_res = lcddev.width;
    disp_drv.ver_res = lcddev.height;

    disp_drv.flush_cb = disp_flush;

    disp_drv.draw_buf = &draw_buf_dsc_1;

    /* 注册显示设备 */
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief       初始化显示设备和必要的外围设备
 * @param       无
 * @retval      无
 */
static void disp_init(void)
{
    /*You code here*/
    lcd_init();         /* 初始化LCD */
    lcd_display_dir(1); /* 设置横屏 */
}

/**
 * @brief       内部缓冲区内容刷新至显示屏上特定区域
 *   @note      可使用 DMA 在后台加速
 *              但是刷新完成后必须调用 'lv_disp_flush_ready()'
 *
 * @param       disp_drv    : 显示设备
 *   @arg       area        : 待刷新区域
 *   @arg       color_p     : 颜色数组
 *
 * @retval      无
 */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lcd_color_fill(area->x1, area->y1, area->x2, area->y2, (uint16_t *)color_p);
    
    /* 通知GUI刷新完毕 */
    lv_disp_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
