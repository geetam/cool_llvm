class Foo inherits Bar {
    x : Int <- a();
    y : Far;
    z : Int <- y.c();
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
