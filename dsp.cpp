#include "dsp.h"

DSP::DSP(DSPSettings *settings, const quint16 &gate)
    : control(0), peak(0)
    , module(0), inStream(NULL)
{
    DSPEngine *dspEngine = new DSPEngine(settings, gate);
    dspEngine->moveToThread(&dspThread);
    connect(&dspThread, SIGNAL(finished()), dspEngine, SLOT(deleteLater()));
    connect(dspEngine, SIGNAL(stateChanged(quint8,quint16)), this, SLOT(stateHandle(quint8,quint16)));
    connect(this, SIGNAL(gotSamples(const qint16*)), dspEngine, SLOT(readBuffer(const qint16*)));
    connect(this, SIGNAL(thresholdChanged(quint16)), dspEngine, SLOT(setGate(quint16)));
    dspThread.start();
    inParameters.device = settings->getDeviceIndex();
    inParameters.channelCount = settings->getChannelCount();
    inParameters.sampleFormat = settings->getSampleType();
    inParameters.suggestedLatency = Pa_GetDeviceInfo(inParameters.device)->defaultLowInputLatency;
    inParameters.hostApiSpecificStreamInfo = NULL;
    control = Pa_OpenStream(&inStream, &inParameters, NULL, settings->getSampleRate(),
                            settings->getFramesPerBuffer(), paClipOff, &inputCallBack, this);
    Pa_Check(control == paNoError);
}

DSP::~DSP()
{
    dspThread.quit();
    dspThread.wait();
    stop();
    control = Pa_CloseStream(inStream);
    Pa_Check(control == paNoError);
}

void DSP::Pa_Check(bool condition)
{
    if (!condition)
        emit errorOccured(QString(Pa_GetErrorText(control)));
}

int DSP::recordCallBack(const void *inBuffer, void *outBuffer, unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags)
{
    const qint16 *rptr = reinterpret_cast<const qint16*>(inBuffer);
    emit gotSamples(rptr);
    peak = 0;
    module = 0;
    for (quint16 i = 0; i < static_cast<quint8>(inParameters.channelCount) * framesPerBuffer; i++)
    {
        module = static_cast<quint16>(abs(rptr[i]));
        peak = module > peak ? module : peak;
    }
    emit peakMeasured(peak);
    (void)outBuffer;
    (void)framesPerBuffer;
    (void)timeInfo;
    (void)statusFlags;
    return paContinue;
}

void DSP::start()
{
    control = Pa_StartStream(inStream);
    Pa_Check(control == paNoError);
}

void DSP::stop()
{
    control = Pa_StopStream(inStream);
    Pa_Check(control == paNoError);
}

void DSP::stateHandle(const quint8 &value, const quint16 &gate)
{
    emit stateChanged(value, gate);
}

void DSP::setGate(const quint16 &threshold)
{
    emit thresholdChanged(threshold);
}
