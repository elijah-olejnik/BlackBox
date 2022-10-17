#ifndef DSPENGINE_H
#define DSPENGINE_H

#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QDataStream>
#include <QLocale>
#include <math.h>
#include <portaudio.h>
#include <lame.h>
#include "qringbuffer.h"
#include "dspsettings.h"

const qreal sampleMax = 32768.0;

class DSPEngine : public QObject
{
    Q_OBJECT

public:
    DSPEngine(DSPSettings *params, const quint16 &thrshld);
    ~DSPEngine();
    enum State{Idle, Active};
    const State &currentState() const;

public slots:
    void readBuffer(const qint16 *inputData);
    void setGate(const quint16 &val);

private:
    void setState(State state);
    void analyzeData();
    void writeDataToFile(qint16 *buf, quint16 bufSize);
    void startOutput();
    void stopOutput();
    quint16 Pa_bufferSize, rms, module, gate;
    qreal counter;
    quint16 write;
    char syncFlag;
    quint8 skip;
    State m_state;
    QRingBuffer<qint16> ringBuffer;
    qint16 *data;
    QFile *outFile;
    QDataStream out;
    QLocale locale;
    lame_t lame;
    quint8 *mp3buff;
    const qreal factor;
    qint16 *tempBuff;
    quint16 tempStored, tempSize;

signals:
    void stateChanged(const quint8&, const quint16&);
};

#endif // DSPENGINE_H
