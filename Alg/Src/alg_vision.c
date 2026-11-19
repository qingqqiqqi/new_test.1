#include "alg_vision.h"

#include <stdint.h>

/* ============================================================
 * 解析：从 buf 开头的 len 个字节里找出一帧
 * 返回 1 = 成功，0 = 没找到
 * ============================================================ */
uint8_t alg_vision_parse(const uint8_t *buf, uint16_t len, VisionFrame_t *out)
{
    uint16_t i;
    uint8_t  k;
    uint8_t  sum;

    /* 第 1 块：长度不够一帧，直接放弃 */
    if (len < VISION_FRAME_LEN)
    {
        return 0;
    }

    /* 第 2 块：从左往右滑，每一格都试一次
     * 上界是 len - 11：保证 buf[i + 10] 不越过 len 的边界 */
    for (i = 0; i <= (uint16_t)(len - VISION_FRAME_LEN); i++)
    {
        /* 第 3 块：不是帧头就跳过这一格 */
        if (buf[i] != 0xAA || buf[i + 1] != 0x55)
        {
            continue;
        }

        /* 第 4 块：算前 10 个字节的累加和 */
        sum = 0;
        for (k = 0; k < 10; k++)
        {
            sum += buf[i + k];
        }

        /* 校验和不对，跳过这一格，继续往后找 */
        if (sum != buf[i + 10])
        {
            continue;
        }

        /* 第 5 块：到这儿一定是合法帧，拆 6 个字段 */
        out->type  = buf[i + 2];
        out->x     = (uint16_t)buf[i + 3] | ((uint16_t)buf[i + 4] << 8);
        out->y     = (uint16_t)buf[i + 5] | ((uint16_t)buf[i + 6] << 8);
        out->conf  = buf[i + 7];
        out->light = buf[i + 8];
        out->plate = buf[i + 9];

        return 1;
    }

    /* 全扫完也没找到 */
    return 0;
}

/* ============================================================
 * 闸门：按 stage 只放行该阶段的字段，其余清 0
 * ============================================================ */
void alg_vision_filter(const VisionFrame_t *in, uint8_t stage, VisionFrame_t *out)
{
    *out = *in;

    if (stage == 1)
    {
        /* 看牌阶段：只信 plate */
        out->light = 0;
    }
    else if (stage == 2)
    {
        /* 看灯阶段：只信 light */
        out->plate = 0;
    }
}