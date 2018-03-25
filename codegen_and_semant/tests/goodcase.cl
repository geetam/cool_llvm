class X {
};
class A inherits X {
};
class P inherits X {
};

class B inherits A {
};

class C inherits A {    
};

class D inherits A {
};


class Main {
    b : B;
    c : C;
    d : D;
    p : P;
    main() : Object {
        case true of
            x : Bool => b;
            y : Int => c;
            z : String => d;
            t : Object => p;
        esac
    };
    
};
