#ifndef FLOATING_TIMER_H
#define FLOATING_TIMER_H

#include <QWidget>
#include <QTimer>
#include <QTime>
#include <QSettings>
#include <QCloseEvent>
#include <QMouseEvent>

class FloatingTimer : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingTimer(QWidget *parent = nullptr);
    ~FloatingTimer();

public slots:
    void updateTimer();
    void setTime(const QTime &time);
    void setWorkPhase(bool isWork);
    void startTimer();
    void stopTimer();
    void resetTimer(int workDuration, int breakDuration, bool isWorkPhase);
    void moveToDefaultPosition();
    void savePosition();
    void loadPosition();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    QTimer *timer;
    QTime remainingTime;
    bool isWorkPhase;
    bool isDragging;
    QPoint dragPosition;
    int workDuration;
    int breakDuration;
};

#endif // FLOATING_TIMER_H