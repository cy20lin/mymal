CC = g++

CFLAGS = -std=c++17 -g -Wall

LIBS = #-ledit -lgc
FFI_LIBS = #-ldl -lffi

SRC = reader.cpp printer.cpp types.cpp env.cpp core.cpp
HEADERS = reader.hpp printer.hpp types.hpp env.hpp core.hpp

S0_SRC = step0_repl.cpp
S1_SRC = step1_read_print.cpp reader.cpp types.cpp printer.cpp
S2_SRC = step2_eval.cpp reader.cpp types.cpp printer.cpp
S3_SRC = step3_env.cpp reader.cpp types.cpp printer.cpp env.cpp
S4_SRC = step4_if_fn_do.cpp $(SRC)
S5_SRC = step5_tco.cpp $(SRC)
S6_SRC = step6_file.cpp $(SRC)
S7_SRC = step7_quote.cpp $(SRC)
S8_SRC = step8_macros.cpp $(SRC)
S9_SRC = step9_try.cpp $(SRC)
SA_SRC = stepA_mal.cpp $(SRC)

S0_HEADERS =
S1_HEADERS = reader.hpp types.hpp printer.hpp
S2_HEADERS = reader.hpp types.hpp printer.hpp 
S3_HEADERS = reader.hpp types.hpp printer.hpp env.hpp
S4_HEADERS = $(HEADERS)
S5_HEADERS = $(HEADERS)
S6_HEADERS = $(HEADERS)
S7_HEADERS = $(HEADERS)
S8_HEADERS = $(HEADERS)
S9_HEADERS = $(HEADERS)
SA_HEADERS = $(HEADERS)

S0 = step0_repl
S1 = step1_read_print
S2 = step2_eval
S3 = step3_env
S4 = step4_if_fn_do
S5 = step5_tco
S6 = step6_file
S7 = step7_quote
S8 = step8_macros
S9 = step9_try
SA = stepA_mal

all: $(S0) $(S1) $(S2) $(S3) $(S4)#$(S5) $(S6) $(S7) $(S8) $(S9) $(SA)

$(S0): $(S0_SRC) $(S0_HEADERS)
	$(CC) $(CFLAGS) $(S0_SRC) $(LIBS) -o $(S0)

$(S1): $(S1_SRC) $(S1_HEADERS)
	$(CC) $(CFLAGS) $(S1_SRC) $(LIBS) -o $(S1)

$(S2): $(S2_SRC) $(S2_HEADERS)
	$(CC) $(CFLAGS) $(S2_SRC) $(LIBS) -o $(S2)

$(S3): $(S3_SRC) $(S3_HEADERS)
	$(CC) $(CFLAGS) $(S3_SRC) $(LIBS) -o $(S3)

$(S4): $(S4_SRC) $(S4_HEADERS)
	$(CC) $(CFLAGS) $(S4_SRC) $(LIBS) -o $(S4)

$(S5): $(S5_SRC) $(S5_HEADERS)
	$(CC) $(CFLAGS) $(S5_SRC) $(LIBS) -o $(S5)

$(S6): $(S6_SRC) $(S6_HEADERS)
	$(CC) $(CFLAGS) $(S6_SRC) $(LIBS) -o $(S6)

$(S7): $(S7_SRC) $(S7_HEADERS)
	$(CC) $(CFLAGS) $(S7_SRC) $(LIBS) -o $(S7)

$(S8): $(S8_SRC) $(S8_HEADERS)
	$(CC) $(CFLAGS) $(S8_SRC) $(LIBS) -o $(S8)

$(S9): $(S9_SRC) $(S9_HEADERS)
	$(CC) $(CFLAGS) $(S9_SRC) $(LIBS) -o $(S9)

$(SA): $(SA_SRC) $(SA_HEADERS)
	$(CC) $(CFLAGS) $(SA_SRC) $(LIBS) $(FFI_LIBS) -DWITH_FFI -o $(SA)

.PHONY clean:
	rm -f $(S0) $(S1) $(S2) $(S3) $(S4) $(S5) $(S6) $(S7) $(S8) $(S9) $(SA)
