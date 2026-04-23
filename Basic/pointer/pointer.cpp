#include <iostream>
#include <string>
#include "header.h" // 헤더 포함 / Includes header

using namespace std; // std 네임스페이스 사용하기 / Using the std namespace

void message(string& msg){
    cout << msg << "\n" << endl;
}

std::string my_message = "pointer complete!!";
int main(){

    // -- 헤더에 클래로 선언된 경우 사용 형식 -- //
    // -- Usage format when declared as a class in the header -- //
    // <네임스페이스>::<클래스네임> <객체>
    // my::Server svr;
    //

    // 일반 string변수 / General string variable
	string name = "Ayloy";

	// *ptr => 포인터변수 / Pointer Variable
	// &food => 일반변수의 주소 값 / Address value of a regular variable
	string* ptv = &name;

	// 변수 출력 /. variable output
	cout << name << "\n";
	cout << &name << "\n";
	cout << ptv << "\n";

	// print value
	cout << *ptv << "\n";

	// 포인터변수에 값 변경
	// change pointer value
	*ptv = "SilverHand";
	cout << *ptv << "\n";

	// 헤더에 클래스로선언된 경우 / When declared as a class in the header
    //cout << "The sum of 1 and 2 is " << svr.add(1, 2) << std::endl;

    // 헤더에 멤버 함수로 선언된 경우 / When declared as a member function in the header
    cout << "The sum of 1 and 2 is " << my::add(1, 2) << std::endl;

    // ftest함수 실행 / Excute ftewt function
    ftest();
    // message함수 실행, 참조로 전달 / Execute message function, passed by reference
    message(my_message);
	return 0;
}
