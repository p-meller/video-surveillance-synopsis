#ifndef VIDEOSYNOPSIS_PREVIEWIMAGEPROVIDER_H
#define VIDEOSYNOPSIS_PREVIEWIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QObject>

class PreviewImageProvider : public QObject, public QQuickImageProvider
{
Q_OBJECT
	QImage previewFgImage_;
	QImage previewContoursImage_;
	QImage previewDetectionsImage_;

public:
	PreviewImageProvider();

	QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

public slots:
	void updateFgImage(const QImage& previewImage);
	void updateContoursImage(const QImage& previewImage);
	void updateDetectionsImage(const QImage& previewImage);
};


#endif //VIDEOSYNOPSIS_PREVIEWIMAGEPROVIDER_H
