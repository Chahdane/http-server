sources = main.cpp Parsing/ConfigFile.cpp Parsing/Location.cpp Parsing/Server.cpp Parsing/utils.cpp\
		WebServer/WebServer.cpp Socket/Socket.cpp

objects = $(sources:.cpp=.o)

CC = c++
CFLAGS = -Wall -Wextra -Werror -g -O3 -I./headers -std=c++98

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

all: WebServ

WebServ: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

clean:
	rm -f $(objects) tmp/tmp*

fclean: clean
	rm -f WebServ

re: fclean all

.PHONY: all clean fclean re