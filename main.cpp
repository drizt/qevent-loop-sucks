#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QObject>
#include <QTimer>

#ifdef Q_OS_WIN
# include <windows.h>
#else
# include <csignal>
#endif

// const int childDeletionDelay = 1000;
const int childDeletionDelay = 100;

class Child : public QObject
{
    Q_OBJECT

public:
    explicit Child() : QObject() {}
    ~Child() {qDebug() << "Child dtor"; }

public:
    void calc()
    {
        QEventLoop loop;
        QTimer::singleShot(500, &loop, &QEventLoop::quit);
        loop.exec();
        qDebug() << this;
        deleteLater();
    }
};

class Parent : public QObject
{
    Q_OBJECT

public:
    explicit Parent() : QObject() {}

    void exec()
    {
        Child *child = new Child;
        QTimer::singleShot(childDeletionDelay, child, &QObject::deleteLater);
        child->calc();
        connect(child, &QObject::destroyed, qApp, &QCoreApplication::quit);
    }
};

#ifdef Q_OS_WIN
LONG WINAPI signalHandler(EXCEPTION_POINTERS *ExceptionInfo)
{
    qDebug() << "Exception caught:" << ExceptionInfo->ExceptionRecord->ExceptionCode;
    return EXCEPTION_EXECUTE_HANDLER;
}
#else
void signalHandler(int signum)
{
    qDebug() << "Signal caught:" << signum;
    exit(0);
}
#endif

int main(int argc, char *argv[])
{
    // Catch segmentation fault
#ifdef Q_OS_WIN
    SetUnhandledExceptionFilter(signalHandler);
#else
    signal(SIGSEGV, signalHandler);
#endif
    QCoreApplication a(argc, argv);
    Parent parent;
    QTimer::singleShot(0, &parent, &Parent::exec);
    return a.exec();
}

#include "main.moc"
