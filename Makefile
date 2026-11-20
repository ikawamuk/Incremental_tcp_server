NAME		=	reactor
CXX			=	c++
CXXFLAG		=	-Wall -Wextra -Werror -I$(INCDIR) -I$(LIBINCDIR) -g -std=c++20 -fsanitize=address,undefined
LDLIBS		= -Llib -lrosetta_json

SRCS		=	$(wildcard $(SRCDIR)/*.cpp)
OBJS		=	$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

SRCDIR		=	src
OBJDIR		=	obj
INCDIR		=	include
LIBINCDIR	=	lib/include
LIB			=	lib/build/librosetta_json.a

all: $(NAME)

$(NAME): $(OBJS) $(LIB)
	$(CXX) $(CXXFLAG) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAG) -c $< -o $@

$(LIB):
	$(MAKE) -C lib

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re
