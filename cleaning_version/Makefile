# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jobenass <jobenass@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/06/10 16:17:37 by sad-aude          #+#    #+#              #
#    Updated: 2021/06/23 14:45:38 by jobenass         ###   ########lyon.fr    #
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
VPATH		=	./config \
				./
HEADERS		=	Socket.hpp \
				Webserv.hpp \
				Request.hpp \
				Config.hpp \
				Parser.hpp
FILES		= Socket.cpp \
			  PageCreation.cpp \
			  CloseError.cpp \
			  InitEnvp.cpp \
			  CheckingHeader.cpp \
			  ParsingRequest.cpp \
			  Config.cpp \
			  Parser.cpp \
			  main.cpp

OPATH		= objs/

OBJS		= $(addprefix $(OPATH), ${FILES:.cpp=.o})

# Compiling
all:	draw $(NAME)

$(OPATH)%.o	:	%.cpp $(HEADERS)
	$(shell mkdir -p $(OPATH))
	@echo "$(BLUE)$(HIDDEN)COMPILATION $(NORMAL)$(GREY)\t"$<
	$(CC) $(CPPFLAGS) -c $< -o $@

$(NAME):	$(OBJS)
	@echo "$(CYAN)\nWEBSERV $(GREEN)\tREADY FOR EXEC$(NORMAL)"
	@$(CC) $(CPPFLAGS) $(OBJS) -o $(NAME)

# Cleaning
clean:
	@echo "$(BLUE)WEBSERV $(GREY)\t.O DELETION $(NORMAL)"
	@rm -rf $(OPATH)

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
	./webserv ./config/nginx.conf