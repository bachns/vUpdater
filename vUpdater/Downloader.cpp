#include "Downloader.h"
#include "Vietnamese.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QSaveFile>

#include <cmath>

Downloader::Downloader(QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);
	mManager = new QNetworkAccessManager;
	mDownloadDir.setPath(QDir::homePath() + "/Downloads/vUpdater");
	connect(cancelButton, &QPushButton::clicked, this, &Downloader::cancelDownloading);
}

void Downloader::setDownloadUrls(const QStringList& urls)
{
	mDownloadUrls = urls;
}

QStringList Downloader::downloadUrls() const
{
	return mDownloadUrls;
}

QDir Downloader::downloadDir() const
{
	return mDownloadDir;
}

void Downloader::start()
{
	if (!mDownloadDir.exists() && !mDownloadDir.mkpath("."))
		return;
	mCurrentIndex = 0;
	download();
}

void Downloader::updateProgress(qint64 received, qint64 total)
{
	progressBar->setValue(qint64(received * 100 / total));
	QString receivedSize = calculateSizes(received);
	QString totalSize = calculateSizes(total);
	QString timeRemaining = calculateTimeRemaining(received, total);
	messageLabel->setText(Vietnamese::str(L"Đang tải %1: %2 / %3").arg(mFileName).arg(receivedSize).arg(totalSize));
	timeRemainingLabel->setText(Vietnamese::str(L"Thời gian còn lại %1").arg(timeRemaining));
	mDataDownloaded.append(mReply->readAll());
}

void Downloader::downloadUrlFinished()
{
	QFile file(mDownloadDir.filePath(mFileName));
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(mDataDownloaded);
		file.close();
		
	}

	//tăng chỉ số để chuyển sang tải file tiếp theo
	mCurrentIndex++;
	//Đã tải xong
	if (mCurrentIndex == mDownloadUrls.size())
	{
		emit finished(); //thông báo hoàn thành
	}
	else
	{
		download(); //lặp lại quá trình download
	}
}

void Downloader::cancelDownloading()
{
	if (QMessageBox::question(this, windowTitle(),
		Vietnamese::str(L"Bạn muốn thoát quá trình tải xuống?"))
		== QMessageBox::Yes)
	{
		mReply->abort();
		mDownloadDir.removeRecursively();
		close();
	}
}

void Downloader::download()
{
	//Đã download xong
	if (mCurrentIndex >= mDownloadUrls.size())
		return;

	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setValue(0);
	mDataDownloaded.clear();
	
	const QString& url = mDownloadUrls.at(mCurrentIndex);
	mFileName = url.split("/").last();
	
	QNetworkRequest request(url);
	mReply = mManager->get(request);
	mStartTime = QDateTime::currentDateTime().toTime_t();
	connect(mReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));
	connect(mReply, SIGNAL(finished()), this, SLOT(downloadUrlFinished()));
}

QString Downloader::calculateSizes(qint64 bytes)
{
	if (bytes < 1024)
		return QString("%1 bytes").arg(bytes);

	if (bytes < 1048576)
		return QString("%1 KB").arg(bytes / 1024);

	return QString("%1 MB").arg(bytes / 1048576);
}

QString Downloader::calculateTimeRemaining(qint64 received, qint64 total) const
{
	uint difference = QDateTime::currentDateTime().toTime_t() - mStartTime;
	QString timeString = Vietnamese::str(L": không xác định");
	if (difference > 0)
	{
		double speed = 1.0 * received / difference;
		double timeRemaining = (1.0 * total - received) / speed;
		if (timeRemaining > 7200)
		{
			timeRemaining /= 3600;
			int hours = int(std::ceil(timeRemaining));

			if (hours > 1)
				timeString = Vietnamese::str(L"khoảng %1 giờ").arg(hours);
			else
				timeString = Vietnamese::str(L"khoảng 1 giờ").arg(hours);
		}
		else if (timeRemaining > 60)
		{
			timeRemaining /= 60;
			int minutes = int(std::ceil(timeRemaining));

			if (minutes > 1)
				timeString = Vietnamese::str(L"khoảng %1 phút").arg(minutes);
			else
				timeString = Vietnamese::str(L"khoảng 1 phút");
		}
		else if (timeRemaining <= 60)
		{
			int seconds = int(std::ceil(timeRemaining));

			if (seconds > 1)
				timeString = Vietnamese::str(L"%1 giây").arg(seconds);
			else
				timeString = Vietnamese::str(L"1 giây");
		}
	}
	return timeString;
}