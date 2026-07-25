NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread

SRC = main.c \
		parser.c \
		threads.c \
		monitor.c \
		scheduler.c \
		logger.c \
		utils.c \
		cleanup.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re