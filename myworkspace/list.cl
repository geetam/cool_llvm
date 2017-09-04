class List inherits A2I{
    item: Object;
    next: List;
    
    init(it: Object, nxt: List): List {
        {
            item <- it;
            next <- nxt;
            self;
        }
    };
    
    flatten(): String {
       let str: String <-
            case item of
                i: Int => i2a(i);
                s: String => s;
                o: Object => {abort(); "";};
            esac
        in
        
            if (isvoid next) then
                str
            else
                str.concat(next.flatten())
            fi
    };
    
};
    
    

class Main inherits IO {
    main(): Object {
        let hello: String <- "Hello",
            world: String <- "World!",
            newline: String <- "\n",
            nil: List,
            list: List <- (new List).init(hello,
                        (new List).init(world,
                                (new List).init(newline, nil)
                        )
                   )
        in 
            out_string(list.flatten())
    };
};
    
