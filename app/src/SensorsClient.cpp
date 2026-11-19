#include "SensorsClient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <cstdio>

namespace {

constexpr int kRefreshMs = 2000;

QString socketPath() {
  const QByteArray runtimeDir = qgetenv("XDG_RUNTIME_DIR");
  const QString dir = runtimeDir.isEmpty() ? QStringLiteral("/tmp")
                                           : QString::fromLocal8Bit(runtimeDir);
  return dir + QStringLiteral("/sensors.sock");
}

void log(const char *label, const QString &message) {
  std::printf("[%s] %s\n", label, message.toLocal8Bit().constData());
  std::fflush(stdout);
}

} // namespace

SensorsClient::SensorsClient(QObject *parent) : QObject(parent) {
  connect(&m_socket, &QLocalSocket::connected, this,
          &SensorsClient::onConnected);
  connect(&m_socket, &QLocalSocket::disconnected, this,
          &SensorsClient::onDisconnected);
  connect(&m_socket, &QLocalSocket::readyRead, this,
          &SensorsClient::onReadyRead);
  connect(&m_socket, &QLocalSocket::errorOccurred, this,
          &SensorsClient::onErrorOccurred);

  m_timer.setInterval(kRefreshMs);
  connect(&m_timer, &QTimer::timeout, this, &SensorsClient::refresh);
  m_timer.start();

  m_socket.connectToServer(socketPath());
}

void SensorsClient::refresh() {
  if (m_socket.state() == QLocalSocket::ConnectedState) {
    m_socket.write("status\n");
  } else if (m_socket.state() == QLocalSocket::UnconnectedState) {
    m_socket.connectToServer(socketPath());
  }
}

void SensorsClient::setConnected(bool value) {
  if (m_connected != value) {
    m_connected = value;
    emit connectionChanged();
  }
}

void SensorsClient::onConnected() {
  log("info", QStringLiteral("socket connected"));
  m_buffer.clear();
  setConnected(true);
  m_socket.write("status\n");
}

void SensorsClient::onDisconnected() {
  log("info", QStringLiteral("socket disconnected"));
  m_buffer.clear();
  setConnected(false);
}

void SensorsClient::onErrorOccurred(QLocalSocket::LocalSocketError error) {
  log("error", QStringLiteral("socket error: %1 (%2)")
                   .arg(m_socket.errorString())
                   .arg(static_cast<int>(error)));
  emit connectionError(m_socket.errorString());
}

void SensorsClient::onReadyRead() {
  m_buffer.append(m_socket.readAll());

  qsizetype newline;
  while ((newline = m_buffer.indexOf('\n')) >= 0) {
    const QByteArray line = m_buffer.left(newline);
    m_buffer.remove(0, newline + 1);
    if (!line.trimmed().isEmpty()) {
      parseLine(line);
    }
  }
}

void SensorsClient::parseLine(const QByteArray &line) {
  QJsonParseError parseError;
  const QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
  if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
    log("error", QStringLiteral("bad json: %1: %2")
                     .arg(parseError.errorString(), QString::fromUtf8(line)));
    return;
  }

  const QJsonObject obj = doc.object();
  if (obj.contains(QStringLiteral("error"))) {
    log("error", QStringLiteral("daemon: %1")
                     .arg(obj.value(QStringLiteral("error")).toString()));
    return;
  }

  m_profile = obj.value(QStringLiteral("profile")).toString();
  m_cpu = obj.value(QStringLiteral("cpu")).toObject().toVariantMap();
  m_gpu = obj.value(QStringLiteral("gpu")).toArray().toVariantList();
  m_fans = obj.value(QStringLiteral("fans")).toArray().toVariantList();
  m_ram = obj.value(QStringLiteral("ram")).toObject().toVariantMap();
  emit statusChanged();
}
