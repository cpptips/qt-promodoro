#include "mainwindow.h"
#include "floating_timer.h"
#include "reminder_dialog.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QFont>
#include <QInputDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QSoundEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), timer(new QTimer(this)),
      isWorkPhase(true), completedCycles(0), isDarkTheme(false),
      volume(0.5f), // 默认音量50%
      settings(new QSettings("PomodoroApp", "QtPomodoro", this)) {
  ui->setupUi(this);
  floatingTimer = new FloatingTimer(this);

  // 从设置加载配置
  loadSettings();

  // 初始化UI
  setWindowTitle("番茄时钟");
  ui->timeLabel->setText(
      QString("%1:00").arg(workDuration / 60, 2, 10, QChar('0')));

  // 如果有主题，显示主题内容；否则显示"工作阶段"
  QString displayText =
      currentSessionTheme.isEmpty() ? "工作阶段" : currentSessionTheme;
  ui->phaseLabel->setText(displayText);

  ui->startButton->setText("开始");
  ui->pauseButton->setText("暂停");
  ui->resetButton->setText("重置");
  ui->cycleLabel->setText(QString("已完成周期: %1").arg(completedCycles));
  ui->themeButton->setText(isDarkTheme ? "浅色主题" : "深色主题");
  ui->autoLockCheckBox->setChecked(enableAutoLock);

  // 连接信号和槽
  connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
  connect(ui->startButton, &QPushButton::clicked, this,
          &MainWindow::onStartButtonClicked);
  connect(ui->pauseButton, &QPushButton::clicked, this,
          &MainWindow::onPauseButtonClicked);
  connect(ui->resetButton, &QPushButton::clicked, this,
          &MainWindow::onResetButtonClicked);
  connect(ui->settingsButton, &QPushButton::clicked, this,
          &MainWindow::onSettingsButtonClicked);
  connect(ui->themeButton, &QPushButton::clicked, this,
          &MainWindow::onThemeChanged);
  connect(ui->floatingButton, &QPushButton::clicked, this,
          &MainWindow::toggleFloatingWindow);
  connect(ui->resetPositionButton, &QPushButton::clicked, this,
          &MainWindow::resetFloatingWindowPosition);
  connect(ui->saveThemeButton, &QPushButton::clicked, this, [this]() {
    saveSessionTheme(ui->themeLineEdit->text());
    ui->themeLineEdit->clear();
  });
  connect(ui->exportThemesButton, &QPushButton::clicked, this, [this]() {
    QString defaultFileName =
        QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss") + "_note.md";
    QString filePath =
        QFileDialog::getSaveFileName(this, "导出主题记录", defaultFileName,
                                     "Markdown文件 (*.md);;CSV文件 (*.csv)");
    if (!filePath.isEmpty()) {
      QDateTime today = QDateTime::currentDateTime();
      QDateTime weekStart = today.addDays(-7);
      exportSessionThemes(filePath, weekStart, today);
    }
  });
  connect(ui->autoLockCheckBox, &QCheckBox::checkStateChanged, this,
          &MainWindow::onAutoLockChanged);
  connect(ui->volumeSlider, &QSlider::valueChanged, this,
          &MainWindow::onVolumeChanged);

  // 初始化时间
  remainingTime = QTime(0, workDuration / 60, workDuration % 60);

  // 创建系统托盘图标
  createTrayIcon();

  // 更新托盘图标显示
  updateTimer(); // 这会设置初始的托盘图标

  // 应用主题
  applyTheme();
}

MainWindow::~MainWindow() {
  saveSettings();
  delete ui;
}

void MainWindow::createTrayIcon() {
  trayIcon = new QSystemTrayIcon(this);

  // 创建一个简单的图标（如果没有系统主题图标）
  updateTrayIcon(); // 使用updateTrayIcon来确保图标正确地响应主题变化

  trayMenu = new QMenu(this);
  QAction *showAction = new QAction("显示窗口", this);
  QAction *hideAction = new QAction("隐藏窗口", this);
  QAction *quitAction = new QAction("退出", this);

  connect(showAction, &QAction::triggered, this, &MainWindow::showWindow);
  connect(hideAction, &QAction::triggered, this, &MainWindow::hideWindow);
  connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

  trayMenu->addAction(showAction);
  trayMenu->addAction(hideAction);
  trayMenu->addSeparator();
  trayMenu->addAction(quitAction);

  trayIcon->setContextMenu(trayMenu);
  trayIcon->show();

  connect(trayIcon, &QSystemTrayIcon::activated, this,
          &MainWindow::onTrayIconActivated);
}

void MainWindow::updateTimer() {
  remainingTime = remainingTime.addSecs(-1);
  ui->timeLabel->setText(remainingTime.toString("mm:ss"));

  // 更新浮动窗口的时间显示
  if (floatingTimer && floatingTimer->isVisible()) {
    floatingTimer->setTime(remainingTime);
  }

  // 更新托盘图标提示和标题
  QString tooltip = QString("番茄时钟 - %1: %2分钟")
                        .arg(ui->phaseLabel->text())
                        .arg(remainingTime.minute());
  trayIcon->setToolTip(tooltip);

  // 创建带进度条和时间显示的图标
  QPixmap pixmap(250, 80); // 更长的图标提供更大显示空间
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);

  // 计算进度百分比
  int totalSeconds = remainingTime.minute() * 60 + remainingTime.second();
  int totalDuration = isWorkPhase ? workDuration : breakDuration;
  int progress = 100 - (totalSeconds * 100) / totalDuration;

  // 绘制左侧圆圈进度条
  int circleSize = 40;
  int circleX = 2;
  int circleY = (pixmap.height() - circleSize) / 2;

  // 绘制圆圈背景
  painter.setBrush(isDarkTheme ? Qt::darkGray : Qt::lightGray);
  painter.setPen(Qt::NoPen);
  painter.drawEllipse(circleX, circleY, circleSize, circleSize);

  // 绘制进度弧线
  painter.setPen(QPen(isDarkTheme ? Qt::white : Qt::black, 3));
  int startAngle = 90 * 16;                   // 从12点开始
  int spanAngle = -progress * 360 / 100 * 16; // 顺时针绘制
  painter.drawArc(circleX + 2, circleY + 2, circleSize - 4, circleSize - 4,
                  startAngle, spanAngle);

  // 在圆圈右侧绘制时间文本（只显示分钟数）
  painter.setPen(isDarkTheme ? Qt::white : Qt::black);
  QFont font("Arial", 100, QFont::Bold);
  painter.setFont(font);

  QString timeText = QString::number(remainingTime.minute());
  QRect textRect(circleSize + 15, 0, pixmap.width() - circleSize - 15,
                 pixmap.height());
  painter.drawText(textRect, Qt::AlignCenter, timeText);

  trayIcon->setIcon(QIcon(pixmap));

  if (remainingTime == QTime(0, 0)) {
    playSound();
    switchPhase();
  }
}

void MainWindow::onStartButtonClicked() {
  if (!timer->isActive()) {
    timer->start(1000); // 1秒触发一次
    ui->startButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
  }
}

void MainWindow::onPauseButtonClicked() {
  if (timer->isActive()) {
    timer->stop();
    ui->startButton->setEnabled(true);
    ui->pauseButton->setText("继续");
  } else {
    timer->start();
    ui->startButton->setEnabled(false);
    ui->pauseButton->setText("暂停");
  }
}

void MainWindow::onResetButtonClicked() {
  timer->stop();
  isWorkPhase = true;
  workDuration = settings->value("workDuration", 25 * 60).toInt();
  breakDuration = settings->value("breakDuration", 5 * 60).toInt();
  remainingTime = QTime(0, workDuration / 60, workDuration % 60);
  ui->timeLabel->setText(remainingTime.toString("mm:ss"));

  // 如果有主题，显示主题内容；否则显示"工作阶段"
  QString displayText =
      currentSessionTheme.isEmpty() ? "工作阶段" : currentSessionTheme;
  ui->phaseLabel->setText(displayText);

  ui->startButton->setEnabled(true);
  ui->pauseButton->setEnabled(false);
  ui->pauseButton->setText("暂停");

  // 重置后更新托盘图标显示
  updateTimer();
}

void MainWindow::switchPhase() {
  // 播放更显著的提示音
  playSound();
  QTimer::singleShot(500, this, [this]() { playSound(); });
  QTimer::singleShot(1000, this, [this]() { playSound(); });

  if (isWorkPhase) {
    completedCycles++;
    updateCycleCount();
  }

  isWorkPhase = !isWorkPhase;

  if (isWorkPhase) {
    remainingTime = QTime(0, workDuration / 60, workDuration % 60);

    // 如果有主题，显示主题内容；否则显示"工作阶段"
    QString displayText =
        currentSessionTheme.isEmpty() ? "工作阶段" : currentSessionTheme;
    ui->phaseLabel->setText(displayText);

    // 显示弹窗提醒
    ReminderDialog *dialog =
        new ReminderDialog("休息结束！\n开始新的一轮工作 ⏰", this);
    dialog->show();

    // 继续显示托盘消息
    trayIcon->showMessage("番茄时钟", "休息结束，开始工作！",
                          QSystemTrayIcon::Information, 3000);
  } else {
    remainingTime = QTime(0, breakDuration / 60, breakDuration % 60);
    ui->phaseLabel->setText("休息阶段");

    // 如果启用自动锁屏，则锁屏
    if (enableAutoLock) {
      lockScreen();
    }

    // 显示弹窗提醒
    QString message;
    if (enableAutoLock) {
      message =
          QString(
              "工作完成！\n恭喜完成第%1个番茄钟 🎉\n系统已自动锁屏，请休息 🌿")
              .arg(completedCycles);
    } else {
      message = QString("工作完成！\n恭喜完成第%1个番茄钟 🎉\n现在开始休息 🌿")
                    .arg(completedCycles);
    }
    ReminderDialog *dialog = new ReminderDialog(message, this);
    dialog->show();

    // 继续显示托盘消息
    trayIcon->showMessage("番茄时钟",
                          "工作结束，开始休息！" +
                              QString(enableAutoLock ? " (系统已锁屏)" : ""),
                          QSystemTrayIcon::Information, 3000);
  }

  ui->timeLabel->setText(remainingTime.toString("mm:ss"));
  saveSettings();
}

void MainWindow::onSettingsButtonClicked() {
  bool ok;
  int newWorkDuration = QInputDialog::getInt(
      this, "设置工作时间", "工作时间（分钟）:", workDuration / 60, 1, 120, 1,
      &ok);
  if (ok) {
    workDuration = newWorkDuration * 60;
    settings->setValue("workDuration", workDuration); // 立即保存
  }

  int newBreakDuration = QInputDialog::getInt(
      this, "设置休息时间", "休息时间（分钟）:", breakDuration / 60, 1, 60, 1,
      &ok);
  if (ok) {
    breakDuration = newBreakDuration * 60;
    settings->setValue("breakDuration", breakDuration); // 立即保存
  }

  // 直接更新显示
  if (isWorkPhase) {
    remainingTime = QTime(0, workDuration / 60, workDuration % 60);
  } else {
    remainingTime = QTime(0, breakDuration / 60, breakDuration % 60);
  }
  ui->timeLabel->setText(remainingTime.toString("mm:ss"));
  updateTimer();
}

void MainWindow::onThemeChanged() {
  isDarkTheme = !isDarkTheme;
  applyTheme();
  saveSettings();
}

void MainWindow::applyTheme() {
  // 使用QPalette来设置主题，避免样式表影响布局
  QPalette palette;

  if (isDarkTheme) {
    // 深色主题
    palette.setColor(QPalette::Window, QColor(43, 43, 43));        // 主窗口背景
    palette.setColor(QPalette::WindowText, QColor(255, 255, 255)); // 文字颜色
    palette.setColor(QPalette::Base, QColor(64, 64, 64));          // 控件背景
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53)); // 交替背景
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(43, 43, 43));
    palette.setColor(QPalette::Text, QColor(255, 255, 255));       // 文本颜色
    palette.setColor(QPalette::Button, QColor(64, 64, 64));        // 按钮背景
    palette.setColor(QPalette::ButtonText, QColor(255, 255, 255)); // 按钮文字
    palette.setColor(QPalette::BrightText, QColor(255, 0, 0));
    palette.setColor(QPalette::Link, QColor(173, 216, 230));
    palette.setColor(QPalette::Highlight, QColor(110, 110, 110)); // 选中背景
    palette.setColor(QPalette::HighlightedText,
                     QColor(255, 255, 255)); // 选中文字

    ui->themeButton->setText("浅色主题");
  } else {
    // 浅色主题（系统默认）
    palette = QApplication::palette(); // 恢复系统默认主题
    ui->themeButton->setText("深色主题");

    // 确保重置所有必要的颜色
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor(233, 233, 233));
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(240, 240, 240));
    palette.setColor(QPalette::ButtonText, Qt::black);
  }

  // 应用全局调色板，保持控件布局稳定
  QApplication::setPalette(palette);
  setPalette(palette);

  // 浮动窗口也需要应用主题，但不改变其布局
  if (floatingTimer && floatingTimer->isVisible()) {
    floatingTimer->update();
  }

  // 更新托盘图标以适应新主题
  updateTrayIcon();
}

void MainWindow::updateCycleCount() {
  ui->cycleLabel->setText(QString("已完成周期: %1").arg(completedCycles));
}

void MainWindow::playSound() {
  QSoundEffect *effect = new QSoundEffect(this);
  effect->setSource(QUrl::fromLocalFile("/System/Library/Sounds/Ping.aiff"));
  effect->setVolume(volume);
  effect->play();
  connect(effect, &QSoundEffect::playingChanged, [effect]() {
    if (!effect->isPlaying()) {
      effect->deleteLater();
    }
  });
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
  if (reason == QSystemTrayIcon::DoubleClick) {
    if (isVisible()) {
      hide();
    } else {
      show();
      activateWindow();
    }
  }
}

void MainWindow::showWindow() {
  show();
  activateWindow();
}

void MainWindow::hideWindow() {
  // 只隐藏主窗口，浮动窗口保持显示状态
  hide();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (trayIcon->isVisible()) {
    hide();
    event->ignore();
  }
}

void MainWindow::saveSettings() {
  settings->setValue("workDuration", workDuration);
  settings->setValue("breakDuration", breakDuration);
  settings->setValue("completedCycles", completedCycles);
  settings->setValue("isDarkTheme", isDarkTheme);
  settings->setValue("enableAutoLock", enableAutoLock);
  settings->setValue("volume", volume);
}

void MainWindow::loadSettings() {
  workDuration = settings->value("workDuration", 25 * 60).toInt();
  breakDuration = settings->value("breakDuration", 5 * 60).toInt();
  completedCycles = settings->value("completedCycles", 0).toInt();
  isDarkTheme = settings->value("isDarkTheme", false).toBool();
  enableAutoLock = settings->value("enableAutoLock", false).toBool();
  volume = settings->value("volume", 0.5f).toFloat(); // 加载音量设置

  // 更新音量滑块和显示标签
  ui->volumeSlider->setValue(static_cast<int>(volume * 100));
  ui->volumeValueLabel->setText(
      QString("%1%").arg(static_cast<int>(volume * 100)));
}

void MainWindow::lockScreen() {
  // 调用系统命令锁屏（macOS）
  system("pmset displaysleepnow");
}

void MainWindow::onAutoLockChanged() {
  enableAutoLock = ui->autoLockCheckBox->isChecked();
  settings->setValue("enableAutoLock", enableAutoLock);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape) {
    hideWindow();
  }
  QMainWindow::keyPressEvent(event);
}

void MainWindow::toggleFloatingWindow() {
  if (floatingTimer->isVisible()) {
    floatingTimer->hide();
    ui->floatingButton->setText("浮动窗口");
  } else {
    // 更新时间到浮动窗口
    floatingTimer->setTime(remainingTime);
    floatingTimer->setWorkPhase(isWorkPhase);
    floatingTimer->resetTimer(workDuration, breakDuration, isWorkPhase);

    if (timer->isActive()) {
      floatingTimer->startTimer();
    } else {
      floatingTimer->stopTimer();
    }

    floatingTimer->show();
    ui->floatingButton->setText("隐藏浮动");
  }
}

void MainWindow::resetFloatingWindowPosition() {
  if (floatingTimer) {
    floatingTimer->moveToDefaultPosition();
    floatingTimer->show(); // 确保窗口显示
  }
}

// 保存当前会话主题
void MainWindow::saveSessionTheme(const QString &theme) {
  currentSessionTheme = theme;
  QSettings settings("PomodoroApp", "SessionThemes");
  settings.beginGroup("Themes");
  settings.setValue(QDateTime::currentDateTime().toString(Qt::ISODate), theme);
  settings.endGroup();

  // 立即更新界面显示
  QString displayText =
      currentSessionTheme.isEmpty() ? "工作阶段" : currentSessionTheme;
  ui->phaseLabel->setText(displayText);
}

// 获取指定时间范围内的主题记录
QList<QPair<QDateTime, QString>>
MainWindow::getSessionThemes(const QDateTime &from, const QDateTime &to) const {
  QList<QPair<QDateTime, QString>> result;
  QSettings settings("PomodoroApp", "SessionThemes");
  settings.beginGroup("Themes");

  foreach (const QString &key, settings.allKeys()) {
    QDateTime dt = QDateTime::fromString(key, Qt::ISODate);
    if (dt >= from && dt <= to) {
      result.append(qMakePair(dt, settings.value(key).toString()));
    }
  }

  settings.endGroup();
  return result;
}

// 导出主题记录到文件
void MainWindow::exportSessionThemes(const QString &filePath,
                                     const QDateTime &from,
                                     const QDateTime &to) const {
  QFile file(filePath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }

  QTextStream out(&file);
  out << "时间,主题内容\n";

  auto themes = getSessionThemes(from, to);
  foreach (const auto &theme, themes) {
    out << theme.first.toString("yyyy-MM-dd hh:mm:ss") << "," << theme.second
        << "\n";
  }

  file.close();
}

// 音量调节槽函数
void MainWindow::onVolumeChanged(int value) {
  volume = value / 100.0f;              // 将0-100的值转换为0.0-1.0
  settings->setValue("volume", volume); // 保存音量设置

  // 更新音量显示标签
  ui->volumeValueLabel->setText(QString("%1%").arg(value));
}

// 更新托盘图标以适配当前主题
void MainWindow::updateTrayIcon() {
  if (!trayIcon)
    return;

  // 创建简单的单色图标适配主题
  QPixmap pixmap(16, 16);
  pixmap.fill(isDarkTheme ? Qt::white : Qt::black);
  trayIcon->setIcon(QIcon(pixmap));
}