CC = gcc
LIBS = -lm -lncursesw

OBJS = main.o shapes.o
donut : $(OBJS)
	$(CC) -o donut $(OBJS) $(LIBS)
$(OBJS) : shapes.h

.PHONNY : clean
clean :
	rm -f $(OBJS)
