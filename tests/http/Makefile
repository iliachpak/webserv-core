.PHONY: all clean fclean re

NAME = http_tests

ROOT_DIR = ../..
TESTS_DIR = $(ROOT_DIR)/tests
INCLUDE_DIR = $(ROOT_DIR)/include
HTTP_SRC_DIR = $(ROOT_DIR)/src/http
NETWORK_SRC_DIR = $(ROOT_DIR)/src/network
FRAMEWORK_DIR = $(TESTS_DIR)/TestFramework

OBJ_DIR = obj
BIN_DIR = bin
UNIT_DIR = unit

SRC_RUNNER = $(FRAMEWORK_DIR)/TestRunner.cc
SRC_MAIN = run_all_tests.cc
SRC_UNIT = $(wildcard $(UNIT_DIR)/*.cc)
SRC_HTTP = $(wildcard $(HTTP_SRC_DIR)/*.cc)
SRC_NETWORK = $(NETWORK_SRC_DIR)/Connection.cc

FRAMEWORK_OBJ = $(OBJ_DIR)/framework/TestRunner.o
MAIN_OBJ = $(OBJ_DIR)/run_all_tests.o
UNIT_OBJS = $(patsubst $(UNIT_DIR)/%.cc,$(OBJ_DIR)/unit/%.o,$(SRC_UNIT))
HTTP_OBJS = $(patsubst $(HTTP_SRC_DIR)/%.cc,$(OBJ_DIR)/http/%.o,$(SRC_HTTP))
NETWORK_OBJS = $(patsubst $(NETWORK_SRC_DIR)/%.cc,$(OBJ_DIR)/network/%.o,$(SRC_NETWORK))

OBJS = $(FRAMEWORK_OBJ) $(MAIN_OBJ) $(UNIT_OBJS) $(HTTP_OBJS) $(NETWORK_OBJS)

INCLUDES = -I $(INCLUDE_DIR) -I $(FRAMEWORK_DIR)

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 $(INCLUDES)
RM = rm -rf

all: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(FRAMEWORK_OBJ): $(SRC_RUNNER)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(MAIN_OBJ): $(SRC_MAIN)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/unit/%.o: $(UNIT_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/http/%.o: $(HTTP_SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/network/%.o: $(NETWORK_SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(BIN_DIR)

re: fclean all
