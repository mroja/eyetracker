#ifndef PUPILDETECTORSETUPDIALOG_H
#define PUPILDETECTORSETUPDIALOG_H

#include <QDialog>
#include "pupildetector.h"

class PupilDetectorSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PupilDetectorSetupDialog(QWidget * parent = 0);
    ~PupilDetectorSetupDialog();

    void setVideoSource(PupilDetectingFrameReceiver * pupilDetector, int cameraIndex);

signals:
    void cameraIndexChanged(int);

protected:
    void showEvent(QShowEvent * event) override;
    void closeEvent(QCloseEvent * event) override;

private:
    Ui::CameraSetupForm m_gui;
    QPointer<PupilDetectingFrameReceiver> m_pupilDetector;
    const double m_marginCoeff = 100.0;
    const double m_contrastCoeff = 100.0;
    const double m_brightnessCoeff = 1.0;
    const double m_gammaCoeff = 100.0;
    const double m_oblatenessCoeff = 50.0;
};

#endif // PUPILDETECTORSETUPDIALOG_H
