#include <string>
#include <vector>

using namespace std;

int solution(string my_string, string is_prefix) {
    int answer = 1; // 디폴트를 1로 해서 코드 단축
    if (my_string.length() < is_prefix.length()) {
        answer = 0; // 1차적으로 접두사가 my_string보다 길면 접두사가 될 수 없음
    }
    for (int i = 0; i < is_prefix.size(); i++) // is_prefix의 길이만큼 반복
    {
        if (my_string[i] != is_prefix[i]) // 같은 자리의 문자와 일치하지 않으면 0을 반환
        {
            answer = 0;
            break;
        }
    }  
    return answer;
}