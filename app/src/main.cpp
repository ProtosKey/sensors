#include "SensorsClient.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  app.setApplicationName(QStringLiteral("sensors"));
  app.setDesktopFileName(QStringLiteral("sensors"));

  SensorsClient client;
  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty(QStringLiteral("socket"), &client);
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  if (engine.rootObjects().isEmpty()) {
    return -1;
  }

  return app.exec();
}
