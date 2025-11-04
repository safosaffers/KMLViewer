# QML + C++ Stack

- QML для интерфейса
- C++ для логики упрощения, потоков и обработки KML
- Современный подход, более гибкий для UI

## Подробнее:

### 1. Qt модули:
- Qt Core (включено в Qt6.9.3)
- Qt Gui (включено в Qt6.9.3)
- Qt Widgets (включено в Qt6.9.3)
- Qt Concurrent (для потоков)
- QtXml (для работы с KML)
- Qt OpenGL (для тайловых карт)

### 2. Дополнительные библиотеки:
- GEOS (Geometry Engine - Open Source) для операций с геометрией
- Proj или QGIS для проекций

### 3. Документация:
- [https://doc.qt.io/qt-6/](https://doc.qt.io/qt-6/) - официальная документация Qt6
- Особое внимание: Qt Concurrent, Qt OpenGL, Qt XML, Qt Widgets
- Алгоритм Дугласа-Пекера: [https://en.wikipedia.org/wiki/Ramer-Douglas-Peucker_algorithm](https://en.wikipedia.org/wiki/Ramer-Douglas-Peucker_algorithm)
- Расстояние от точки до прямой https://ru.wikipedia.org/wiki/Расстояние_от_точки_до_прямой_на_плоскости.

### 4. Инструменты:
- Qt Creator (для удобной разработки)
- CMake или qmake (в зависимости от предпочтений)

Необходимо ознакомиться с документацией по:
- Qt Concurrent (для потоков)
- QDomDocument (для работы с XML/KML)
- QOpenGLWidget (для отображения областей и тайловых карт)