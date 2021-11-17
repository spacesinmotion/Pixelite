
#include "Pixelite.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setApplicationName("Pixelite");
  a.setApplicationVersion("0.0");
  a.setOrganizationName("pjame");

  a.setWindowIcon(QIcon(":icons/Pixelite.png"));

  QSettings::setDefaultFormat(QSettings::IniFormat);

  Pixelite w;
  w.show();

  return a.exec();
}
