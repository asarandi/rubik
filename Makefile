NAME    = rubik
CFLAGS  += -Wall -Werror -Wextra -I include
LDFLAGS += $(shell sdl2-config --libs) -lm

SOLVER_SRC = $(addprefix src/, hashtable.c solver_only.c queue.c solver.c stack.c utils.c)
SOLVER_OBJ = $(SOLVER_SRC:.c=.o)

SRC = $(addprefix src/, hashtable.c main.c queue.c solver.c stack.c utils.c)
OBJ = $(SRC:.c=.o)

src/main.o src/solver_only.o : CFLAGS += $(shell sdl2-config --cflags)

UNAME	:= $(shell uname -s)

ifeq ($(UNAME),Darwin)
    src/main.o : CFLAGS += -Wno-deprecated-declarations
    LDFLAGS += -framework OpenGL
endif

ifeq ($(UNAME),Linux)
    LDFLAGS += -lGL -lGLU
endif

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

solver: $(SOLVER_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean $(NAME)
