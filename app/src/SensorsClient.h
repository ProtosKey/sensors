#pragma once

#include <QByteArray>
#include <QLocalSocket>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>

class SensorsClient : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool connected READ connected NOTIFY connectionChanged)
  Q_PROPERTY(QString profile READ profile NOTIFY statusChanged)
  Q_PROPERTY(QVariantMap cpu READ cpu NOTIFY statusChanged)
  Q_PROPERTY(QVariantList gpu READ gpu NOTIFY statusChanged)
  Q_PROPERTY(QVariantList fans READ fans NOTIFY statusChanged)
  Q_PROPERTY(QVariantMap ram READ ram NOTIFY statusChanged)

public:
  explicit SensorsClient(QObject *parent = nullptr);

  bool connected() const { return m_connected; }
  QString profile() const { return m_profile; }
  QVariantMap cpu() const { return m_cpu; }
  QVariantList gpu() const { return m_gpu; }
  QVariantList fans() const { return m_fans; }
  QVariantMap ram() const { return m_ram; }

public slots:
  void refresh();

signals:
  void statusChanged();
  void connectionChanged();
  void connectionError(const QString &message);

private:
  void onConnected();
  void onDisconnected();
  void onErrorOccurred(QLocalSocket::LocalSocketError error);
  void onReadyRead();
  void parseLine(const QByteArray &line);
  void setConnected(bool value);

  QLocalSocket m_socket;
  QTimer m_timer;
  QByteArray m_buffer;

  bool m_connected = false;
  QString m_profile;
  QVariantMap m_cpu;
  QVariantList m_gpu;
  QVariantList m_fans;
  QVariantMap m_ram;
};
