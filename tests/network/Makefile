.PHONY: all clean fclean re

NAME = network_tests

ROOT_DIR = ../..
TESTS_DIR = $(ROOT_DIR)/tests
INCLUDE_DIR = $(ROOT_DIR)/include
PROJECT_SRC_DIR = $(ROOT_DIR)/src/network

OBJ_DIR = obj
BIN_DIR = bin

UNIT_DIR = unit
INTEGRATION_DIR = integration
STRESS_DIR = stress
FRAMEWORK_DIR = $(TESTS_DIR)/TestFramework

SRC_RUNNER = $(FRAMEWORK_DIR)/TestRunner.cc
SRC_MAIN = run_all_tests.cc
SRC_UNIT = $(wildcard $(UNIT_DIR)/*.cc)
SRC_INTEGRATION = $(wildcard $(INTEGRATION_DIR)/*.cc)
SRC_STRESS = $(wildcard $(STRESS_DIR)/*.cc)
SRC_NETWORK = $(wildcard $(PROJECT_SRC_DIR)/*.cc)

FRAMEWORK_OBJ = $(OBJ_DIR)/framework/TestRunner.o
MAIN_OBJ = $(OBJ_DIR)/run_all_tests.o
UNIT_OBJS = $(patsubst $(UNIT_DIR)/%.cc,$(OBJ_DIR)/unit/%.o,$(SRC_UNIT))
INTEGRATION_OBJS = $(patsubst $(INTEGRATION_DIR)/%.cc,$(OBJ_DIR)/integration/%.o,$(SRC_INTEGRATION))
STRESS_OBJS = $(patsubst $(STRESS_DIR)/%.cc,$(OBJ_DIR)/stress/%.o,$(SRC_STRESS))
NETWORK_OBJS = $(patsubst $(PROJECT_SRC_DIR)/%.cc,$(OBJ_DIR)/network/%.o,$(SRC_NETWORK))

OBJS = $(FRAMEWORK_OBJ) $(MAIN_OBJ) $(UNIT_OBJS) $(INTEGRATION_OBJS) $(STRESS_OBJS) $(NETWORK_OBJS)

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

$(OBJ_DIR)/integration/%.o: $(INTEGRATION_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/stress/%.o: $(STRESS_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/network/%.o: $(PROJECT_SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(BIN_DIR)

re: fclean all
