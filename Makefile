.PHONY: all clean

APP=lab4adtN3150
CFLAGS=-Wall -Wextra -Werror -g

all: $(APP)

$(APP):
	gcc -o $(APP) $(CFLAGS) $(APP).c -lm

clean:
	rm $(APP)
