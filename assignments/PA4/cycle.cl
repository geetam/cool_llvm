
class A inherits C {
};

Class B inherits A {
};

Class C inherits B {
};

Class Main {
	main():C {
	  (new C).init(1,true)
	};
};
