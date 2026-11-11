NAME	=	reactor
CXX		=	c++
CXXFLAG	=	-Wall -Wextra -Werror -I$(INCDIR) -std=c++20 -g -fsanitize=address,undefined

SRCS	=	$(wildcard $(SRCDIR)/*.cpp)
OBJS	=	$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

SRCDIR	=	src
OBJDIR	=	obj
INCDIR	=	include

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAG) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAG) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean
	$(MAKE) all

.PHONY: all clean fclean re
