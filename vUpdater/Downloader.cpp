#include "Downloader.h"
#include "Vietnamese.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <cmath>

Downloader::Downloader(QWidget* parent)
	: QDialog(parent)
{
	setupUi(this);
	mManager = new QNetworkAccessManager;
	mDownloadDir.setPath(QDir::homePath() + "/Downloads/");
	mSuffix = ".part";

	connect(cancelButton, &QPushButton::clicked, this, &Downloader::cancelDownloading);
}

void Downloader::setUrls(QStringList urls)
{
	mUrls = std::move(urls);
}

QStringList Downloader::urls() const
{
	return mUrls;
}

QDir Downloader::downloadDir() const
{
	return mDownloadDir;
}

void Downloader::start()
{
	if (!mDownloadDir.exists() && !mDownloadDir.mkpath("."))
		return;
	
	removeOldFiles();
	if (!mUrls.empty())
	{
		showNormal();
		mCurrentIndex = 0;
		mAbort = false;
		QString url = mUrls.at(mCurrentIndex);
		downloadUrl(url);
	}
}

void Downloader::updateProgress(qint64 received, qint64 total) const
{
	if (total > 0)
	{
		progressBar->setValue(qint64(received * 100 / total));
		QString receivedSize = calculateSizes(received);
		QString totalSize = calculateSizes(total);
		QString timeRemaining = calculateTimeRemaining(received, total);
		messageLabel->setText(Vietnamese::str(L"Đang tải %1: %2 / %3").arg(mFileName).arg(receivedSize).arg(totalSize));
		timeRemainingLabel->setText(Vietnamese::str(L"Thời gian còn lại %1").arg(timeRemaining));
		saveFile();
	}
	else
	{
		progressBar->setMinimum(0);
		progressBar->setMaximum(0);
		progressBar->setValue(-1);
		messageLabel->setText(Vietnamese::str(L"Đang tải bản cập nhât"));
		timeRemainingLabel->setText(Vietnamese::str(L"Thời gian còn lại: Không xác định"));
	}
}

void Downloader::downloadUrlFinished()
{
	if (!mAbort)
	{
		QFile::rename(mDownloadDir.filePath(mFileName + mSuffix),
			mDownloadDir.filePath(mFileName));
		mReply->close();
		++mCurrentIndex;
		if (mCurrentIndex == mUrls.size())
		{
			hide();
			emit finished();
		}
		else
		{
			QString url = mUrls.at(mCurrentIndex);
			downloadUrl(url);
		}
	}
}

void Downloader::cancelDownloading()
{
	if (mReply->isFinished())
	{
		close();
	}
	else
	{
		if (QMessageBox::question(
			this, windowTitle(),
			Vietnamese::str(L"Bạn muốn thoát quá trình tải xuống?"))
			== QMessageBox::Yes)
		{
			mReply->abort();
			mAbort = true;
			removeOldFiles();
			close();
		}
	}
}

void Downloader::removeOldFiles()
{
	for (const QString& url : mUrls)
	{
		QString fileName = url.split("/").last();
		QFile::remove(mDownloadDir.filePath(fileName));
		QFile::remove(mDownloadDir.filePath(fileName + mSuffix));
	}
}

void Downloader::downloadUrl(const QString& url)
{
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setValue(0);
	mFileName = url.split("/").last();

	QNetworkRequest request(url);
	mReply = mManager->get(request);
	mStartTime = QDateTime::currentDateTime().toTime_t();
	connect(mReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));
	connect(mReply, SIGNAL(finished()), this, SLOT(downloadUrlFinished()));
}

void Downloader::saveFile() const
{
	QFile file(mDownloadDir.filePath(mFileName + mSuffix));
	if (file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		file.write(mReply->readAll());
		file.close();
	}
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