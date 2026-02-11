# 番茄时钟App发布到苹果商店指南

## 📱 2026年重要更新

### SDK要求（2026年4月起生效）
- **iOS/iPadOS**: 必须使用iOS 26 SDK或更高版本
- **macOS**: 必须使用macOS 26 SDK或更高版本
- **工具**: 使用Xcode 26进行构建和测试

### 新增合规要求
- **隐私标签**: 必须详细说明App隐私保护做法
- **辅助功能标签**: 需要说明支持的辅助功能
- **年龄分级系统**: 已更新为更精细的分类

## 📱 准备工作

### 1. 注册苹果开发者账号
- 费用：$99/年
- 注册地址：developer.apple.com
- 需要：苹果ID、信用卡、税务信息

### 2. 检查应用合规性
- 确保应用符合苹果App Store审核指南
- 确认功能完整且无崩溃
- 准备应用截图和描述

## 🔧 技术准备

### 1. 代码签名和证书
```bash
# 需要创建以下证书：
- iOS Distribution Certificate
- App Store Connect API Key
- Provisioning Profile
```

### 2. 应用信息配置
- **Bundle Identifier**: com.yourcompany.qtpomodoro
- **版本号**: 1.0.0
- **构建版本**: 1

### 3. Qt特定配置
```pro
# 在.pro文件中添加
QMAKE_INFO_PLIST = Info.plist

# 创建Info.plist文件定义应用权限和功能
```

## 🛠️ 构建流程

### 1. 静态链接构建（推荐）
```bash
# 安装Qt静态版本
brew install qt --with-static

# 配置静态构建
./configure -static -release -nomake examples -nomake tests

# 构建应用
qmake && make
```

### 2. 动态链接构建（需要包含Qt框架）
```bash
# 使用macdeployqt工具打包
macdeployqt qt_pomodoro.app -dmg
```

## ?? 苹果商店提交清单

### 必需材料
- [ ] 应用图标（1024x1024 PNG）
- [ ] 5张截图（6.5英寸、5.5英寸设备）
- [ ] 应用描述（中英文）
- [ ] 关键词（相关搜索词）
- [ ] 支持URL（技术支持网站）
- [ ] 隐私政策URL（如果收集数据）

### 元数据
- 应用名称：番茄时钟
- 副标题：高效工作计时器
- 分类：效率工具
- 年龄分级：4+

## ?? 提交流程

### 1. 应用商店连接设置
1. 登录App Store Connect
2. 创建新应用
3. 填写应用信息
4. 上传截图和图标

### 2. 使用Xcode Archive
```bash
# 生成归档文件
xcodebuild -workspace QtPomodoro.xcworkspace -scheme QtPomodoro archive

# 导出为IPA文件
xcodebuild -exportArchive -archivePath QtPomodoro.xcarchive -exportOptionsPlist ExportOptions.plist
```

### 3. 上传到App Store Connect
- 使用Application Loader或Xcode
- 验证构建文件
- 提交审核

## ⚠️ 常见问题

### Qt应用特殊要求
1. **框架包含**: 确保所有Qt框架正确打包
2. **权限申请**: 明确说明需要的声音播放权限
3. **沙盒限制**: 确保文件访问符合沙盒规则

### 审核注意事项
- 无内购功能需明确说明
- 托盘功能在iOS上需要替代方案
- 锁屏功能需要用户明确授权

## 🎯 Qt应用iOS发布特别注意事项

### Qt许可协议问题
```
重要：Qt使用GPL协议，非开源项目可能面临审核风险
解决方案：
1. 使用Qt商业许可证（推荐商业项目）
2. 确保代码开源并遵守GPL协议
3. 或考虑使用其他跨平台框架
```

### iOS平台适配要求
1. **系统托盘替代方案**
   - iOS不支持系统托盘，需要重新设计UI
   - 考虑使用推送通知替代托盘功能

2. **锁屏功能适配**
   - 需要明确用户授权说明
   - 可能需要在隐私政策中说明

3. **界面布局重构**
   - 重新设计适应移动端触摸操作
   - 优化小屏幕显示效果

### 构建工具链
```bash
# Qt for iOS构建流程
# 1. 在Qt Creator中配置iOS工具链
# 2. 选择iOS设备目标
# 3. 编译生成.app文件
# 4. 使用Xcode打包为.ipa
```

## 💡 优化建议

### 性能优化
- 减少应用体积（<100MB）
- 优化启动时间
- 内存使用监控

### 用户体验
- 添加Touch ID/Face ID支持
- 适配深色模式
- 支持iPad分屏

## 📞 技术支持

### 资源链接
- [苹果开发者文档](https://developer.apple.com/documentation/)
- [Qt for iOS指南](https://doc.qt.io/qt-6/ios.html)
- [App Store审核指南](https://developer.apple.com/app-store/review/guidelines/)

### 预计时间线
1. 准备材料：1-2周
2. 审核等待：1-3天
3. 上架发布：立即

---
*注意：首次提交可能需要多次修改和重新提交，建议预留充足时间。*