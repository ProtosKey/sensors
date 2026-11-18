import json
import os
import sys
from pathlib import Path

from PySide6.QtCore import Property, QObject, QTimer, QUrl, Signal, Slot
from PySide6.QtNetwork import QLocalSocket
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtWidgets import QApplication

REFRESH_TIME = 2000
SOCKET_PATH = os.path.join(
    os.environ.get("XDG_RUNTIME_DIR") or "/tmp", "sensors.sock"
)


def log(label, message):
    print(f"[{label}] {message}")


def parse_json(raw: bytes):
    try:
        return json.loads(raw)
    except json.JSONDecodeError as e:
        log("error", f"bad json: {e}: {raw!r}")
        return None


class Socket(QObject):
    statusChanged = Signal()
    connectionChanged = Signal()
    connectionError = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)

        self._connected = False
        self._profile = ""
        self._cpu = {}
        self._gpu = []
        self._fans = []
        self._ram = {}
        self._buffer = b""

        self._socket = QLocalSocket(self)
        self._socket.connected.connect(self._on_connected)
        self._socket.disconnected.connect(self._on_disconnected)
        self._socket.readyRead.connect(self._on_ready_read)
        self._socket.errorOccurred.connect(self._on_socket_error)

        self._timer = QTimer(self)
        self._timer.setInterval(REFRESH_TIME)
        self._timer.timeout.connect(self.refresh)
        self._timer.start()

        self._socket.connectToServer(SOCKET_PATH)

    def _get_connected(self): return self._connected
    def _get_profile(self):   return self._profile
    def _get_cpu(self):       return self._cpu
    def _get_gpu(self):       return self._gpu
    def _get_fans(self):      return self._fans
    def _get_ram(self):       return self._ram

    connected = Property(bool,           _get_connected, notify=connectionChanged) # type: ignore
    profile   = Property(str,            _get_profile,   notify=statusChanged)     # type: ignore
    cpu       = Property("QVariantMap",  _get_cpu,       notify=statusChanged)     # type: ignore
    gpu       = Property("QVariantList", _get_gpu,       notify=statusChanged)     # type: ignore
    fans      = Property("QVariantList", _get_fans,      notify=statusChanged)     # type: ignore
    ram       = Property("QVariantMap",  _get_ram,       notify=statusChanged)     # type: ignore

    def _is_connected(self):
        return self._socket.state() == QLocalSocket.LocalSocketState.ConnectedState

    def _set_connected(self, value: bool):
        if self._connected != value:
            self._connected = value
            self.connectionChanged.emit()

    @Slot()
    def refresh(self):
        if self._is_connected():
            self._socket.write(b"status\n")
        elif self._socket.state() == QLocalSocket.LocalSocketState.UnconnectedState:
            self._socket.connectToServer(SOCKET_PATH)

    def _on_connected(self):
        log("info", "socket connected")
        self._buffer = b""
        self._set_connected(True)
        self._socket.write(b"status\n")

    def _on_disconnected(self):
        log("info", "socket disconnected")
        self._buffer = b""
        self._set_connected(False)

    def _on_socket_error(self, error):
        log("error", f"socket error: {self._socket.errorString()} ({error})")
        self.connectionError.emit(self._socket.errorString())

    def _on_ready_read(self):
        self._buffer += bytes(self._socket.readAll())
        while b"\n" in self._buffer:
            line, self._buffer = self._buffer.split(b"\n", 1)
            if line.strip():
                self._parse(line)

    def _parse(self, raw: bytes):
        obj = parse_json(raw)
        if obj is None or "error" in obj:
            if obj:
                log("error", f"daemon: {obj['error']}")
            return

        self._profile = obj.get("profile", "")
        self._cpu = obj.get("cpu", {})
        self._gpu = obj.get("gpu", [])
        self._fans = obj.get("fans", [])
        self._ram = obj.get("ram", {})
        self.statusChanged.emit()


def main():
    app = QApplication(sys.argv)
    socket = Socket()

    engine = QQmlApplicationEngine()
    engine.rootContext().setContextProperty("socket", socket)
    engine.load(QUrl.fromLocalFile(str(Path(__file__).with_name("main.qml"))))

    if not engine.rootObjects():
        sys.exit(-1)
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
