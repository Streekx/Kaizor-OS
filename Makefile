CXX = g++
CXXFLAGS = -Wall -std=c++17 $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
LDFLAGS = $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf)

TARGET = kaizor

SRC = main.cpp \
      graphics/display_server.cpp \
      graphics/renderer.cpp \
      desktop/desktop_shell.cpp \
      desktop/wallpaper_engine.cpp \
      desktop/wallpaper.cpp \
      desktop/desktop_icons.cpp \
      window_manager/window.cpp \
      window_manager/window_manager.cpp \
      gui/taskbar.cpp \
      gui/dock.cpp \
      gui/font_manager.cpp \
      gui/text_renderer.cpp \
      gui/ui_theme.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)
