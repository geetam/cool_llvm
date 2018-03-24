class Main {
    io_obj: IO;
    x: Int <- 0;
    main() : Int {
        {
            while x < 5
            loop
            {
                io_obj.out_int(x);
                x <- x + 1;
            }
            pool;
            1;
        }
    };
};
