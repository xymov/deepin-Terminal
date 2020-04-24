#ifndef TERMINAL_H
#define TERMINAL_H
#include <QProcess>
#include <QWidget>

#include <QMainWindow>

namespace Ui {
class Terminal;
}

class Terminal : public QMainWindow
{
    Q_OBJECT

public:
    explicit Terminal(QWidget *parent = nullptr);
    ~Terminal();
     QProcess * cmd;

 protected :

    bool eventFilter(QObject *target, QEvent *event);



private slots:

    void on_readoutput();
    void on_readerror();
    void init();
    void write();
    void checkshell();
private:
    Ui::Terminal *ui;
    QString textCursor();
    QString textEnd(int num);

};

#endif // TERMINAL_H
