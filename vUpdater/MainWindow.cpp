#include "MainWindow.h"
#include "Vietnamese.h"
#include "Downloader.h"

#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	setupUi(this);
	setFixedSize(width(), height());
	mDownloader = new Downloader(this);
	connect(mDownloader, &Downloader::finished, this, &MainWindow::downloadFinished);
	
	mManager = new QNetworkAccessManager;
	connect(mManager, &QNetworkAccessManager::finished, this, &MainWindow::onReply);
	connect(cancelButton, &QPushButton::clicked, this, &QMainWindow::close);
	connect(checkButton, &QPushButton::clicked, this, &MainWindow::checkForUpdate);
	connect(downloadButton, &QPushButton::clicked, this, &MainWindow::downloadUpdate);
}

void MainWindow::setParameters(QString name, QString currentVersion, QString jsonFile, QString installPath)
{
	mName = std::move(name);
	mCurrentVersion = std::move(currentVersion);
	mJsonFile = std::move(jsonFile);
	mInstallPath = std::move(installPath);

	updateName();
	setVersionInfo(Vietnamese::str(L"Phiên bản hiện tại: %1").arg(mCurrentVersion));
}

void MainWindow::checkForUpdate()
{
	QUrl url(mJsonFile);
	QNetworkRequest request(url);
	mReply = mManager->get(request);
}

void MainWindow::downloadUpdate() const
{
	if (mNewVersionExists)
	{
		mDownloader->setUrls(mDownloadUrls);
		mDownloader->start();
	}
}

void MainWindow::onReply(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::HostNotFoundError)
	{
		QMessageBox::critical(this, Vietnamese::str(L"Lỗi kết nối tới máy chủ"),
			Vietnamese::str(L"Hãy kiểm tra kết nối mạng, sau đó thử lại."));
		return;
	}

	if (reply->error() != QNetworkReply::NoError)
	{
		QMessageBox::critical(this, Vietnamese::str(L"Lỗi cập nhật"),
			Vietnamese::str(L"Không thể cập nhật do gặp lỗi: %1").arg(reply->error()));
		return;
	}

	QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
	if (document.isNull())
	{
		QMessageBox::warning(this, Vietnamese::str(L"Lỗi cập nhật"),
			Vietnamese::str(L"Không tìm thấy thông tin về bản cập nhật"));
		return;
	}

	QJsonObject updates = document.object().value(mName).toObject();
	mLatestVersion = updates.value("latest-version").toString();
	mChangelog = updates.value("changelog").toString();
	
	QJsonObject downloads = updates.value("downloads").toObject();
	mDownloadUrls.clear();
	for (const QString& key : downloads.keys())
		mDownloadUrls << downloads.value(key).toString();

	compareVersions();
}

void MainWindow::downloadFinished()
{
	if (QMessageBox::question(this, Vietnamese::str(L"Bản cập nhật đã tải xong"),
		Vietnamese::str(L"Bạn có muốn cài đặt ngay bây giờ không?"))
		== QMessageBox::Yes)
	{
		QDir installDir;
		installDir.setPath(mInstallPath);
		QDir downloadDir = mDownloader->downloadDir();
		for (const QString& url : mDownloader->urls())
		{
			QString fileName = url.split("/").last();
			QFile::remove(installDir.filePath(fileName));
			QFile::copy(downloadDir.filePath(fileName), installDir.filePath(fileName));
			//QFile::remove(downloadDir.filePath(fileName));
		}
		QMessageBox::information(this, Vietnamese::str(L"Cài đặt bản cập nhật xong"), 
			Vietnamese::str(L"Quá trình cập nhật cho %1 đã hoàn thành").arg(mName));
		QApplication::exit();
	}
}

void MainWindow::updateName() const
{
	titleLabel->setText(Vietnamese::str(L"Cập nhật phần mềm %1").arg(mName));
}

void MainWindow::setVersionInfo(const QString& info) const
{
	updateInfoLabel->setText(info);
}

void MainWindow::compareVersions()
{
	QStringList currentVersion = mCurrentVersion.split(".");
	QStringList lastestVersion = mLatestVersion.split(".");

	int index = 0;
	mNewVersionExists = false;
	while (!mNewVersionExists && index < currentVersion.size() && index < lastestVersion.size())
	{
		if (currentVersion[index].toInt() < lastestVersion[index].toInt())
			mNewVersionExists = true;
		++index;
	}

	if (!mNewVersionExists)
		mNewVersionExists = lastestVersion.size() > currentVersion.size();
	
	if(mNewVersionExists)
	{
		setVersionInfo(Vietnamese::green(L"Phiên bản mới: %1").arg(mLatestVersion));
		changelogTextEdit->setText(mChangelog);
	}
	else
	{
		changelogTextEdit->setText(Vietnamese::str(L"Không có bản cập nhật nào, đây đã là bản mới nhất."));
	}
}