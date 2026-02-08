#include "floating_timer.h"
#include <QApplication>
#include <QFont>
#include <QPainter>
#include <QScreen>
#include <QSettings>

FloatingTimer::FloatingTimer(QWidget *parent)
    : QWidget(parent), timer(new QTimer(this)), isWorkPhase(true),
      isDragging(false), workDuration(25 * 60), breakDuration(5 * 60) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool |
                 Qt::WindowDoesNotAcceptFocus | Qt::WindowTransparentForInput |
                 Qt::Window);
  setAttribute(Qt::WA_TranslucentBackground);

  // 设置窗口大小 - 超大字体显示
  setFixedSize(400, 200);

  // 尝试加载保存的位置，如果没有则移动到屏幕左上角
  loadPosition();

  timer->setInterval(1000);
  connect(timer, &QTimer::timeout, this, &FloatingTimer::updateTimer);

  remainingTime = QTime(0, workDuration / 60, workDuration % 60);
}

FloatingTimer::~FloatingTimer() {}

void FloatingTimer::updateTimer() {
  if (remainingTime > QTime(0, 0)) {
    remainingTime = remainingTime.addSecs(-1);
    update(); // 触发重绘
  }
}

void FloatingTimer::setTime(const QTime &time) {
  remainingTime = time;
  update();
}

void FloatingTimer::setWorkPhase(bool isWork) {
  isWorkPhase = isWork;
  update();
}

void FloatingTimer::startTimer() { timer->start(); }

void FloatingTimer::stopTimer() { timer->stop(); }

void FloatingTimer::resetTimer(int workDur, int breakDur, bool phase) {
  workDuration = workDur;
  breakDuration = breakDur;
  isWorkPhase = phase;

  if (isWorkPhase) {
    remainingTime = QTime(0, workDuration / 60, workDuration % 60);
  } else {
    remainingTime = QTime(0, breakDuration / 60, breakDuration % 60);
  }
  update();
}

void FloatingTimer::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event)

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  // 绘制半透明黑色背景
  painter.setBrush(QColor(0, 0, 0, 200));
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(rect(), 15, 15);

  // 设置超大字体
  QFont font("Arial", 120, QFont::Bold);
  painter.setFont(font);

  // 根据阶段设置颜色
  if (isWorkPhase) {
    painter.setPen(QColor(255, 100, 100)); // 工作阶段红色
  } else {
    painter.setPen(QColor(100, 255, 100)); // 休息阶段绿色
  }

  // 绘制时间文本（显示分钟和秒数，格式：mm:ss）
  QString timeText = remainingTime.toString("mm:ss");
  painter.drawText(rect(), Qt::AlignCenter, timeText);

  // 绘制小文本显示阶段信息
  QFont smallFont("Arial", 16, QFont::Normal);
  painter.setFont(smallFont);
  painter.setPen(Qt::white);

  QString phaseText = isWorkPhase ? "工作阶段" : "休息阶段";
  QRect phaseRect(10, 10, width() - 20, 30);
  painter.drawText(phaseRect, Qt::AlignLeft | Qt::AlignTop, phaseText);

  // 绘制时间格式提示
  QString formatText = remainingTime.toString("mm:ss");
  QRect formatRect(10, height() - 40, width() - 20, 30);
  painter.drawText(formatRect, Qt::AlignLeft | Qt::AlignBottom, formatText);
}

void FloatingTimer::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    isDragging = true;
    dragPosition =
        event->globalPosition().toPoint() - frameGeometry().topLeft();
    event->accept();
  }
}

void FloatingTimer::mouseMoveEvent(QMouseEvent *event) {
  if (isDragging && (event->buttons() & Qt::LeftButton)) {
    move(event->globalPosition().toPoint() - dragPosition);
    event->accept();
  }
}

void FloatingTimer::moveToDefaultPosition() {
  // 移动到屏幕左上角
  move(20, 80); // 左上角位置
  savePosition();
}

void FloatingTimer::savePosition() {
  QSettings settings("QtPomodoro", "FloatingTimer");
  settings.setValue("windowPos", pos());
}

void FloatingTimer::loadPosition() {
  QSettings settings("QtPomodoro", "FloatingTimer");
  QPoint savedPos = settings.value("windowPos").toPoint();

  if (savedPos.isNull()) {
    // 如果没有保存的位置，移动到屏幕左上角
    moveToDefaultPosition();
  } else {
    move(savedPos);
  }
}

void FloatingTimer::closeEvent(QCloseEvent *event) {
  savePosition(); // 关闭时保存位置
  timer->stop();
  event->accept();
}