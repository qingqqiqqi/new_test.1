#include "bsp_vision.h"
#include "usart.h"
#include "usart.h"
#include "dma.h"
extern DMA_HandleTypeDef hdma_usart2_rx;

/* ============================================================
 * 内部数据（只在 .c 里可见）
 * ============================================================ */
static uint8_t       dma_buf[256];   /* DMA 往里搬字节的地方 */
static VisionFrame_t raw_frame;      /* 解析结果，跨中断共享 */
uint8_t g_vision_stage = 1;

/* ============================================================
 * 内部小工具：开一次接收 + 关半传输中断
 * ============================================================ */
static void vision_start_rx(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, dma_buf, sizeof(dma_buf));
    __HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

/* ============================================================
 * 1. 初始化：main 里调一次
 * ============================================================ */
void bsp_vision_init(void)
{
    vision_start_rx();
}

/* ============================================================
 * 2. 串口空闲中断：解析 → 存 → 重新开接收
 * ============================================================ */
void bsp_vision_on_idle(uint16_t size)
{
    if (size < VISION_FRAME_LEN)
    {
        vision_start_rx();      /* 先重启接收，再退出 */
        return;
    }

    alg_vision_parse(dma_buf, size, &raw_frame);

    vision_start_rx();          /* 无论解析成功与否都必须重启 */
}

/* ============================================================
 * 3. 安全读取（关中断快照）
 * ============================================================ */
void bsp_vision_get(VisionFrame_t *out)
{
    __disable_irq();
    *out = raw_frame;
    __enable_irq();
}

/* ============================================================
 * 4. 给视觉发 1 字节（阻塞发送，超时 1ms）
 * ============================================================ */
void bsp_vision_send_cmd(uint8_t cmd)
{
    HAL_UART_Transmit(&huart2, &cmd, 1, 1);
}
