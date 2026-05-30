#include "AuraLogChannels.h"

/**
 * 定义（分配存储）LogAura 这个日志通道。
 *
 * 注意：
 *  - DECLARE_LOG_CATEGORY_EXTERN 只是“声明”，不会真正生成变量。
 *  - DEFINE_LOG_CATEGORY 必须且只能在一个 .cpp 里出现一次，否则会出现重复定义链接错误。
 *
 * 一般做法：
 *  - 项目里统一放一个 LogChannels 文件（本文件）集中定义所有自定义通道。
 */
DEFINE_LOG_CATEGORY(LogAura);