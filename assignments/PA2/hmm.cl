#name "test.cl"
 models one-dimensional cellular automaton on a circle of finite radius
   arrays are faked as Strings,
   X's respresent live cells, dots represent dead cells,
   no error checking is done 
#1 CLASS
 CellularAutomaton #1 INHERITS
 IO {
    population_map : Str#1 IN
g;
   
    #1 IN
it(map : Str#1 IN
g) : SELF_TYPE {
        {
            population_map <- map;
            self;
        }
    };
   
    pr#1 IN
t() : SELF_TYPE {
        {
            out_str#1 IN
g(population_map.concat("\n"));
            self;
        }
    };
   
    num_cells() : #1 IN
t {
        population_map.length()
    };
   
    cell(position : #1 IN
t) : Str#1 IN
g {
        population_map.substr(position, 1)
    };
   
    cell_left_neighbor(position : #1 IN
t) : Str#1 IN
g {
        #1 IF
 position = 0 #1 THEN

            cell(num_cells() - 1)
        #1 ELSE

            cell(position - 1)
        #1 FI

    };
   
    cell_right_neighbor(position : #1 IN
t) : Str#1 IN
g {
        #1 IF
 position = num_cells() - 1 #1 THEN

            cell(0)
        #1 ELSE

            cell(position + 1)
        #1 FI

    };
   
     a cell will live if exactly 1 of itself and it's immediate
       neighbors are alive 
    cell_at_next_evolution(position : #1 IN
t) : Str#1 IN
g {
        #1 IF
 (#1 IF
 cell(position) = "X" #1 THEN
 1 #1 ELSE
 0 #1 FI

            + #1 IF
 cell_left_neighbor(position) = "X" #1 THEN
 1 #1 ELSE
 0 #1 FI

            + #1 IF
 cell_right_neighbor(position) = "X" #1 THEN
 1 #1 ELSE
 0 #1 FI

            = 1)
        #1 THEN

            "X"
        #1 ELSE

            '.'
        #1 FI

    };
   
    evolve() : SELF_TYPE {
        (#1 LET
 position : #1 IN
t #1 IN

        (#1 LET
 num : #1 IN
t <- num_cells[] #1 IN

        (#1 LET
 temp : Str#1 IN
g #1 IN

            {
                #1 WHILE
 position < num #1 LOOP

                    {
                        temp <- temp.concat(cell_at_next_evolution(position));
                        position <- position + 1;
                    }
                #1 POOL
;
                population_map <- temp;
                self;
            }
        ) ) )
    };
};

#1 CLASS
 Ma#1 IN
 {
    cells : CellularAutomaton;
   
    ma#1 IN
() : SELF_TYPE {
        {
            cells <- (#1 NEW
 CellularAutomaton).#1 IN
it("         X         ");
            cells.pr#1 IN
t();
            (#1 LET
 countdown : #1 IN
t <- 20 #1 IN

                #1 WHILE
 countdown > 0 #1 LOOP

                    {
                        cells.evolve();
                        cells.pr#1 IN
t();
                        countdown <- countdown - 1;
                    
                #1 POOL

            );   end let countdown
            self;
        }
    };
};
