#ifndef BSP_VISION_H
#define BSP_VISION_H

#include <stdint.h>
#include "alg_vision.h"

/* 初始化：main 里调用一次，开始接收视觉数据 */
void bsp_vision_init(void);

/* 串口空闲中断里调用：解析一帧 → 存起来 → 重新开接收 */
void bsp_vision_on_idle(uint16_t size);

/* 安全读取最新一帧（内部关中断快照，防止读到半新半旧的数据） */
void bsp_vision_get(VisionFrame_t *out);

/* 给视觉发(1)字节指令 */
void bsp_vision_send_cmd(uint8_t cmd);

extern uint8_t g_vision_stage;

#endif