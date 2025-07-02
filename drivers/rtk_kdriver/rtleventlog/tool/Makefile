
all: parser

clean:
	rm -rf *.exe
parser:
	gcc ../core/event_log_core.c parser_string.c -I ../ -D CONFIG_PC -o parser_string.exe
parser_binary:
	gcc parser_binary.c -I ../ -o parser_binary.exe