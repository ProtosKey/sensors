# Sensors

Небольшой минималистичный виджет мониторинга железа для Linux. Сделан для ноутбуков ASUS, но работает на большинстве оборудования на Linux. Показывает CPU, GPU, FAN и RAM в компактном окне поверх остальных.

Пока это **только монитор**: активный профиль питания показывается, но не переключается.

## Возможности

- **CPU:** модель, температура, частота, мощность пакета (**пока только для AMD**)
- **GPU:** все видеокарты в системе, включая дискретные NVIDIA; название, температура, мощность, частота, VRAM или GTT
- **FAN:** обороты с анимацией
- **RAM:** занято и сколько доступно
- **Профиль питания** (из `/sys/firmware/acpi/platform_profile`)
- Root не нужен: всё читается из `/sys` и `/proc`

## Как это устроено

Демон слушает `$XDG_RUNTIME_DIR/sensors.sock` (если переменной нет, то `/tmp/sensors.sock`; доступ только владельцу). 

Соединение остаётся открытым: отправляете `status\n`, получаете одну строку JSON.

```json
{
  "profile": "Performance",
  "cpu":  { "model": "AMD Ryzen AI 9 HX 370 w/ Radeon 890M", "temp": 54.2, "freq": 2650, "power": 17.9 },
  "gpu":  [ { "name": "GeForce RTX 5070 Ti Mobile", "driver": "nvidia", "active": true,
              "temp": 45, "power": 4.4, "freq": 180, "vram": { "used": 45, "total": 12227 } },
            { "name": "Radeon 890M", "driver": "amdgpu", "temp": 46, "freq": 1343,
              "gtt": { "used": 1641, "total": 15594 } } ],
  "fans": [ { "label": "cpu_fan", "rpm": 2500 }, { "label": "gpu_fan", "rpm": 2700 } ],
  "ram":  { "used": 8559, "total": 31187 }
}
```

Отсутствующие значения просто не передаются. Память в МиБ, частоты в МГц.

## Требования

**Демон (для сборки):** компилятор C, CMake ≥ 3.16, `pkg-config`, заголовки
`libcjson` и `libudev`.

| Дистрибутив | Пакеты |
|---|---|
| Arch | `base-devel cmake cjson libudev` (`libudev` входит в `systemd-libs`) |
| Debian / Ubuntu | `build-essential cmake pkg-config libcjson-dev libudev-dev` |
| Fedora | `gcc cmake pkgconf-pkg-config cjson-devel systemd-devel` |

**Интерфейс (для сборки):** компилятор C++17, CMake ≥ 3.16, Qt 6.6 или новее
(`Core`, `Gui`, `Network`, `Qml`, `Quick`; разрабатывалось на 6.12). При работе
нужен только системный Qt 6 (на KDE Plasma 6 он уже установлен).

| Дистрибутив | Пакеты |
|---|---|
| Arch | `qt6-base qt6-declarative` |
| Debian / Ubuntu | `qt6-base-dev qt6-declarative-dev` |
| Fedora | `qt6-qtbase-devel qt6-qtdeclarative-devel` |

**Необязательно:** `nvidia-smi`.

## Сборка и запуск

```sh
cmake -S daemon -B daemon/build
cmake --build daemon/build
./daemon/build/sensors-daemon         

cmake -S app -B app/build
cmake --build app/build
./app/build/sensors-widget       
```

Интерфейс и демон должны быть одной версии: у протокола пока нет версионирования.

### Запуск демона вместе с сессией 

Необходимо создать службу в `~/.config/systemd/user/sensors.service`.

```ini
[Unit]
Description=Sensors Daemon

[Service]
Type=simple
ExecStart=%h/.local/bin/sensors-daemon
Restart=on-failure
RestartSec=3

[Install]
WantedBy=default.target
```

```sh
install -Dm755 daemon/build/sensors-daemon ~/.local/bin/sensors-daemon
systemctl --user daemon-reload
systemctl --user enable --now sensors.service
```

`ExecStart` должен быть абсолютным путём: systemd не раскрывает `~` и `$HOME`
(используйте `%h`), а пути с пробелами нужно брать в кавычки. После каждой
пересборки копируйте бинарник заново и выполняйте
`systemctl --user restart sensors.service`.

## Откуда берутся данные

| Данные | Источник | Примечания |
|---|---|---|
| Температура CPU | hwmon (`k10temp`, `coretemp`) | |
| Частота CPU | `cpufreq` в sysfs, запасной вариант `/proc/cpuinfo` | среднее по всем потокам, мгновенное значение |
| Мощность CPU | `PPT` у `amdgpu` в hwmon | **только AMD APU**: это мощность всего чипа. Intel RAPL требует root |
| Список GPU и названия | `/sys/class/drm` + база udev | работает и для видеокарт без hwmon |
| Датчики AMD GPU | `amdgpu` в hwmon, `mem_info_*` | у встроенных карт вместо маленького VRAM из BIOS показывается GTT |
| NVIDIA GPU | `nvidia-smi` | спящая карта **не опрашивается**, чтобы её не будить |
| Intel GPU | частота из `gt_act_freq_mhz` | только название и частота; температура, мощность и память без root недоступны |
| Вентиляторы | hwmon `fan*_input` | безымянные (`fan1`) скрываются, если есть именованные |
| RAM | `/proc/meminfo` | занято = всего - доступно |
| Профиль питания | `/sys/firmware/acpi/platform_profile` | первая буква заглавная (`quiet` → `Quiet`); если файла нет, показывается `unknown` |

## Если что-то не работает

- **«Daemon not connected»:** демон не запущен либо стороны используют разные
  пути к сокету. Проверьте `systemctl --user status sensors.service` и
  `ls $XDG_RUNTIME_DIR/sensors.sock`.
- **`status=203/EXEC`:** systemd не может запустить файл из `ExecStart`:
  неверный или относительный путь, нет `chmod +x`, либо бинарник перенесли или
  удалили. Смотрите `journalctl --user -u sensors.service -n 10`.
- **`Neither a valid executable name nor an absolute path`:** в `ExecStart`
  указан `~`, `$HOME` или относительный путь.
- **Нет видеокарты:** пришлите вывод `ls /sys/class/drm` и
  `lspci | grep -iE "vga|3d|display"`.
- **Профиль `unknown`:** прошивка не поддерживает ACPI platform profile
  (нет файла `/sys/firmware/acpi/platform_profile`).
- **Положение окна или «поверх всех окон» игнорируется:** на Wayland это решает
  композитор. В KDE используйте правило окна (Alt+F3, «Дополнительные действия»,
  «Особые параметры окна»).

## Структура проекта

```
daemon/                      демон на C (CMake)
  src/
    main.c                   точка входа, путь к сокету
    core/                    вспомогательные функции для файлов и процессов
    hwmon/                   универсальный сканер hwmon (датчики, категории)
    info/                    CPU, GPU (DRM/udev), NVIDIA, RAM, профиль питания
    stats/                   собирает JSON для cpu / gpu / fans / ram
    server/                  сервер на unix-сокете, команды
app/                         интерфейс (C++/Qt + QML, CMake)
  src/
    main.cpp                 окно и движок QML
    SensorsClient.*          клиент сокета, передаёт данные в QML
  main.qml                   окно и раскладка (QML встраивается в бинарник)
  components/                Theme, Card, Line, Icon, FanIcon, ...
  cards/                     по файлу на каждую карточку
```

## Состояние и планы

Пока не сделано: загрузка CPU и GPU в процентах, карточка батареи, переключение профиля питания из интерфейса, мощность CPU на Intel, упаковка для дистрибутивов.

## Лицензия

[MIT](LICENSE)
