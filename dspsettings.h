#ifndef DSPSETTINGS_H
#define DSPSETTINGS_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include<QTextCodec>
#include <portaudio.h>

class DSPSettings : public QObject
{
    Q_OBJECT

public:
    explicit DSPSettings();
    ~DSPSettings();
    const QStringList &getHostApis();
    const QHash<QString, int> *getHostApiDevices(int index);
    const PaDeviceIndex &getDeviceIndex();
    const quint32 &getSampleRate();
    const PaSampleFormat &getSampleType();
    const quint8 &getChannelCount();
    const quint16 &getFramesPerBuffer();
    void setDeviceIndex(const PaDeviceIndex &index);
    void setSampleRate(const quint32 &value);
    void setSampleType(const PaSampleFormat &fmt);
    void setChannelCount(const quint8 &numChannels);
    void setFramesPerBuffer(const quint16 &numFrames);

private:
    void Pa_Check(bool condition);
    PaError control;
    quint32 sampleRate;
    QStringList hostApis;
    PaDeviceIndex deviceIndex;
    PaSampleFormat sampleType;
    quint16 framesPerBuffer;
    quint8 channelCount;

signals:
    void errorOccured(const QString &error);
};

#endif // DSPSETTINGS_H
