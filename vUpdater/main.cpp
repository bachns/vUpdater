#include "MainWindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QString paramJson;
	if (argc == 2)
		paramJson = argv[1];
	else if (QFile::exists("param.json"))
		paramJson = "param.json";
	else if (QFile::exists("logs/param.json"))
		paramJson = "logs/param.json";

	if (paramJson.isEmpty())
	{
		printf("cannot find param.json file");
		return -1;
	}

	MainWindow w(paramJson);
	w.show();

	return QApplication::exec();
}
