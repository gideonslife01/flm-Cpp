// if not define HEADER_H
// -- 만약 HEADER_H심볼이 정의되어 있지 않으면
//    If the HEADER_H symbol is not defined
// define HEADER_H
// -- HEADER_H 심볼을 정의한다.
//    Defines the HEADER_H symbol.
//
#ifndef HEADER_H
#define HEADER_H


// 네임스페이스 / namespace
namespace my {

    // -- 클래스를 사용할 경우 -- /
    // -- When using classes -- /
    //
    // class Server {
    //     public:
    //     int add(int x, int y)
    //     {
    //         return x + y;
    //     }
    // };


    // -- 멤버 함수로 사용할 경우 -- /
    // -- When used as a member function -- /
    int add(int x, int y)
    {
        return x + y;
    }

}

// 헤더 사용에 권장되지 않은 방식
// Not recommended method for using headers
void ftest(){
    std::cout << "ftest message\n" << std::endl;
}

// 헤더 사용에 권장되는 방식
// Recommended way to use headers
void message (std::string& msg);

#endif
