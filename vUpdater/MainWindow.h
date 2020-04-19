#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
class Downloader;
QT_END_NAMESPACE

class MainWindow : public QMainWindow, Ui::MainWindowClass
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow() = default;
	void setParameters(QString name, QString currentVersion, QString jsonFile, QString installPath);

private slots:
	void checkForUpdate();
	void downloadUpdate() const;
	void onReply(QNetworkReply*);
	void downloadFinished();
	
private:
	void updateName() const;
	void setVersionInfo(const QString& info) const;
	void compareVersions();
	
	QString mName;
	QString mCurrentVersion;
	QString mJsonFile;
	QString mInstallPath;

	Downloader* mDownloader = nullptr;
	QNetworkAccessManager* mManager = nullptr;
	QNetworkReply* mReply = nullptr;
	QString mLatestVersion;
	QString mChangelog;
	QStringList mDownloadUrls;
	bool mNewVersionExists = false;
};

#endif
