#include "reminder_dialog.h"
#include <QApplication>
#include <QScreen>
#include <QTimer>

ReminderDialog::ReminderDialog(const QString &message, QWidget *parent)
    : QDialog(parent)
{
    setupUI(message);
    playAlertSound();
    
    // 设置窗体属性
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    
    // 淡入动画
    fadeAnimation = new QPropertyAnimation(this, "windowOpacity");
    fadeAnimation->setDuration(500);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(1.0);
    fadeAnimation->start();
    
    connect(fadeAnimation, &QPropertyAnimation::finished, this, &ReminderDialog::onFadeInFinished);
    connect(okButton, &QPushButton::clicked, this, &ReminderDialog::onOkButtonClicked);
}

ReminderDialog::~ReminderDialog()
{
    delete fadeAnimation;
    delete soundEffect;
}

void ReminderDialog::setupUI(const QString &message)
{
    // 设置对话框大小和位置
    setFixedSize(300, 200);
    
    // 获取主屏幕尺寸并居中显示
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 创建布局和控件
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    messageLabel = new QLabel(message, this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #e74c3c;");
    
    okButton = new QPushButton("确定", this);
    okButton->setFixedHeight(40);
    okButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #21618c;"
        "}"
    );
    
    layout->addWidget(messageLabel);
    layout->addWidget(okButton);
    
    // 设置对话框样式
    setStyleSheet(
        "QDialog {"
        "    background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
        "                                stop: 0 #f9f9f9, stop: 1 #e8e8e8);"
        "    border: 3px solid #e74c3c;"
        "    border-radius: 10px;"
        "}"
    );
}

void ReminderDialog::playAlertSound()
{
    soundEffect = new QSoundEffect(this);
    // 播放更明显的提示音
    soundEffect->setSource(QUrl::fromLocalFile("/System/Library/Sounds/Submarine.aiff"));
    soundEffect->setVolume(0.8f);
    soundEffect->play();
    
    // 播放第二次声音以增强提醒效果
    QTimer::singleShot(500, this, [this]() {
        soundEffect->play();
    });
}

void ReminderDialog::onOkButtonClicked()
{
    close();
}

void ReminderDialog::onFadeInFinished()
{
    // 确保对话框获取焦点
    activateWindow();
    raise();
}