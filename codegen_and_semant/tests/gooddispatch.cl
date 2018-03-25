class Foo inherits Bar {
    x : Int <- a();
    y : Far;
    z : Int <- y.c();
    p : SELF_TYPE;
    q : Int <- p.a();
};

class Bar {
    a() : Int {
        1 
    };
    
    b(y : Int) : Bool {
        true
    };
};

class Far {
    c() : Int {1};
};
