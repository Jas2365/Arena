cxx = gcc
cflags =
ldflags = -lbcrypt

src_dir = src
out_dir = out

inc = -Iinc -IArena -IString
srcs = $(wildcard $(src_dir)/*.c) $(wildcard $(src_dir)/**/*.c)

objs = $(patsubst $(src_dir)/%.c, $(out_dir)/%.o, $(srcs))

# Detect OS
ifeq ($(OS),Windows_NT)
	MKDIR = if not exist "$(subst /,\,$@)" mkdir $(subst /,\,$@)"
	RM    = if exist "$(out_dir)" rd /s /q $(out_dir)
else
	MKDIR = mkdir -p $@
	RM    = rm -rf $(out_dir)
endif

dirs := $(sort $(dir $(objs)))

target = out/Arena.exe

all: clean build run

build: $(objs) $(target)

#dirs
$(dirs):
	@$(MKDIR)

#  linking 
$(target): $(objs)
	@echo Linking $@ ...
	@$(cxx) $(objs) -o $(target) $(ldflags)

# compilation
$(out_dir)/%.o : $(src_dir)/%.c | $(dirs)
	@echo Compiling $< ...
	@$(cxx) $(cflags) $(inc) -c $< -o $@

run:
	@echo  ---- Running Project ---- 
	@./$(target)
	@echo  ---- Ending  Project ---- 

clean:
	@echo ---- Cleaning up ----
	@$(RM)