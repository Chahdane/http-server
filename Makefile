sources = main.cpp Parsing/ConfigFile.cpp Parsing/Location.cpp Parsing/Server.cpp Parsing/utils.cpp\
		CGI/cgi.cpp WebServer/WebServer.cpp Socket/Socket.cpp

objects = $(sources:.cpp=.o)

CC = c++
CFLAGS = -Wall -Wextra -Werror -g -O3 -I./headers

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

all: WebServ

WebServ: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

clean:
	rm -f $(objects)

fclean: clean
	rm -f WebServ

re: fclean all

.PHONY: all clean fclean re