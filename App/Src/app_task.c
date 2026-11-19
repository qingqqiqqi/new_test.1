#include "app_task.h"

#include "bsp_vision.h"

/* ------------------------------------------------------------------
 * vision_stage_task：视觉阶段状态机
 *   底盘到隧道前（chassis_state == 2）时，
 *   把 g_vision_stage 从 1 切到 2，并连发 3 次通知视觉切模型。
 *   靠 g_vision_stage == 1 这个条件保证"只发一次"。
 * ------------------------------------------------------------------ */
void vision_stage_task(uint8_t chassis_state)
{
 if (g_vision_stage == 1 && chassis_state == 2)
 {
  g_vision_stage = 2;

  bsp_vision_send_cmd(g_vision_stage);
  bsp_vision_send_cmd(g_vision_stage);
  bsp_vision_send_cmd(g_vision_stage);
 }
}

/* ------------------------------------------------------------------
 * app_task_10ms：整车 10ms 周期任务总入口
 *   所有周期性业务在这里按顺序排。现在只有视觉阶段，
 *   以后接底盘、机械臂就往下面加。
 * ------------------------------------------------------------------ */
void app_task_10ms(void)
{
 /* 底盘状态量：还没接底盘，先写死 0（永远不切）
  * 接上底盘后改成底盘组给的变量，比如：chassis_get_state() */
 static uint8_t chassis_state = 0;

 vision_stage_task(chassis_state);
}