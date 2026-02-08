#ifndef REMINDERDIALOG_H
#define REMINDERDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QSoundEffect>

class ReminderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReminderDialog(const QString &message, QWidget *parent = nullptr);
    ~ReminderDialog();

private slots:
    void onOkButtonClicked();
    void onFadeInFinished();

private:
    QLabel *messageLabel;
    QPushButton *okButton;
    QPropertyAnimation *fadeAnimation;
    QSoundEffect *soundEffect;
    
    void setupUI(const QString &message);
    void playAlertSound();
};

#endif // REMINDERDIALOG_H