#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "PreviewFilter.h"
#include "PreviewImageProvider.h"
#include "DetectorOutputTypeEnum.h"
#include "VideoSynopsisController.h"

int main(int argc, char* argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	//QDir().mkdir("preview");

	qmlRegisterType<PreviewFilter>("com.videoSynopsisGui.classes", 1, 0, "PreviewFilter");
	qmlRegisterType<PreviewImageProvider>("com.videoSynopsisGui.classes", 1, 0, "PreviewImageProvider");
	qmlRegisterType<VideoSynopsisController>("com.videoSynopsisGui.classes", 1, 0, "VideoSynopsisController");

	QQmlApplicationEngine engine;
	PreviewImageProvider* imageProvider = new PreviewImageProvider;

	engine.rootContext()->setContextProperty("imageProvider", imageProvider);
	engine.rootContext()->setContextProperty("appRootPath", "file://"+QDir::currentPath()+"/");
	engine.addImageProvider(QLatin1String("previewprovider"), imageProvider);
	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
			&app, [url](QObject* obj, const QUrl& objUrl)
			{
				if (!obj && url == objUrl)
				{
					QCoreApplication::exit(-1);
				}
			}, Qt::QueuedConnection);
	engine.load(url);

	return app.exec();
}
