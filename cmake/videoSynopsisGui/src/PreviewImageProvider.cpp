#include "PreviewImageProvider.h"

PreviewImageProvider::PreviewImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage PreviewImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
	return this->previewImage;
}

void PreviewImageProvider::updatePreviewImage(const QImage& previewImage)
{
	this->previewImage = previewImage;
}