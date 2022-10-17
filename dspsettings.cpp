#include "dspsettings.h"

DSPSettings::DSPSettings()
    : sampleRate(32000)
    , deviceIndex(0)
    , sampleType(paInt16)
    , framesPerBuffer(2048)
    , channelCount(1)
{
    control = Pa_Initialize();
    Pa_Check(control == paNoError);
    control = Pa_GetHostApiCount();
    Pa_Check(control > 0 && Pa_GetDeviceCount() > 0);
    for (quint8 i = 0; i < control; i++)
    {
        const PaHostApiInfo *apiNfo = Pa_GetHostApiInfo(i);
        hostApis.append(QString(apiNfo->name));
    }
}

DSPSettings::~DSPSettings()
{

}

void DSPSettings::Pa_Check(bool condition)
{
    if (!condition)
        emit errorOccured(QString(Pa_GetErrorText(control)));
}

const QStringList &DSPSettings::getHostApis()
{
    return hostApis;
}

const QHash<QString, int> *DSPSettings::getHostApiDevices(int index)
{
    QHash<QString, int> *devices = new QHash<QString, int>();
    const PaHostApiInfo *apiNfo = Pa_GetHostApiInfo(index);
    PaStreamParameters inParams;
    inParams.channelCount = channelCount;
    inParams.sampleFormat = sampleType;
    inParams.suggestedLatency = Pa_GetDeviceInfo(deviceIndex)->defaultLowInputLatency;
    inParams.hostApiSpecificStreamInfo = NULL;
    for (quint8 i = 0; i < apiNfo->deviceCount; i++)
    {
        deviceIndex = Pa_HostApiDeviceIndexToDeviceIndex(index, i);
        inParams.device = deviceIndex;
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(deviceIndex);
        if ((deviceInfo->maxInputChannels > 0) &&
                (Pa_IsFormatSupported(&inParams
                                      , NULL
                                      , static_cast<double>(sampleRate)) == paFormatIsSupported))
            devices->insert(QString(deviceInfo->name), deviceIndex);
    }
    return devices;
}

void DSPSettings::setDeviceIndex(const PaDeviceIndex &index)
{
    deviceIndex = index;
}

const PaDeviceIndex &DSPSettings::getDeviceIndex(){return deviceIndex;}

const quint32 &DSPSettings::getSampleRate(){return sampleRate;}

const PaSampleFormat &DSPSettings::getSampleType(){return sampleType;}

const quint8 &DSPSettings::getChannelCount(){return channelCount;}

const quint16 &DSPSettings::getFramesPerBuffer(){return framesPerBuffer;}
