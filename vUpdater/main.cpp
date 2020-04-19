#include "MainWindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QString name, currentVersion, jsonFile, installPath;
	QStringList argvs;

	if (QFile::exists("logs/vUpdater.param"))
	{
		QFile paramFile("logs/vUpdater.param");
		if (paramFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream textStream(&paramFile);
			argvs = textStream.readLine().split(' ');
			paramFile.close();
		}
	}
	else
	{
		for (int i = 1; i < argc; ++i)
			argvs << argv[i];
	}

	for (int i = 0; i < argvs.size(); ++i)
	{
		if (argvs[i] == "--name" || argvs[i] == "-n")
		{
			name = argvs[i + 1];
			++i;
		}
		else if (argvs[i] == "--current" || argvs[i] == "-c")
		{
			currentVersion = argvs[i + 1];
			++i;
		}
		else if (argvs[i] == "--json" || argvs[i] == "-j")
		{
			jsonFile = argvs[i + 1];
			++i;
		}
		if (argvs[i] == "--install" || argvs[i] == "-i")
		{
			installPath = argvs[i + 1];
			++i;
		}
	}

	if (name.isEmpty() || jsonFile.isEmpty() || currentVersion.isEmpty() || installPath.isEmpty())
		return -1;

	MainWindow w;
	w.setParameters(name, currentVersion, jsonFile, installPath);
	w.show();
	return QApplication::exec();
}
