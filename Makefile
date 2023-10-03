my_zsh:
	gcc -Wall -Wextra -Werror -o zsh my_zsh.c

my_zsh.o: bc-main.c
	gcc -Wall -Wextra -Werror -c my_zsh.c

clean:
	rm my_zsh

fclean: clean
	rm my_zsh

re: fclean my_zsh