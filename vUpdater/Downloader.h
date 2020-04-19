#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "ui_Downloader.h"

#include <QDir>

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
	void setUrls(QStringList urls);
	QStringList urls() const;
	QDir downloadDir() const;
	void start();

signals:
	void finished();
	
private slots:
	void updateProgress(qint64, qint64) const;
	void downloadUrlFinished();
	void cancelDownloading();
	
private:
	void removeOldFiles();
	void downloadUrl(const QString& url);
	void saveFile() const;
    QString calculateTimeRemaining (qint64 received, qint64 total) const;
	static QString calculateSizes (qint64 bytes);
	
	QStringList mUrls;
	QString mFileName;
	int mCurrentIndex = 0;
	QNetworkAccessManager* mManager = nullptr;
	QNetworkReply* mReply = nullptr;
	QDir mDownloadDir;
	QString mSuffix;
	uint mStartTime = 0;
	bool mAbort = false;
};

#endif