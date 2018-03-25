class Foo inherits Bar {
    x : Int <- a();
    y : Bool <- b();
    z : Int <- c();
};

class Bar {
    a() : Int {
        1 
    };
    
    b(y : Int) : Bool {
        true
    };
};
