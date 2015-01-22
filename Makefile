################################################################
########	SGP	MAkefile								########	
## Author: Ashkan Rezaei <e0926809@student.tuwien.ac.at>	####
##
##


B=build
S=src
EXECUTABLE=$(B)/sgp
MAIN.O=$(B)/main.o
SGP.O=$(B)/sgp.o
TABU.O=$(B)/tabu.o

OBJECT_FILES=$(SGP.O) $(TABU.O)

CC=g++
CFLAGS=-O3 -g -std=c++11 -Wall -pedantic
LFLAGS=-llog4cpp -lboost_program_options-mt

#/usr/local/opt/boost/lib/libboost_program_options-mt.a

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES) $(MAIN.O)
	$(CC) $(LFLAGS) $(OBJECT_FILES) $(MAIN.O) -o $(EXECUTABLE) 

$(B)/%.o : $(S)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(B)

.PHONY : clean 
