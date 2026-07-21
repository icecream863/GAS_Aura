# SpellMenu InputTag 问题总结

## 现象

SpellMenu 中运行时用蓝图 `Set InputTag` 设置失败，但在细节面板手动设置 `InputTag` 正常。

## 根因

蓝图里存在两个相似变量：

```text
InputTag
Input Tag
```

运行时 Set 到了其中一个变量，但真正用于 `AbilityInfoDelegate` 匹配的是另一个变量。

## 为什么细节面板正常

细节面板设置的是实际参与匹配的那个变量，所以控件创建时就带有正确的 `InputTag`。

## 结论

问题不是广播顺序、委托绑定或 GAS 数据错误，而是同名/近似同名变量导致 Set 错变量。

## 修复建议

只保留一个明确变量，例如：

```text
InputTag
```

并确保以下位置全部使用同一个变量：

```text
Set InputTag
Get InputTag
AbilityInfoDelegate 匹配逻辑
细节面板默认值
父控件传给子控件
```
