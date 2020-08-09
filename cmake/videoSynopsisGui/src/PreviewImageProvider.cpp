#include "PreviewImageProvider.h"

PreviewImageProvider::PreviewImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage PreviewImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
	if (id == "fg")
	{
		return this->previewFgImage_;
	}
	else if (id == "contours")
	{
		return this->previewContoursImage_;
	}
	else if (id == "detections")
	{
		return this->previewDetectionsImage_;
	}
	return QImage();
}

void PreviewImageProvider::updateFgImage(const QImage& previewImage)
{
	this->previewFgImage_ = previewImage;
}

void PreviewImageProvider::updateContoursImage(const QImage& previewImage)
{
	this->previewContoursImage_ = previewImage;
}

void PreviewImageProvider::updateDetectionsImage(const QImage& previewImage)
{
	this->previewDetectionsImage_ = previewImage;
}
