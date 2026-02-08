#!/bin/bash
# 番茄时钟Qt应用安装脚本
# 适用于macOS系统

echo "🍅 番茄时钟Qt应用安装脚本"
echo "==========================="

# 检查是否安装了Qt
if ! command -v qmake &> /dev/null; then
    echo "❌ 未检测到Qt环境，请先安装Qt6"
    echo ""
    echo "推荐使用Homebrew安装："
    echo "brew install qt"
    echo ""
    echo "或从Qt官网下载安装："
    echo "https://www.qt.io/download"
    exit 1
fi

echo "✅ 检测到Qt环境：$(qmake --version | head -1)"

# 编译项目
echo ""
echo "🔨 正在编译项目..."
qmake && make

if [ $? -eq 0 ]; then
    echo "✅ 编译成功！"
else
    echo "❌ 编译失败，请检查错误信息"
    exit 1
fi

# 创建启动脚本
echo ""
echo "📝 创建启动脚本..."
cat > run_pomodoro.sh << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
./qt_pomodoro.app/Contents/MacOS/qt_pomodoro
EOF

chmod +x run_pomodoro.sh

# 创建快速启动脚本（后台运行）
cat > start_pomodoro.sh << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
nohup ./qt_pomodoro.app/Contents/MacOS/qt_pomodoro &
echo "🍅 番茄时钟已启动（PID: $!）"
echo "💡 提示：可通过系统托盘图标访问程序"
EOF

chmod +x start_pomodoro.sh

echo ""
echo "🎉 安装完成！"
echo ""
echo "📋 可用命令："
echo "  ./run_pomodoro.sh    - 前台运行程序"
echo "  ./start_pomodoro.sh  - 后台运行程序"
echo "  qmake && make        - 重新编译"
echo ""
echo "🌟 功能特性："
echo "  • 智能番茄计时（25分钟工作 + 5分钟休息）"
echo "  • 浮动窗口实时倒计时"
echo "  • 主题记录和导出功能"
echo "  • 深色/浅色主题切换"
echo "  • 系统托盘集成"
echo ""
echo "开始高效工作吧！🚀"