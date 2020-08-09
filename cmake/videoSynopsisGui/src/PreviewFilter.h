#ifndef VIDEOSYNOPSIS_PREVIEWFILTER_H
#define VIDEOSYNOPSIS_PREVIEWFILTER_H


#include <QtMultimedia/QAbstractVideoFilter>
#include <memory>

class Detector;

class PreviewFilter : public QAbstractVideoFilter
{
Q_OBJECT
public:
	QVideoFilterRunnable* createFilterRunnable() override;

signals:
	void imageUpdate(const QImage& previewImage);

public slots:

};

class QCvPreviewFilterRunnable : public QVideoFilterRunnable
{
public:
	explicit QCvPreviewFilterRunnable(PreviewFilter* creator);

	QVideoFrame run(QVideoFrame* input, const QVideoSurfaceFormat& surfaceFormat, RunFlags flags) override;

private:
	PreviewFilter* filter;
	std::unique_ptr<Detector> detector;
};

#endif //VIDEOSYNOPSIS_PREVIEWFILTER_H
