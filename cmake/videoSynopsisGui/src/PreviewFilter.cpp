#include "PreviewFilter.h"
#include "videoSynopsisLib.h"


#include <QDebug>
#include <opencv2/imgproc.hpp>
#include <memory>

QCvPreviewFilterRunnable::QCvPreviewFilterRunnable(PreviewFilter *creator) : filter(creator),
                                                                             detector(std::make_unique<Detector>()) {}

QVideoFilterRunnable *PreviewFilter::createFilterRunnable() {
    return new QCvPreviewFilterRunnable(this);
}

QVideoFrame
QCvPreviewFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags) {
    Q_UNUSED(flags);
    input->map(QAbstractVideoBuffer::ReadOnly);


    auto form = input->pixelFormat();

    if (surfaceFormat.handleType() == QAbstractVideoBuffer::NoHandle) {
        cv::Mat picYUV420 = cv::Mat(input->height() * 3 / 2, input->width(), CV_8UC1, input->bits());

        if (!picYUV420.empty()) {
            cv::Mat picBGR;
            cv::cvtColor(picYUV420, picBGR, cv::COLOR_YUV2BGR_I420);
            detector->processFrame(picBGR);
            cv::Mat outputFrame = detector->getOutput();
            //cv::imwrite("/home/piotr/test.png", outputFrame);

            //cv::cvtColor(outputFrame, picYUV420, cv::COLOR_BGR2YUV_I420);

            //outputFrame = picBGR;
            QImage image = QImage((const uchar *) outputFrame.data, outputFrame.cols, outputFrame.rows,
                                  outputFrame.step,
                                  QImage::Format_BGR888);

            emit filter->imageUpdate(image.copy());
            //image.bits();


        }

    } else {
        qDebug() << "Other surface formats are not supported yet!";
    }

    input->unmap();
    return *input;
}
