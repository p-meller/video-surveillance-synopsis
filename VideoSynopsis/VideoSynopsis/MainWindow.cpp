#include "MainWindow.h"
#include <QPixmap>
#include <QFileDialog>
#include <QtMultimedia/QVideoProbe>
#include <QtMultimedia/qabstractvideosurface.h>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), player(new QMediaPlayer(this))
{
	ui.setupUi(this);
	//statusBar()->showMessage("fdsfdsfsdf");
	//worker->moveToThread(computingThread);

	player->setVideoOutput(ui.videoWidget);

	QObject::connect(ui.actionLoad_video, &QAction::triggered, this, &MainWindow::loadImage_click);
	QObject::connect(ui.pushButtonPlay, &QPushButton::clicked, [=]() {
		//controller.startPreview();
		player->play();
		});
	QObject::connect(ui.pushButtonPause, &QPushButton::clicked, [=]() {
		player->pause();
		controller.pausePreview();
		});
	QObject::connect(&controller, &ComputingController::renderImage, this, &MainWindow::renderedImage);

	QObject::connect(player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &MainWindow::errorSlot);

	for each (auto item in vs::FrameType::ToArray())
	{
		ui.comboBoxFrameType->addItem(vs::FrameType::ToString(item).c_str());
	}
	QObject::connect(ui.comboBoxFrameType, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
		controller.setFrameType(static_cast<vs::FrameType::FrameTypeEnum>(index));
		});

	QVideoProbe* probe = new QVideoProbe(this);

	probe->setSource(player);

	QObject::connect(probe, &QVideoProbe::videoFrameProbed, &controller, &ComputingController::processFrame);
	QObject::connect(&controller, &ComputingController::procesFrameFinished, this, &MainWindow::procesFrameFinished);

}

MainWindow::~MainWindow()
{

}

void MainWindow::loadImage_click()
{
	QString filename = QFileDialog::getOpenFileName(this,
		tr("LoadVideo"),
		QDir::currentPath(),
		tr("Video file (*.mp4 *.avi);;All files (*.*)"));

	if (!filename.isNull() && !filename.isEmpty())
	{
		//controller.loadVideo(filename);
		player->setMedia(QUrl::fromLocalFile(filename));
	}
}

void MainWindow::renderedImage(const QImage& image, int frame)
{
	QPixmap qPixmap = QPixmap::fromImage(image);
	//int max_width = ui.labelPreview->width();
	//int max_height = ui.labelPreview->height();
	//ui.labelPreview->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	//ui.labelPreview->setPixmap(qPixmap.scaled(max_width, max_height, Qt::KeepAspectRatio));
}

void MainWindow::errorSlot(QMediaPlayer::Error playerError)
{
	QMediaPlayer::Error e = playerError;
}

void MainWindow::procesFrameFinished(const QImage& img)
{
	ui.videoWidget->videoSurface()->present(img);
}


