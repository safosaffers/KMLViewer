# файл .pro
PRO_FILE = ../KMLViewer.pro
# исполняемый файл
TARGET = kmlviewer.out
# имя для архива
DIST_NAME = KMLViewer_dist.tar.gz
###############################################################

.PHONY: all clean style_check style_fix cppcheck_check docker install uninstall

# Цель по умолчанию - создание исполняемого файла
all: $(TARGET)

### Создание исполняемого файла
$(TARGET):
	cd View \
	&& mkdir -p build \
	&& cd build \
	&& qmake $(PRO_FILE) -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug \
	&& make TARGET=../../$(TARGET) \
	&& $(MAKE) -C ../../ style_fix

# Правило для установки
install: $(TARGET)

uninstall:
	rm -f /usr/local/bin/$(TARGET)

### создания дистрибутива
dist: clean
	mkdir -p dist
	cp -r Model/ Controller/ View/ main.cpp Makefile doc/ KMLs/ .clang-format .gitignore dist/
	tar -czf $(DIST_NAME) dist/
	rm -rf dist

### Проверка стиля кода на соответсвие Google Style
style_check:
	find . \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.cc" -o -name "*.hh" \) -exec clang-format -n {} +

### Исправление стиля кода на соответствие Google Style
style_fix:
	find . \( -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.cc" -o -name "*.hh" \) -exec clang-format -i {} +

### Запуск cppcheck
cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem $(MODEL_SRC) $(CONTROLLER_SRC) $(VIEW_SRC)

rebuild: clean all

clean:
	rm -rf *.a *.out dist/ $(DIST_NAME)
	find . -name "*.o" -exec rm {} +
	rm -rf View/build View/KMLViewer.pro.user libs/ $(TARGET)
