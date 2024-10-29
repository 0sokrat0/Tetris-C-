# Компилятор и флаги
CXX = g++
CXXFLAGS = -Wall -std=c++14

# Путь к исходным файлам и заголовкам
SRC_DIR = src
INC_DIR = include

# Библиотеки
LIBS = -lraylib -lGL -lm -ldl -lpthread -lrt -lX11

# Поиск всех файлов .cpp в SRC_DIR
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Соответствующие объектные файлы в той же папке
OBJS = $(SRCS:.cpp=.o)

# Цель по умолчанию
all: game

# Правило сборки
game: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LIBS)

# Правило компиляции объектных файлов
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJS) game
