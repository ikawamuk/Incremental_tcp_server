NAME	=	reactor
CXX		=	cc
CXXFLAG	=	-Wall -Wextra -Werror -I$(INCDIR) -g -fsanitize=address,undefined

SRCS	=	$(wildcard $(SRCDIR)/*.c)
OBJS	=	$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

SRCDIR	=	src
OBJDIR	=	obj
INCDIR	=	include

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAG) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAG) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
