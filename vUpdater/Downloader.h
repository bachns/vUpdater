#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "ui_Downloader.h"

#include <QDir>
#include <QByteArray>

QT_BEGIN_NAMESPACE
class QNetworkReply;
class QNetworkAccessManager;
QT_END_NAMESPACE

class Downloader : public QDialog, Ui::Downloader
{
	Q_OBJECT

public:
	explicit Downloader(QWidget *parent = Q_NULLPTR);
	~Downloader() = default;
	void setDownloadUrls(const QStringList& urls);
	QStringList downloadUrls() const;
	QDir downloadDir() const;
	void start();

signals:
	void finished();
	
private slots:
	void updateProgress(qint64, qint64);
	void downloadUrlFinished();
	void cancelDownloading();
	
private:
	void download();
    QString calculateTimeRemaining (qint64 received, qint64 total) const;
	static QString calculateSizes (qint64 bytes);

	int mCurrentIndex = 0; //Tải xong một file, rồi mới tải tiếp
	QString mFileName;
	QStringList mDownloadUrls;
	QDir mDownloadDir;
	uint mStartTime = 0;
	QByteArray mDataDownloaded;
	QNetworkReply* mReply = nullptr;
	QNetworkAccessManager* mManager = nullptr;
};

#endif