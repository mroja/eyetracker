#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget * parent = 0);

    int heightForWidth(int width) const override;

public slots:
    void setPixmap(const QPixmap & pixmap);

private:
    void paintEvent(QPaintEvent * ev);

private:
    QPixmap m_pixmap;
    double m_ratio;
};

#endif // IMAGEVIEWER_H
