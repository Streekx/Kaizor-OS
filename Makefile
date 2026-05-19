CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 \
           $(shell pkg-config --cflags sdl2 SDL2_image SDL2_ttf)
LDFLAGS  = $(shell pkg-config --libs sdl2 SDL2_image SDL2_ttf) -lm

TARGET = kaizor

SRC = main.cpp \
      graphics/display_server.cpp \
      graphics/renderer.cpp \
      desktop/desktop_shell.cpp \
      desktop/wallpaper_engine.cpp \
      desktop/wallpaper.cpp \
      desktop/desktop_icons.cpp \
      desktop/launcher.cpp \
      desktop/notifications.cpp \
      window_manager/window_manager.cpp \
      window_manager/window.cpp \
      gui/taskbar.cpp \
      gui/dock.cpp \
      gui/font_manager.cpp \
      gui/text_renderer.cpp \
      gui/ui_theme.cpp \
      apps/terminal_app.cpp \
      apps/browser_app.cpp \
      apps/file_manager_app.cpp \
      apps/settings_app.cpp

# Filter to only existing files
EXISTING_SRC = $(wildcard $(SRC))

all:
	$(CXX) $(CXXFLAGS) $(EXISTING_SRC) -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET) *.o
