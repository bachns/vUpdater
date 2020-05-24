#include "MainWindow.h"
#include "Vietnamese.h"
#include "Downloader.h"

#include "quazip5/JlCompress.h"

#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QTimer>
#include <QCloseEvent>

#include <utility>

MainWindow::MainWindow(QString paramJson, QWidget* parent)
	: QMainWindow(parent), mParamJson(std::move(paramJson))
{
	setupUi(this);
	setFixedSize(width(), height());
	mDownloader = new Downloader(this);
	mManager = new QNetworkAccessManager;

	connect(mDownloader, &Downloader::finished, this, &MainWindow::downloadFinished);
	connect(mManager, &QNetworkAccessManager::finished, this, &MainWindow::onReply);
	connect(cancelButton, &QPushButton::clicked, this, &QMainWindow::close);
	connect(checkButton, &QPushButton::clicked, this, &MainWindow::checkForUpdate);
	connect(downloadButton, &QPushButton::clicked, this, &MainWindow::downloadUpdate);

	if (!loadParamJson())
	{
		QMessageBox::critical(this, "vUpdater",
			Vietnamese::str(L"Lỗi truyền sai hoặc thiếu tham số."));
		QTimer::singleShot(0, [this] { close(); });
	}
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	//bảo đảm xóa các tệp đã download trước khi thoát
	mDownloader->downloadDir().removeRecursively();
	event->accept();
}

void MainWindow::checkForUpdate()
{
	QUrl url(mJsonUrl);
	QNetworkRequest request(url);
	mReply = mManager->get(request);
}

void MainWindow::onReply(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::HostNotFoundError)
	{
		QMessageBox::critical(this, "vUpdater",
			Vietnamese::str(L"Lỗi kết nối tới máy chủ. Hãy kiểm tra kết nối và thử lại."));
		return;
	}

	if (reply->error() != QNetworkReply::NoError)
	{
		QString message = reply->readAll();
		QMessageBox::critical(this, "vUpdater",
			Vietnamese::str(L"Cập nhật gặp lỗi %1: %2\n%3.")
			.arg(reply->error())
			.arg(reply->errorString())
			.arg(message));
		return;
	}

	QJsonDocument updateDocument = QJsonDocument::fromJson(reply->readAll());
	QJsonObject mainObject = updateDocument.object();
	QStringList versionList = mainObject.keys();
	while (versionList.first().compare(mCurrentVersion) <= 0)
	{
		versionList.removeFirst();
	}

	if (!versionList.isEmpty())
	{
		loadLastVersionInfo(updateDocument, versionList.last());
		parseChange(updateDocument, versionList);
		downloadButton->setEnabled(true);
	}
	else
	{
		lastVersionLineEdit->setText(currentVersionLineEdit->text());
		setChangelog({ Vietnamese::str(L"Phiên bản hiện tại đã là mới nhất!") });
	}
}

void MainWindow::downloadUpdate()
{
	mDownloadUrls.clear();
	QList<UpdateItem> updateItems = mUpdateItemMap.values();
	for (const UpdateItem& item : updateItems)
		if (item.mAction == UpdateItem::Action::Add && !item.mUrl.isEmpty())
			mDownloadUrls.append(item.mUrl);
	if (!mDownloadUrls.isEmpty())
	{
		mDownloader->setDownloadUrls(mDownloadUrls);
		mDownloader->showNormal();
		mDownloader->start();
	}
	else
	{
		//Không download gì thì không báo tải xong luôn
		//để thực hiện các lệnh thay đổi khác, như Remove
		downloadFinished();
	}
}

void MainWindow::downloadFinished()
{
	mDownloader->close();
	if (QMessageBox::question(this, "vUpdater",
		Vietnamese::str(L"Đã tải xong, bạn có muốn cài đặt ngay không?"))
		== QMessageBox::Yes)
	{
		QDir installDir(mInstallPath);
		QDir downloadDir = mDownloader->downloadDir();

		//Kiểm tra số lượng
		QStringList downloaded = downloadDir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
		if (downloaded.size() != mDownloadUrls.size())
		{
			QMessageBox::critical(this, "vUpdater",
				Vietnamese::str(L"Tải về chưa đủ số lượng các tệp tin."));
			return;
		}

		//Kiểm tra tên
		QList<UpdateItem> updateItems = mUpdateItemMap.values();
		for (const UpdateItem& item : updateItems)
		{
			if (item.mAction == UpdateItem::Action::Add)
			{
				QString name = item.mUrl.split("/").last();
				if (downloaded.contains(name) == 0)
				{
					QMessageBox::critical(this, "vUpdater",
						Vietnamese::str(L"Tải về không đúng tệp tin."));
					return;
				}
			}
		}

		//Thực hiện install update
		for (const UpdateItem& item : updateItems)
		{
			QString destination = installDir.absolutePath() + QDir::separator() + item.mTarget;
			if (item.mAction == UpdateItem::Action::Add)
			{
				QString name = item.mUrl.split("/").last();
				QString source = downloadDir.absolutePath() + QDir::separator() + name;
				QFileInfo fileInfo(source);
				if (fileInfo.suffix() != "zip")
				{
					copyFile(source, destination);
				}
				else
				{
					//giải nén sau đó copy thư mục
					JlCompress::extractDir(source, downloadDir.absolutePath());
					QString ss = downloadDir.absolutePath() + QDir::separator() + fileInfo.baseName();
					copyDir(ss, destination);
				}
			}
			else if (item.mAction == UpdateItem::Action::Remove)
			{
				if (QFileInfo(destination).isFile())
					QFile::remove(destination);
				else
					QDir(destination).removeRecursively();
			}
		}
		QMessageBox::information(this, "vUpdater",
			Vietnamese::str(L"Đã hoàn thành cập nhật %1 %2").arg(softwareNameLineEdit->text()).arg(lastVersionLineEdit->text()));
		close();
	}
}

void MainWindow::setSoftwareName(const QString& softwareName) const
{
	softwareNameLineEdit->setText(softwareName);
}

void MainWindow::setCurrentVersion(const QString& version) const
{
	currentVersionLineEdit->setText(version);
}

void MainWindow::setLastVersion(const QString& version) const
{
	lastVersionLineEdit->setText(version);
}

void MainWindow::setChangelog(const QStringList& changelog) const
{
	changelogTextEdit->clear();
	for (const QString& message : changelog)
	{
		changelogTextEdit->append("* " + message);
	}
}

void MainWindow::copyFile(const QString& source, const QString& destination)
{
	QFileInfo fileInfo(destination);
	QDir dir = fileInfo.dir();
	if (!dir.exists() && !dir.mkpath("."))
		return;

	QFile::copy(source, destination);
}

void MainWindow::copyDir(const QString& source, const QString& destination)
{
	QDir dir(source);
	if (!dir.exists())
		return;

	for (const QString& subDir : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		QString subDestination = destination + QDir::separator() + subDir;
		if (dir.mkpath(subDestination))
			copyDir(source + QDir::separator() + subDir, subDestination);
	}

	for (const QString& file : dir.entryList(QDir::Files))
	{
		copyFile(source + QDir::separator() + file, destination + QDir::separator() + file);
	}
}

void MainWindow::loadLastVersionInfo(const QJsonDocument& document, const QString& version) const
{
	QJsonObject lastVersionObject = document.object().value(version).toObject();
	QString releaseDate = lastVersionObject.value("release-date").toString();
	QJsonArray changelogArray = lastVersionObject.value("changelog").toArray();
	QStringList changelog;
	for (QJsonValue value : changelogArray)
		changelog.append(value.toString());

	lastVersionLineEdit->setText(QString("%1 (%2)").arg(version).arg(releaseDate));
	setChangelog(changelog);
}

bool MainWindow::loadParamJson()
{
	QFile file(mParamJson);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	QJsonDocument document = QJsonDocument::fromJson(file.readAll());
	if (document.isNull())
		return false;

	mSoftwareName = document.object().value("software-name").toString();
	mCurrentVersion = document.object().value("current-version").toString();
	mReleaseDate = document.object().value("release-date").toString();
	mInstallPath = document.object().value("install-path").toString();
	mJsonUrl = document.object().value("json-url").toString();
	if (mSoftwareName.isEmpty() || mCurrentVersion.isEmpty())
		return false;

	if (mJsonUrl.isEmpty())
		mJsonUrl = QString("https://raw.githubusercontent.com/bachns/vUpdaterStorage/master/%1/update.json").arg(mSoftwareName);

	if (mInstallPath.isEmpty())
		mInstallPath = QString(".");

	softwareNameLineEdit->setText(mSoftwareName);
	currentVersionLineEdit->setText(QString("%1 (%2)").arg(mCurrentVersion).arg(mReleaseDate));
	return true;
}

void MainWindow::parseChange(const QJsonDocument& document, const QStringList& versionList)
{
	mUpdateItemMap.clear();
	QJsonObject mainObject = document.object();
	for (const QString& version : versionList)
	{
		QJsonObject versionObject = mainObject.value(version).toObject();
		QJsonObject changeObject = versionObject.value("change").toObject();

		QStringList fileNames = changeObject.keys();
		for (const QString& fileName : fileNames)
		{
			QJsonObject fileObject = changeObject.value(fileName).toObject();
			mUpdateItemMap.insert(fileName, UpdateItem(fileObject));
		}
	}
}
