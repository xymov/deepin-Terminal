#include "terminal.h"
#include "ui_terminal.h"
#include <QMessageBox>
#include <QTimer>
#include <QProcess>
#include <QKeyEvent>
#define T_PrivPtr( o ) (( StationaryLampSet *) o )

Terminal::Terminal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Terminal)
{
    ui->setupUi(this);

    //注册监视对象
    ui->textEdit->installEventFilter(this);

    //初始化QProcess
    cmd=new QProcess(this);
    connect(cmd , SIGNAL(readyReadStandardOutput()) , this , SLOT(on_readoutput()));
    connect(cmd , SIGNAL(readyReadStandardError()) , this , SLOT(on_readerror()));

    //注册启动事件
    QTimer::singleShot(0, this, SLOT(init()));

}

Terminal::~Terminal()
{
   //销毁外部程序
    if(cmd)
    {
          cmd->close();
          cmd->waitForFinished();
    }

    delete ui;

}

void Terminal::init()
{
    cmd->start("bash");            //启动终端(Windows下改为cmd)
    cmd->waitForStarted();        //等待启动完成
    ui->textEdit->append("user@Terminal# ");
}

void Terminal::write(){
    int pos=0;
    QString word= ui->textEdit->toPlainText();
    QRegExp rxlen("user@Terminal#(.+)(\\n|$)");
    rxlen.setMinimal (true) ;
    pos=rxlen.lastIndexIn(word);
   if(pos!=-1){
       QString ch = rxlen.cap(1).trimmed()+"\n";
       char* shell; QByteArray ba = ch.toLatin1(); shell=ba.data();
       cmd->write(shell);
       //Terminal::checkshell();
   }

}

void Terminal::checkshell(){
    QString word= ui->textEdit->toPlainText().trimmed();
    QString w=word.mid(word.length()-1,word.length());
    if(w!="#"){
        ui->textEdit->append("user@Terminal# ");
    }

}

//监视对象
bool Terminal::eventFilter(QObject *target, QEvent *event)
{
     /*处理按键消息 */
    if (target == ui->textEdit && event->type() == QEvent::KeyPress){
            /*强制类型转换 */
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220) {
                   Terminal::write();
                    return true;
            }
    }
    /*处理按键消息 */
    return QWidget::eventFilter(target, event);
}

void Terminal::on_readoutput()
{
    ui->textEdit->append(cmd->readAllStandardOutput().data());   //将输出信息读取到编辑框
    Terminal::checkshell();
}

void Terminal::on_readerror()
{
    ui->textEdit->append(cmd->readAllStandardError().data());
    Terminal::checkshell();
}

