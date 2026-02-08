#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QCheckBox>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>

class FloatingTimer; // 前向声明浮动窗口类

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void updateTimer();
  void onStartButtonClicked();
  void onPauseButtonClicked();
  void onResetButtonClicked();
  void onSettingsButtonClicked();
  void onThemeChanged();
  void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
  void showWindow();
  void hideWindow();
  void toggleFloatingWindow();        // 切换浮动窗口显示
  void resetFloatingWindowPosition(); // 重置浮动窗口位置到左上角

private:
  Ui::MainWindow *ui;
  QTimer *timer;
  QTime remainingTime;
  bool isWorkPhase;
  int workDuration;    // 工作时间（秒）
  int breakDuration;   // 休息时间（秒）
  int completedCycles; // 完成的周期数
  bool isDarkTheme;    // 当前主题
  bool enableAutoLock; // 是否启用自动锁屏
  QSystemTrayIcon *trayIcon;
  QMenu *trayMenu;
  QSettings *settings;
  FloatingTimer *floatingTimer; // 浮动窗口

  void createTrayIcon();
  void switchPhase();
  void playSound();
  void updateCycleCount();
  void applyTheme();
  void saveSettings();
  void loadSettings();
  void lockScreen();        // 锁屏函数
  void onAutoLockChanged(); // 自动锁屏设置改变

  // 主题记录功能
  void saveSessionTheme(const QString &theme);
  QList<QPair<QDateTime, QString>> getSessionThemes(const QDateTime &from,
                                                    const QDateTime &to) const;
  void exportSessionThemes(const QString &filePath, const QDateTime &from,
                           const QDateTime &to) const;
  QString currentSessionTheme; // 当前会话主题

protected:
  void closeEvent(QCloseEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
};

#endif // MAINWINDOW_H