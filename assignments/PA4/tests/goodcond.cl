class A inherits Int {
};

class B inherits Int {
};

class Main {
    x : A;
    y: B;
    main() : Object {
        if true then x else y fi
    };
    
};
