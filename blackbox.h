#ifndef BLACKBOX_H
#define BLACKBOX_H

#include <QMainWindow>
#include "dsp.h"
#include "dspsettings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BlackBox; }
QT_END_NAMESPACE

class BlackBox : public QMainWindow
{
    Q_OBJECT
    QThread mainThread;
public:
    BlackBox(QWidget *parent = NULL);
    ~BlackBox();

private slots:
    void displayError(const QString & errText);
    void displayDevices(int index);
    void startListen(int index);
    void indicate(const quint16 &val);
    void displayState(const quint8 &code, const quint16& gateIn);
    void setThreshold(int v);

private:
    void ctrlsDisable(bool trigger);
    Ui::BlackBox *ui;
    DSPSettings *dspSettings;
    DSP *inDSP;
    qreal dBLevel, linear, gate, inGate;
    int hostApiIndex,deviceIndex;
    quint16 savedGate;

signals:
    void startDSP();
    void thresholdSet(const quint16 &value);
};
#endif // BLACKBOX_H
