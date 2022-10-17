#include "dspengine.h"

DSPEngine::DSPEngine(DSPSettings *params, const quint16 &thrshld)
    : Pa_bufferSize(params->getFramesPerBuffer() * params->getChannelCount())
    , rms(0), module(0), gate(thrshld), counter(0), write(0), syncFlag('o')
    , skip(0), m_state(Idle), ringBuffer(Pa_bufferSize * 3), data(NULL)
    , outFile(NULL), locale(QLocale(QLocale::English, QLocale::UnitedStates))
    , lame(lame_init()), mp3buff(NULL), factor(1.0 / sampleMax), tempBuff(NULL)
    , tempStored(0), tempSize(Pa_bufferSize * 16)
{
    lame_set_in_samplerate(lame, static_cast<int>(params->getSampleRate()));
    lame_set_num_channels(lame, params->getChannelCount());
    lame_set_VBR(lame, vbr_off);
    lame_set_brate(lame, 32);
    lame_set_mode(lame, MONO);
    lame_set_quality(lame, 2);
    lame_set_out_samplerate(lame, static_cast<int>(params->getSampleRate()));
    lame_init_params(lame);
}

void DSPEngine::setState(State state)
{
    m_state = state;
    emit stateChanged(state, gate);
}

void DSPEngine::setGate(const quint16 &val)
{
    gate = val;
}

const DSPEngine::State &DSPEngine::currentState() const
{
    return m_state;
}

void DSPEngine::startOutput()
{
    if(m_state == Idle && skip < 16)
    {
        tempBuff = new qint16[tempSize];
        for(quint16 i = 0; i < Pa_bufferSize; i++)
        {
            tempBuff[tempStored] = data[i];
            tempStored++;
        }
        delete[] data;
        emit stateChanged(m_state, gate);
        skip++;
    }
    else
    {
        if(m_state == Idle)
        {
            tempStored = 0;
            if(!QDir("Archive").exists())
                QDir().mkdir("Archive");
            outFile = new QFile("Archive/" + locale.toString(QDateTime::currentDateTime(),
                                                "yyyy-MM-dd-hh-mm-ss-zzz") + ".mp3");
            outFile->open(QIODevice::WriteOnly);
            out.setDevice(outFile);
            writeDataToFile(tempBuff, tempSize);
            writeDataToFile(data, Pa_bufferSize);
            setState(Active);
        }
        else
        {
            writeDataToFile(data, Pa_bufferSize);
            emit stateChanged(m_state, gate);
        }
        skip = 0;
    }
}

void DSPEngine::stopOutput()
{
    if (m_state == Active && skip < 16)
    {
        writeDataToFile(data, Pa_bufferSize);
        emit stateChanged(m_state, gate);
        skip++;
    }
    else
    {
        delete[] data;
        if(m_state == Active)
        {
            lame_encode_flush(lame, mp3buff, Pa_bufferSize);
            outFile->close();
            delete outFile;
            out.unsetDevice();
            setState(Idle);
        }
        else
        {
            if(tempStored > 0)
            {
                delete[] tempBuff;
                tempStored = 0;
            }
            emit stateChanged(m_state, gate);
        }
        skip = 0;
    }
}

void DSPEngine::writeDataToFile(qint16 *buf, quint16 bufSize)
{
    mp3buff = new quint8[bufSize];
    write = static_cast<quint16>(lame_encode_buffer(lame, buf, NULL, bufSize
                               , mp3buff, bufSize));
    for (quint16 i = 0; i < write; i++)
        out << mp3buff[i];
    delete[] buf;
    delete[] mp3buff;
}

void DSPEngine::analyzeData()
{
    rms = 0;
    module = 0;
    counter = 0;
    data = new qint16[Pa_bufferSize];
    for (quint16 i = 0; i < Pa_bufferSize; i++)
    {
         data[i] = ringBuffer.getSample();
         module = static_cast<quint16>(abs(data[i]));
         counter += qreal(module * factor) * qreal(module * factor);
    }
    rms = static_cast<quint16>((sqrt(counter / Pa_bufferSize)) * sampleMax);
    if (rms > gate)
        startOutput();
    else
        stopOutput();
    syncFlag = 'o';
}

void DSPEngine::readBuffer(const qint16 *inputData)
{
    for (quint16 i = 0; i < Pa_bufferSize; i++)
        ringBuffer.addSample(inputData[i]);
    if (syncFlag == 'o')
    {
        syncFlag = 'c';
        analyzeData();
    }
    else
        return;
}

DSPEngine::~DSPEngine()
{
    lame_close(lame);
}
