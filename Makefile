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

CFLAGS=-O3 -g -std=c++11 -Wall -pedantic
LFLAGS=-llog4cpp -L/usr/local/opt/boost/lib/ -lboost_program_options-mt

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECT_FILES) $(MAIN.O)
	g++ $(OBJECT_FILES) $(MAIN.O) $(LFLAGS) -o $(EXECUTABLE) 

$(B)/%.o : $(S)/%.cpp
	@mkdir -p $(@D)
	g++ $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(B)

.PHONY : clean 
