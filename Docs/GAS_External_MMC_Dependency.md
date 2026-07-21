# GAS External MMC Dependency Notes

## 背景

在 Aura 项目里，`MMC_MaxHealth` 和 `MMC_MaxMana` 的公式依赖 `PlayerLevel`：

```cpp
MaxHealth = 80 + 2.5 * Vigor + 10 * PlayerLevel
MaxMana = 50 + 2.5 * Intelligence + 15 * PlayerLevel
```

其中 `Vigor`、`Intelligence` 是 GAS 捕获的 Attribute，变化时 GAS 会自动让相关 MMC 重新计算。

但 `PlayerLevel` 存在 `PlayerState` 中，不是 `AttributeSet` 里的 Attribute。MMC 只是通过 `ICombatInterface::GetPlayerLevel` 临时读取它，所以 GAS 不知道 `PlayerLevel` 是 `MaxHealth/MaxMana` 的依赖项。

结果是：单纯升级改变 `PlayerState.Level` 时，`MaxHealth/MaxMana` 不一定会立刻重新计算。

## 解决思路

Unreal 的 `UGameplayModMagnitudeCalculation` 提供了外部依赖刷新入口：

```cpp
virtual FOnExternalGameplayModifierDependencyChange* GetExternalModifierDependencyMulticast(
    const FGameplayEffectSpec& Spec,
    UWorld* World) const override;
```

它的作用是让 MMC 告诉 GAS：

```text
除了 captured Attribute，我还依赖一个外部事件。
当这个 delegate Broadcast 时，请重新计算使用这个 MMC 的 Active GameplayEffect。
```

## 当前实现

`ICombatInterface` 暴露一个返回外部依赖委托的函数：

```cpp
virtual FOnExternalGameplayModifierDependencyChange* GetExternalGameplayModifierDependencyMulticast()
{
    return nullptr;
}
```

`AAuraCharacterBase` 持有实际的委托：

```cpp
FOnExternalGameplayModifierDependencyChange ExternalGameplayModifierDependencyMulticast;
```

并返回给 MMC：

```cpp
FOnExternalGameplayModifierDependencyChange* AAuraCharacterBase::GetExternalGameplayModifierDependencyMulticast()
{
    return &ExternalGameplayModifierDependencyMulticast;
}
```

`MMC_MaxHealth` 和 `MMC_MaxMana` override `GetExternalModifierDependencyMulticast`，从 `Spec.GetContext().GetSourceObject()` 上拿到 `ICombatInterface`，然后返回角色的外部依赖委托。

## 升级流程

升级时：

```text
1. 增加 PlayerState.Level
2. 设置 bTopOffHealth / bTopOffMana
3. Broadcast ExternalGameplayModifierDependencyMulticast
4. GAS 让相关 Active GameplayEffect 重新计算 MMC
5. MaxHealth / MaxMana 更新后触发 PostAttributeChange
6. PostAttributeChange 中回满 Health / Mana
```

对应代码：

```cpp
bTopOffHealth = true;
bTopOffMana = true;

ExternalDelegate->Broadcast();
```

`PostAttributeChange` 负责在上限变化后补满：

```cpp
if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
{
    SetHealth(GetMaxHealth());
    bTopOffHealth = false;
}
```

## Fallback

如果广播后 `MaxHealth/MaxMana` 没有变化，`PostAttributeChange` 不会触发。为了避免升级不回血，升级逻辑里还会检查 flag：

```cpp
if (bTopOffHealth)
{
    SetHealth(GetMaxHealth());
    bTopOffHealth = false;
}
```

这样至少能回满到当前已经计算好的上限。

## 适用场景

这种方式适合处理“不在 GAS AttributeSet 里，但会影响 MMC 结果”的外部数据，例如：

- PlayerState.Level
- 装备属性
- 天赋树加成
- 队伍光环
- 外部配置或状态

当这些外部数据变化时，广播对应 delegate，即可让相关 MMC 重新计算。

## 和把 Level 做成 Attribute 的区别

更体系化的方案是把 `Level` 也做成 GAS Attribute，并在 MMC 中正式捕获它。这样 GAS 能天然感知 Level 变化。

当前方案改动更小，适合保留现有 `PlayerState.Level` 结构，同时让 MMC 能被显式刷新。
