#!/bin/bash
# 番茄时钟GitHub推送脚本

echo "🚀 番茄时钟GitHub推送配置"
echo "==========================="
echo "仓库地址: https://github.com/cpptips/qt-promodoro"
echo ""

# 检查是否已初始化Git仓库
if [ ! -d ".git" ]; then
    echo "📦 初始化Git仓库..."
    git init
fi

# 添加所有文件到暂存区
echo "📝 添加文件到暂存区..."
git add .

# 检查是否有未提交的更改
if git diff --cached --quiet; then
    echo "ℹ️  没有需要提交的更改"
else
    echo "💾 提交代码..."
    git commit -m "feat: 番茄时钟Qt应用 - 完整功能版本
    
    - 🎯 功能丰富的桌面番茄时钟应用
    - 🪟 支持浮动窗口实时倒计时
    - 📝 主题记录和导出功能
    - 🎨 深色/浅色主题切换
    - 📌 系统托盘集成
    - 🔧 完整的项目文档和安装脚本"
fi

# 设置远程仓库
echo "🔗 设置GitHub远程仓库..."
git remote remove origin 2>/dev/null
git remote add origin https://github.com/cpptips/qt-promodoro.git

echo ""
echo "📋 推送选项："
echo "1. 推送到现有分支"
echo "2. 推送到新分支"
read -p "请选择 (1/2): " push_option

case $push_option in
    1)
        echo "📤 推送到main分支..."
        git push -u origin main
        ;;
    2)
        read -p "请输入新分支名称: " branch_name
        echo "🌿 创建并推送到分支: $branch_name"
        git checkout -b $branch_name
        git push -u origin $branch_name
        ;;
    *)
        echo "❌ 无效选项"
        exit 1
        ;;
esac

if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 推送成功！"
    echo "🌐 访问: https://github.com/cpptips/qt-promodoro"
    echo ""
    echo "📋 后续操作建议："
    echo "  • 添加项目描述和标签"
    echo "  • 设置README为项目首页"
    echo "  • 添加开源许可证徽章"
    echo "  • 配置GitHub Actions自动化构建"
else
    echo "❌ 推送失败，请检查网络连接和仓库权限"
fi