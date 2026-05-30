#pragma once


#include "CoreMinimal.h"
#include "Logging/LogMacros.h"

/**
 * 声明一个名为 LogAura 的日志通道。
 *
 * 参数含义（从左到右）：
 *  - LogAura：通道名字（之后 UE_LOG 的第一个参数就写它）
 *  - Log：    编译时默认等级（一般用 Log）
 *  - All：    允许的最高等级（All 表示全部等级都可能出现）
 */
DECLARE_LOG_CATEGORY_EXTERN(LogAura, Log, All);




/**
 * Aura 项目的日志通道（Log Category）声明。
 *
 * UE 的日志体系允许你把日志按“通道/分类”分组：
 *  - 这样在输出窗口/日志文件里更容易过滤（例如只看 LogAura）。
 *  - 也可以在 ini 或控制台里单独调整某个通道的日志等级。
 *
 * 用法概览：
 *  1) 在某个头文件里用 DECLARE_LOG_CATEGORY_EXTERN(...) 声明一个全局日志通道变量。
 *  2) 在某个 .cpp 里用 DEFINE_LOG_CATEGORY(...) 定义它（只定义一次）。
 *  3) 其它任意 .cpp 里 include 这个头文件后，就能写：
 *       UE_LOG(LogAura, Log, TEXT("Hello"));
 *       UE_LOG(LogAura, Warning, TEXT("Something happened: %d"), Value);
 */