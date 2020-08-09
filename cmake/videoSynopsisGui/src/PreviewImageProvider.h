#ifndef VIDEOSYNOPSIS_PREVIEWIMAGEPROVIDER_H
#define VIDEOSYNOPSIS_PREVIEWIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QObject>

class PreviewImageProvider : public QObject, public QQuickImageProvider
{
Q_OBJECT
	QImage previewImage_;

public:
	PreviewImageProvider();

	QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

public slots:

	void updatePreviewImage(const QImage& previewImage);
};


#endif //VIDEOSYNOPSIS_PREVIEWIMAGEPROVIDER_H
