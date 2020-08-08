#ifndef VIDEOSYNOPSIS_PREVIEWFILTER_H
#define VIDEOSYNOPSIS_PREVIEWFILTER_H


#include <QtMultimedia/QAbstractVideoFilter>
#include <QDebug>
#include <QTemporaryFile>
#include <QMutex>

#include "videoSynopsisLib.h"

class PreviewFilter : public QAbstractVideoFilter
{
Q_OBJECT
public:
    QVideoFilterRunnable *createFilterRunnable();

signals:
    void imageUpdate(const QImage& previewImage);

public slots:

};

class QCvDetectFilterRunnable : public QVideoFilterRunnable
{
public:
    QCvDetectFilterRunnable(PreviewFilter *creator) { filter = creator;}
    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags);

private:
    //void dft(cv::InputArray input, cv::OutputArray output);
    PreviewFilter *filter;
    Detector detector;
};

#endif //VIDEOSYNOPSIS_PREVIEWFILTER_H
