NAME			=	ircserv

CFLAGS			=	-Wall -Wextra -Werror -I ${INC_PATH} -g3 -std=c++98

CC				=	c++

GREEN			=	\033[0;32m

PURPLE			=	\033[10;95m

YELLOW			=	\033[0;33m

END				=	\033[0m

# **************************************************************************** #
#                                                                              #
#                                  SOURCES                                     #
#                                                                              #
# **************************************************************************** #

SOURCES_PATH	=	./srcs/

SOURCES_MAIN	= 	main.cpp \

SOURCES_SERVER	=	Server.cpp \
					Client.cpp \
					Utils.cpp \
					numerics.cpp \

SOURCES_CHANNEL	=	Channel.cpp \
	
# **************************************************************************** #
#                                                                              #
#                                  OBJECTS                                     #
#                                                                              #
# **************************************************************************** #

OBJECTS_PATH	=	objs/

OBJECTS			=	$(addprefix ${OBJECTS_PATH}, ${SOURCES_MAIN:.cpp=.o}) \
					$(addprefix ${OBJECTS_PATH}, ${SOURCES_SERVER:.cpp=.o}) \
					$(addprefix ${OBJECTS_PATH}, ${SOURCES_CHANNEL:.cpp=.o}) \

# **************************************************************************** #
#                                                                              #
#                                  INCLUDES                                    #
#                                                                              #
# **************************************************************************** #

INC_PATH		=	./includes/

INCLUDES		=	${INC_PATH}Server.hpp \
					${INC_PATH}Client.hpp \
					${INC_PATH}Utils.hpp \
					${INC_PATH}numerics.hpp \
					${INC_PATH}Channel.hpp \

# **************************************************************************** #
#                                                                              #
#                                  RULES                                       #
#                                                                              #
# **************************************************************************** #

all:  ${NAME}

${NAME}: ${OBJECTS} ${INCLUDES}
	@${CC} ${CFLAGS} ${OBJECTS} -o ${NAME}
	@echo "${GREEN}\33[2K\nircserv compiled with : \n${END}"
	@echo "	${CC} ${CFLAGS}\n"
	@printf "$(PURPLE)"
	@echo " ____  ____   ___"
	@echo "(_  _)(  _ \ / __)"
	@echo " _)(_  )   /( (__ "
	@echo "(____)(_)\_) \___)"
	@echo "\n"
	@echo "© 2025 Ircserv, Inc. aciezadl - jaung - ade-fran"
	@echo "\n"
	@printf "${COLOUR_END}"

${OBJECTS_PATH}%.o:	${SOURCES_PATH}%.cpp
	@mkdir -p ${dir $@}
	@${CC} ${CFLAGS} -c $< -o $@ && printf "\33[2K\r${YELLOW}Compiling ircserv :${END} $@"

clean:
	@rm -rf ${OBJECTS_PATH}

fclean:
	@rm -rf ${OBJECTS_PATH}
	@rm -f ${NAME}
	@echo "${GREEN}\nircserv cleaned\n${END}"

re: fclean all

.PHONY: fclean clean all re