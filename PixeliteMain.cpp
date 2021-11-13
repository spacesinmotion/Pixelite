
#include <QApplication>
#include <QSettings>

#include "Pixelite.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setApplicationName("Pixelite");
  a.setApplicationVersion("0.0");
  a.setOrganizationName("pjame");

  QSettings::setDefaultFormat(QSettings::IniFormat);

  Pixelite w;
  w.show();

  return a.exec();
}
