class Main {
    x : IO;
    main() : IO {
        {
            x.out_string("10 + 5 = ");
            x.out_int(10 + 5);
            x.out_string("10 - 5 = ");
            x.out_int(10 - 5);
            x.out_string("10 * 5 = ");
            x.out_int(10 * 5);
            x.out_string("10 / 5 = ");
            x.out_int(10 / 5);
        }
    };
};
