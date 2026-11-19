#ifndef ALG_VISION_H
#define ALG_VISION_H

#include <stdint.h>

#define VISION_FRAME_LEN   11

typedef struct
{
    uint8_t  type;    /* 目标类型，0=无目标 */
    uint16_t x;       /* 目标中心 x，像素 */
    uint16_t y;       /* 目标中心 y，像素 */
    uint8_t  conf;    /* 置信度 0~100 */
    uint8_t  light;   /* 0未知 1红 2绿 3黄 */
    uint8_t  plate;   /* 0未识别 1/2/3 */
} VisionFrame_t;

/* 解析：从一串字节里找出一帧，成功返回 1 */
uint8_t alg_vision_parse(const uint8_t *buf, uint16_t len, VisionFrame_t *out);

/* 闸门：按 stage 只放行需要的字段，其余清 0 */
void alg_vision_filter(const VisionFrame_t *in, uint8_t stage, VisionFrame_t *out);

#endif