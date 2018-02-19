

GCCPARAMS = -W -Wall -Wextra -Werror -lm -O3
OBJECTS = comon.o filesys.o main.o netbuild.o netsys.o trsys.o
LDPARAMS = -lm -O3
EXEC = bnn.ex



all: $(EXEC)

%.o: %.c
	gcc $(GCCPARAMS) -o $@ -c $^


$(EXEC): $(OBJECTS)
	gcc -o $@ $^ $(GCCPARAMS)


run: $(EXEC)
	./$<
	
clean:
	rm -rf $(OBJECTS)

remove:
	rm -rf $(OBJECTS) $(EXEC)




