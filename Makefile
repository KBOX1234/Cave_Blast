CC = g++
CFLAGS = -lraylib -lm -ldl -lpthread -lGL -lX11 -lrlImGui -lenet
INCLUDE = -I./include -I./external/rl_imgui/ -I./external/raylib/src/ -I./external/imgui/ -I./external/enet/include/
LINKS = -L./lib

SRC_DIR = src
OBJ_DIR = obj
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(INCLUDE) $(LINKS) $(CFLAGS)

# Create obj/ if it doesn't exist and compile each .cpp to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) -c $< -o $@ $(INCLUDE)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
