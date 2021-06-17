# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sad-aude <sad-aude@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/06/10 16:17:37 by sad-aude          #+#    #+#              #
#    Updated: 2021/06/17 15:45:42 by sad-aude         ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

# Colors
NORMAL= \033[0m
GREY = \033[90m
GREEN = \033[32m
YELLOW = \033[33m
BLUE = \033[34m
CYAN = \033[36m
PURPLE = \033[35m
RED = \033[31m
HIDDEN = \033[2m
ITALIC = \033[3m

# Pre-compiling
NAME		= webserv
CC		 	= clang++
CPPFLAGS 	= -g -Wall -Wextra -Werror
HEADERS		=	Socket.hpp \
				Webserv.hpp
FILES		= Socket.cpp \
			  PageCreation.cpp \
			  CloseError.cpp \
			  InitEnvp.cpp \
			  CheckingHeader.cpp \
			  ParsingRequest.cpp \
			  main.cpp

OBJS		= $(FILES:.cpp=.o)

# Compiling
all:	draw $(NAME)

$(NAME):	$(OBJS)
	@echo "$(CYAN)\nWEBSERV $(GREEN)\tREADY FOR EXEC$(NORMAL)"
	@$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp $(HEADERS)
	@echo "$(BLUE)$(HIDDEN)COMPILATION $(NORMAL)$(GREY)\t"$<
	@$(CC) $(CPPFLAGS) -c $< -o $@

# Cleaning
clean:
	@echo "$(BLUE)WEBSERV $(GREY)\t.O DELETION $(NORMAL)"
	@rm -f $(OBJS)

fclean:		clean
	@echo "$(BLUE)WEBSERV $(GREY)\tEXEC DELETION $(NORMAL)"
	@rm -f $(NAME)
	@echo "$(CYAN)\nWEBSERV $(GREEN)\tALL HAS BEEN CLEANED $(NORMAL)"

re: fclean all

# Designing
draw:
	@printf "\n"
	@printf "$(BLUE)[..        [..          [..                                         \n"
	@printf "$(BLUE)[..        [..          [..                                         \n"
	@printf "$(BLUE)[..   [.   [..   [..    [..       [....    [..    [. [...[..     [..\n"
	@printf "$(BLUE)[..  [..   [.. [.   [.. [.. [..  [..     [.   [..  [..    [..   [.. \n"
	@printf "$(BLUE)[.. [. [.. [..[..... [..[..   [..  [... [..... [.. [..     [.. [..  \n"
	@printf "$(BLUE)[. [.    [....[.        [..   [..    [..[.         [..      [.[..   \n"
	@printf "$(BLUE)[..        [..  [....   [.. [..  [.. [..  [....   [...       [..    \n"
	@printf "$(ITALIC)\t\t\t\t\t\t\tWebserv\n"
	@printf "$(HIDDEN)$(ITALIC)\t\t\t\t\t\t\t La team\n"
	@printf "$(NORMAL)																\n"

exec: all
	./webserv