#include "terminal.h"
#include "ui_terminal.h"
#include <QMessageBox>
#include <QTimer>
#include <QProcess>
#include <QKeyEvent>
#include <QTextBlock>

#define CMDTXT "user@Terminal# "

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
    ui->textEdit->append(CMDTXT);
}

void Terminal::write(){
    QString word=Terminal::textCursor();
    QRegExp rxlen("user@Terminal#(.+)$");
   if(rxlen.indexIn(word)!=-1 && Terminal::textEnd(15)!=CMDTXT)
   {
       QString ch = rxlen.cap(1).trimmed()+"\n";
       char* shell; QByteArray ba = ch.toLatin1(); shell=ba.data();
       cmd->write(shell);
       //Terminal::checkshell();
   }else{

        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEdit->setTextCursor(cursor);

   }

}

void Terminal::checkshell(){
    QString word= ui->textEdit->toPlainText().trimmed();
    QString w=word.mid(word.length()-1,word.length());
    if(w!="#"){
        ui->textEdit->append(CMDTXT);
    }

}

//监视对象
bool Terminal::eventFilter(QObject *target, QEvent *event)
{
  /*处理按键消息 */
  if(event->type() == QEvent::KeyPress)
  {
    if (target == ui->textEdit){
            /*强制类型转换 */
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

            //处理回车消息
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220){
                   Terminal::write();
                    return true;
            //处理删除消息
            }else if(keyEvent->key() == Qt::Key_Backspace ){
                QString  word=Terminal::textCursor();
                //不允许删除前面的内容
                if(Terminal::textEnd(15)==CMDTXT){return true;}

                if(word.contains(CMDTXT)){

                    return (Terminal::textCursor()==CMDTXT);
                }else{

                    return true;
                }

            //不允许修改前面的内容
            }else if(Terminal::textEnd(15)==CMDTXT && Terminal::textCursor()!=CMDTXT){
                return true;

            }
     }

   }

     return QWidget::eventFilter(target, event);
}

//取光标所在行内容
QString Terminal::textCursor()
{
    QTextCursor tc;
    tc = ui->textEdit->textCursor();
    return tc.block().text();
}

//从后截取文本

QString Terminal::textEnd(int num)
{
   QString word=ui->textEdit->toPlainText();

    word=word.mid(word.size()-num,word.size());

    return word;
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

