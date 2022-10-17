#include "blackbox.h"
#include "ui_blackbox.h"

BlackBox::BlackBox(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BlackBox)
    , dspSettings(new DSPSettings())
    , inDSP(NULL)
    , hostApiIndex(0), deviceIndex(Pa_GetDefaultInputDevice()), savedGate(20)
{
    ui->setupUi(this);
    connect(dspSettings, SIGNAL(errorOccured(QString)), this, SLOT(displayError(QString)));
    ui->apiBox->addItems(dspSettings->getHostApis());
    QFile iF("Settings.dat");
    if(iF.exists())
    {
        iF.open(QIODevice::ReadOnly);
        QDataStream iS(&iF);
        iS >> hostApiIndex >> deviceIndex >> savedGate;
        iF.close();
    }
    ui->apiBox->setCurrentIndex(hostApiIndex);
    connect(ui->apiBox, SIGNAL(currentIndexChanged(int)), this, SLOT(displayDevices(int)));
    displayDevices(hostApiIndex);
}

BlackBox::~BlackBox()
{
    QFile oF("Settings.dat");
    oF.open(QIODevice::WriteOnly);
    QDataStream oS(&oF);
    oS << hostApiIndex << deviceIndex << savedGate;
    oF.close();
    Pa_Terminate();
    mainThread.quit();
    mainThread.wait();
    delete dspSettings;
    delete ui;
}

void BlackBox::displayError(const QString & errText)
{
    ui->statusbar->showMessage(errText);
    ctrlsDisable(true);
}

void BlackBox::ctrlsDisable(bool trigger)
{
    ui->devBox->setDisabled(trigger);
}

void BlackBox::displayDevices(int index)
{
    hostApiIndex = index;
    ui->devBox->clear();
    const QHash<QString, int> *devs = dspSettings->getHostApiDevices(index);
    QHashIterator<QString, int> i(*devs);
    quint8 cntr = 0;
    quint8 boxIndex = 0;
    while(i.hasNext())
    {
        i.next();
        if (i.value() == deviceIndex)
            boxIndex = cntr;
        ui->devBox->addItem(i.key(), qVariantFromValue(i.value()));
        cntr++;
    }
    delete devs;
    ctrlsDisable(false);
    ui->devBox->setCurrentIndex(boxIndex);
    connect(ui->devBox, SIGNAL(currentIndexChanged(int)), this, SLOT(startListen(int)));
    ui->gateSlider->setValue(static_cast<int>(20 * log10(savedGate / sampleMax)));
    startListen(boxIndex);
}

void BlackBox::startListen(int index)
{
    if (inDSP)
        delete inDSP;
    dspSettings->setDeviceIndex(ui->devBox->itemData(index).value<int>());
    deviceIndex = dspSettings->getDeviceIndex();
    inDSP = new DSP(dspSettings, savedGate);
    inDSP->moveToThread(&mainThread);
    connect(&mainThread, SIGNAL(finished()), inDSP, SLOT(deleteLater()));
    connect(this, SIGNAL(startDSP()), inDSP, SLOT(start()));
    connect(inDSP, SIGNAL(peakMeasured(quint16)), this, SLOT(indicate(quint16)));
    connect(inDSP, SIGNAL(errorOccured(QString)), this, SLOT(displayError(QString)));
    connect(inDSP, SIGNAL(stateChanged(quint8,quint16)), this, SLOT(displayState(quint8,quint16)));
    connect(this, SIGNAL(thresholdSet(quint16)), inDSP, SLOT(setGate(quint16)));
    connect(ui->gateSlider, SIGNAL(valueChanged(int)), this, SLOT(setThreshold(int)));
    mainThread.start();
    emit startDSP();
}

void BlackBox::indicate(const quint16 &val)
{
    dBLevel = 20 * log10(val / sampleMax);
    ui->progressBar->setValue(static_cast<int>(dBLevel));
}

void BlackBox::setThreshold(int v)
{
    gate = pow(10, v / 20.0) * sampleMax;
    emit thresholdSet(static_cast<quint16>(gate));
}

void BlackBox::displayState(const quint8 &code, const quint16 &gateIn)
{
    savedGate = gateIn;
    inGate = 20 * log10(gateIn / sampleMax);
    ui->gateLabel->setText(QString::number(static_cast<qint8>(inGate)) + "dB");
    switch(code)
    {
    case DSPEngine::Idle:
        ui->progressBar->setStyleSheet("QProgressBar{text-align: right} "
                                       "QProgressBar::chunk {background-color: #aaaaaa}");
        ui->statusbar->showMessage("Listening...");
        break;
    case DSPEngine::Active:
        ui->progressBar->setStyleSheet("QProgressBar{text-align: right} "
                                       "QProgressBar::chunk {background-color: }");
        ui->statusbar->showMessage("Recording...");
        break;
    default:
        break;
    }
}
