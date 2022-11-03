# Target library
lib := libuthread.a

# Default rule
all: $(lib)


# Current directory
CUR_PWD := $(shell pwd)


# Don't print the commands unless explicitly requested with `make V=1`
ifneq ($(V),1)
Q = @
V = 0
endif


# Define compilation toolchain
CC	= gcc
# static library compile
AR = ar
RCS = rcs

# General gcc options
CFLAGS	:= -Wall -Wextra -Werror
CFLAGS	+= -pipe
## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -g
endif
## Dependency generation
CFLAGS	+= -MMD

# Object files to compile
objs := queue.o uthread.o preempt.o context.o sem.o  # other files to be added later

# Include dependencies
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

# Build libuthread.a
$(lib): $(objs)
	@echo "CC	$@"
	$(Q)$(AR) $(RCS) $@ $^

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# Cleaning rule
clean: 
	@echo "CLEAN	$(CUR_PWD)"
	$(Q)rm -rf $(lib) $(objs) $(deps)
