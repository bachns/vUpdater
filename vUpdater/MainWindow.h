#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"
#include "UpdateItem.h"

#include <QMap>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
class Downloader;
QT_END_NAMESPACE

class MainWindow : public QMainWindow, Ui::MainWindowClass
{
	Q_OBJECT

public:
	explicit MainWindow(QString paramJson, QWidget *parent = Q_NULLPTR);
	~MainWindow() = default;

protected:
	void closeEvent(QCloseEvent* event) override;

private slots:
	void checkForUpdate();
	void downloadUpdate();
	void onReply(QNetworkReply*);
	void downloadFinished();
	
private:
	static void copyFile(const QString& source, const QString& destination);
	static void copyDir(const QString& source, const QString& destination);
	void setSoftwareName(const QString& softwareName) const;
	void setCurrentVersion(const QString& version) const;
	void setLastVersion(const QString& version) const;
	void setChangelog(const QStringList& changelog) const;
	
	bool loadParamJson();
	void parseChange(const QJsonDocument& document, const QStringList& versionList);
	void loadLastVersionInfo(const QJsonDocument& document, const QString& version) const;
	QString mParamJson;
	QString mSoftwareName;
	QString mCurrentVersion;
	QString mReleaseDate;
	QString mJsonUrl;
	QMap<QString, UpdateItem> mUpdateItemMap;
	QStringList mDownloadUrls;
	
	QString mInstallPath;
	Downloader* mDownloader = nullptr;
	QNetworkAccessManager* mManager = nullptr;
	QNetworkReply* mReply = nullptr;
};

#endif
