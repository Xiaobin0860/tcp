/**
 * The first thing to know about are types. The available types in Thrift are:
 *
 *  bool        Boolean, one byte
 *  i8 (byte)   Signed 8-bit integer
 *  i16         Signed 16-bit integer
 *  i32         Signed 32-bit integer
 *  i64         Signed 64-bit integer
 *  double      64-bit floating point value
 *  string      String
 *  binary      Blob (byte array)
 *  map<t1,t2>  Map from one type to another
 *  list<t1>    Ordered list of one type
 *  set<t1>     Set of unique elements of one type
 *
 * Did you also notice that Thrift supports C style comments?
 */
 
namespace cpp Demo
namespace lua Demo
namespace php Demo

enum Type {
    ADD = 1,
    SUBTRACT = 2,
    MULTIPLY = 3,
    DIVIDE = 4
}

struct Work {
    1: i32 num1 = 0,
    2: i64 num2,
    3: Type type,
    4: optional string comment,
    5: string key;
}

struct Worker {
    1: optional list<Work> works,
    2: double result,
    3: map<string, Work> work_map,
}

service Server {
    void send_i(1:i32 num),
    void send_is(1:i32 len, 2:string msg),
}
