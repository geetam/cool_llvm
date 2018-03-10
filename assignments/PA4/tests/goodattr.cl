class Foo inherits Bar {
    x : Int <- a;
    y: SELF_TYPE <- b;
    
};

class Bar {
    a : Int <- 1;
    b : SELF_TYPE;
};
