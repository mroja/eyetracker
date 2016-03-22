
#include "pupildetectorsetupdialog.h"
#include "ui_camera_setup.h"

PupilDetectorSetupDialog::PupilDetectorSetupDialog(QWidget * parent)
    : QDialog(parent)
    , m_pupilDetector(nullptr)
{
    m_gui.setupUi(this);

    connect(m_gui.cameraIndexSpinBox,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), // isn't C++11 beautiful?
            [&](int value)
            {
                // do not emit cameraIndexChanged when no pupilDetector is connected
                if(m_pupilDetector)
                    emit cameraIndexChanged(value);
            }
    );

    connect(m_gui.mirrorCheckBox, &QCheckBox::stateChanged, [&](int value){
        if(m_pupilDetector)
            m_pupilDetector->setMirrored(value != 0);
    });

    connect(m_gui.equalizeHistogramCheckBox, &QCheckBox::stateChanged, [&](int value){
        if(m_pupilDetector)
            m_pupilDetector->setEqualizeHistogram(value != 0);
    });

    connect(m_gui.previewTypeComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [&](int index)
            {
                if(m_pupilDetector)
                {
                    PupilDetectingFrameReceiver::PreviewType previewType;
                    switch(index)
                    {
                        case 0:
                            previewType = PupilDetectingFrameReceiver::PreviewColor;
                            break;
                        case 1:
                            previewType = PupilDetectingFrameReceiver::PreviewGrayscale;
                            break;
                        case 2:
                            previewType = PupilDetectingFrameReceiver::PreviewThreshold;
                            break;
                        default:
                            previewType = PupilDetectingFrameReceiver::PreviewGrayscale;
                    }
                    m_pupilDetector->setPreviewType(previewType);
                }
            }
    );

    connect(m_gui.thresholdSlider, &QSlider::valueChanged, [&](int value){
        m_gui.thresholdValueLabel->setText(QString::number(value));
        if(m_pupilDetector)
            m_pupilDetector->setThreshold(value);
    });

    connect(m_gui.contrastSlider, &QSlider::valueChanged, [&](int value){
        const double v = double(value) / m_contrastCoeff;
        m_gui.contrastValueLabel->setText(QString::number(v, 'f', 1));
        if(m_pupilDetector)
            m_pupilDetector->setContrast(v);
    });

    connect(m_gui.brightnessSlider, &QSlider::valueChanged, [&](int value){
        const double v = double(value) / m_brightnessCoeff;
        m_gui.brightnessValueLabel->setText(QString::number(v, 'f', 0));
        if(m_pupilDetector)
            m_pupilDetector->setBrightness(v);
    });

    connect(m_gui.gammaSlider, &QSlider::valueChanged, [&](int value){
        const double v = double(value) / m_gammaCoeff;
        m_gui.gammaValueLabel->setText(QString::number(v, 'f', 1));
        if(m_pupilDetector)
            m_pupilDetector->setGamma(v);
    });

    connect(m_gui.marginTopSlider, &QSlider::valueChanged, [&](int value){
        const double v = double(value) / m_marginCoeff;
        m_gui.marginTopValueLabel->setText(QString::number(v, 'f', 1));
        if(m_pupilDetector)
            m_pupilDetector->setTopMargin(v);
    });

    connect(m_gui.marginBottomSlider, &QSlider::valueChanged, [&](int value){
        const double v = double(value) / m_marginCoeff;
        m_gui.marginBottomValueLabel->setText(QString::number(v, 'f', 1));
        if(m_pupilDetector)
            m_pupilDetector->setBottomMargin(v);
    });

    connect(m_gui.marginRightSlider, &QSlider::valueChanged, [&](int value){
        const double v = double(value) / m_marginCoeff;
        m_gui.marginRightValueLabel->setText(QString::number(v, 'f', 1));
        if(m_pupilDetector)
            m_pupilDetector->setRightMargin(v);
    });

    connect(m_gui.marginLeftSlider, &QSlider::valueChanged, [&](int value){
        const double v = double(value) / m_marginCoeff;
        m_gui.marginLeftValueLabel->setText(QString::number(v, 'f', 1));
        if(m_pupilDetector)
            m_pupilDetector->setLeftMargin(v);
    });

    connect(m_gui.pointsMinSlider, &QSlider::valueChanged, [&](int value){
        m_gui.pointMinValueLabel->setText(QString::number(value));
        if(m_pupilDetector)
            m_pupilDetector->setPointsMin(value);
    });

    connect(m_gui.pointsMaxSlider, &QSlider::valueChanged, [&](int value){
        m_gui.pointMaxValueLabel->setText(QString::number(value));
        if(m_pupilDetector)
            m_pupilDetector->setPointsMax(value);
    });

    connect(m_gui.oblatenessLowSlider, &QSlider::valueChanged, [&](int value){
        const double v = double(value) / m_oblatenessCoeff;
        m_gui.oblatenessLowValueLabel->setText(QString::number(v, 'f', 2));
        if(m_pupilDetector)
            m_pupilDetector->setOblatenessLow(v);
    });

    connect(m_gui.oblatenessHighSlider, &QSlider::valueChanged, [&](int value){
        const double v = double(value) / m_oblatenessCoeff;
        m_gui.oblatenessHighValueLabel->setText(QString::number(v, 'f', 2));
        if(m_pupilDetector)
            m_pupilDetector->setOblatenessHigh(v);
    });

    connect(m_gui.acceptButton, &QPushButton::clicked, [&](){
        accept();
        close();
    });

    connect(m_gui.exitButton, &QPushButton::clicked, [&](){
        reject();
        close();
    });
}

PupilDetectorSetupDialog::~PupilDetectorSetupDialog()
{
    setVideoSource(nullptr, 0); // disconnect from m_pupilDetector
}

void PupilDetectorSetupDialog::setVideoSource(PupilDetectingFrameReceiver * pupilDetector, int cameraIndex)
{
    if(m_pupilDetector)
        m_gui.cameraView->disconnect();

    m_pupilDetector = pupilDetector;

    if(!m_pupilDetector)
        return;

    connect(m_pupilDetector, &PupilDetectingFrameReceiver::imageReady, m_gui.cameraView, &ImageViewer::setPixmap);

    qDebug() << "Video source connected...";

    m_gui.cameraIndexSpinBox->setValue(cameraIndex);
    m_gui.mirrorCheckBox->setChecked(m_pupilDetector->mirrored());
    m_gui.equalizeHistogramCheckBox->setChecked(m_pupilDetector->equalizeHistogram());

    m_pupilDetector->setPreviewType(PupilDetectingFrameReceiver::PreviewGrayscale);
    m_gui.previewTypeComboBox->setCurrentIndex(1);

    m_gui.thresholdSlider->setValue(m_pupilDetector->threshold());
    m_gui.contrastSlider->setValue(int(m_pupilDetector->contrast() * m_contrastCoeff));
    m_gui.brightnessSlider->setValue(int(m_pupilDetector->brightness() * m_brightnessCoeff));
    m_gui.gammaSlider->setValue(int(m_pupilDetector->gamma() * m_gammaCoeff));

    m_gui.pointsMinSlider->setValue(m_pupilDetector->pointsMin());
    m_gui.pointsMaxSlider->setValue(m_pupilDetector->pointsMax());

    m_gui.marginTopSlider->setValue(int(m_pupilDetector->topMargin() * m_marginCoeff));
    m_gui.marginBottomSlider->setValue(int(m_pupilDetector->bottomMargin() * m_marginCoeff));
    m_gui.marginRightSlider->setValue(int(m_pupilDetector->rightMargin() * m_marginCoeff));
    m_gui.marginLeftSlider->setValue(int(m_pupilDetector->leftMargin() * m_marginCoeff));

    m_gui.oblatenessLowSlider->setValue(int(m_pupilDetector->oblatenessLow() * m_oblatenessCoeff));
    m_gui.oblatenessHighSlider->setValue(int(m_pupilDetector->oblatenessHigh() * m_oblatenessCoeff));
}

void PupilDetectorSetupDialog::showEvent(QShowEvent * event)
{
    Q_UNUSED(event);
    setResult(-1);
}

void PupilDetectorSetupDialog::closeEvent(QCloseEvent * event)
{
    Q_UNUSED(event);
    qDebug() << "Video source disconnected";
    setVideoSource(nullptr, 0);
    emit finished(result() == QDialog::Accepted);
}
