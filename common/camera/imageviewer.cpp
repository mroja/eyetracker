#include "imageviewer.h"
#include <QPainter>

ImageViewer::ImageViewer(QWidget * parent)
    : QWidget(parent)
    , m_ratio(0.75)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    QSizePolicy p(sizePolicy());
    p.setHeightForWidth(true);
    setSizePolicy(p);
}

int ImageViewer::heightForWidth(int width) const
{
    return int(0.5 + m_ratio * width);
}

void ImageViewer::setPixmap(const QPixmap & pixmap)
{
    m_pixmap = pixmap;
    update();

    if(m_pixmap.isNull() || m_pixmap.width() <= 0 ||  m_pixmap.height() <= 0)
        return;

    const double ratio = double(m_pixmap.width()) / double(m_pixmap.height());
    if(m_ratio != ratio)
    {
        resize(width(), int(0.5 + m_ratio * width()));
        m_ratio = ratio;
    }
}

void ImageViewer::paintEvent(QPaintEvent * ev)
{
    Q_UNUSED(ev);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.drawPixmap(QRect(0, 0, width(), height()), m_pixmap,
                 QRect(0, 0, m_pixmap.width(), m_pixmap.height()));
}
