#ifndef QRINGBUFFER_H
#define QRINGBUFFER_H

template<class T>

class QRingBuffer
{

public:
    QRingBuffer(unsigned short size)
        : buffer(new T[size]), bufferSize(size)
        , head(0), tail(0){}

    ~QRingBuffer(){delete[] buffer;}

    void addSample(T sample)
    {
        buffer[head] = sample;
        if (++head >= bufferSize)
            head = 0;
    }

    T getSample()
    {
        T result = buffer[tail++];
        if (tail >= bufferSize)
            tail = 0;
        return result;
    }

private:
    T *buffer;
    unsigned short bufferSize, head, tail;
};

#endif // QRINGBUFFER_H
