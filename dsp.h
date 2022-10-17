#ifndef DSP_H
#define DSP_H

#include <QThread>
#include "dspengine.h"
#include "dspsettings.h"

class DSP : public QObject
{
    Q_OBJECT
    QThread dspThread;
public:
    DSP(DSPSettings *settings, const quint16 &gate);
    ~DSP();
    static int inputCallBack(const void *inBuffer, void *outBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo *timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void* userData)
    {
        return reinterpret_cast<DSP*>(userData)->recordCallBack(inBuffer, outBuffer,
                                                                framesPerBuffer,
                                                                timeInfo, statusFlags);
    }

public slots:
    void start();
    void stop();
    void setGate(const quint16 &threshold);

private:
    void Pa_Check(bool condition);
    int recordCallBack(const void *inBuffer, void *outBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo *timeInfo,
                       PaStreamCallbackFlags statusFlags);
    PaError control;
    quint16 peak, module;
    PaStreamParameters inParameters;
    PaStream *inStream;

private slots:
    void stateHandle(const quint8 &value, const quint16 &gate);

signals:
    void errorOccured(const QString&);
    void gotSamples(const qint16*);
    void peakMeasured(const quint16&);
    void stateChanged(const quint8&, const quint16&);
    void thresholdChanged(const quint16&);
};

#endif // DSP_H
