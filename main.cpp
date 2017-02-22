#include "server.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    server w;
    w.show();
    w.tab->setWindowIcon(QIcon(":/icon.png"));
    w.setWindowIcon(QIcon(":/icon.png"));
    return a.exec();
}
