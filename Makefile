NAME = webserv

CXXFLAGS =  -Wall -Wextra -Werror -fsanitize=address -g3
CXX = c++

CFILES = afadlane/SERVER.cpp \
 		 afadlane/MAIN.cpp \
 		 afadlane/GET.cpp \
 		 afadlane/CGI.cpp \
 		 afadlane/DELETE.cpp \
		 akatfi/Requeste.cpp \
		 akatfi/PostMethod.cpp \

OBJ = ${CFILES:.cpp=.o}

all: ${NAME}

${NAME} : ${OBJ}
	@${CXX} ${CXXFLAGS}  ${OBJ} -o ${NAME}

clean :
	@rm -rf ${OBJ}
	
fclean :clean
	@rm -rf ${NAME}

re : fclean all

run : all clean
	@  ./webserv
