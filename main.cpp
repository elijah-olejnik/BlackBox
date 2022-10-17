#include "blackbox.h"
#include "dspsettings.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BlackBox w;
    w.show();
    return a.exec();
}
