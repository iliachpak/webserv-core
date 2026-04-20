# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ichpakov <ichpakov@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/20 11:10:34 by ichpakov          #+#    #+#              #
#    Updated: 2025/12/22 16:18:40 by ichpakov         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.PHONY: all clean fclean re

#//////////////////////////////////////////////////////////////////////////////
#		BINARY
#//////////////////////////////////////////////////////////////////////////////

NAME = webserv

#//////////////////////////////////////////////////////////////////////////////
#		DIRECTORIES
#//////////////////////////////////////////////////////////////////////////////

SRC_DIR = src
OBJ_DIR = obj

SRC_HTTP_DIR    = $(SRC_DIR)/http
SRC_NETWORK_DIR = $(SRC_DIR)/network
SRC_SESSION_DIR = $(SRC_DIR)/session
SRC_LOGGING_DIR = $(SRC_DIR)/logging
SRC_UTILS_DIR   = $(SRC_DIR)/utils

OBJ_HTTP_DIR    = $(OBJ_DIR)/http
OBJ_NETWORK_DIR = $(OBJ_DIR)/network
OBJ_SESSION_DIR = $(OBJ_DIR)/session
OBJ_LOGGING_DIR = $(OBJ_DIR)/logging
OBJ_UTILS_DIR   = $(OBJ_DIR)/utils

FILE_DIR     = file
UPLOADS_DIR  = uploads

#//////////////////////////////////////////////////////////////////////////////
#		SOURCES
#//////////////////////////////////////////////////////////////////////////////

SRC_MAIN    = $(SRC_DIR)/main.cc
SRC_HTTP    = $(wildcard $(SRC_HTTP_DIR)/*.cc)
SRC_NETWORK = $(wildcard $(SRC_NETWORK_DIR)/*.cc)
SRC_SESSION = $(wildcard $(SRC_SESSION_DIR)/*.cc)
SRC_LOGGING = $(wildcard $(SRC_LOGGING_DIR)/*.cc)
SRC_UTILS   = $(wildcard $(SRC_UTILS_DIR)/*.cc)

SRCS = \
	$(SRC_MAIN) \
	$(SRC_HTTP) \
	$(SRC_NETWORK) \
	$(SRC_SESSION) \
	$(SRC_LOGGING) \
	$(SRC_UTILS)

#//////////////////////////////////////////////////////////////////////////////
#		OBJECTS
#//////////////////////////////////////////////////////////////////////////////

OBJS = $(SRCS:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

#//////////////////////////////////////////////////////////////////////////////
#		INCLUDES
#//////////////////////////////////////////////////////////////////////////////

INC_DIR = include
INCLUDES = -I $(INC_DIR)

#//////////////////////////////////////////////////////////////////////////////
#		COMPILATION
#//////////////////////////////////////////////////////////////////////////////

CC = c++
CXXFLAGS = -Wall -Wextra -Werror -Woverloaded-virtual -std=c++98 $(INCLUDES) -pedantic -g 
RM = rm -rf

#//////////////////////////////////////////////////////////////////////////////
#		RULES
#//////////////////////////////////////////////////////////////////////////////

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CXXFLAGS) -o $@ $^

# Create object files (with directory mirroring)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CC) $(CXXFLAGS) -c $< -o $@

#//////////////////////////////////////////////////////////////////////////////
#		CLEAN
#//////////////////////////////////////////////////////////////////////////////

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)
	$(RM) $(FILE_DIR)/*
	$(RM) $(UPLOADS_DIR)/*

re: fclean all
