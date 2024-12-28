#include <string>
#include <vector>

using namespace std;

int solution(int a, int b) {
    int answer = 0;
    int fb = stoi(to_string(a) + to_string(b)); // a ⊕ b의 경우  front + back
    int bf = stoi(to_string(b) + to_string(a)); // b ⊕ a의 경우  back + front
    if (fb >= bf){  // >= 로 비교하여 코드 단축
        answer = fb;} // a ⊕ b와 b ⊕ a가 같거나 a ⊕ b가 b ⊕ a보다 크다면 a ⊕ b를 return 합니다.
    else if (fb < bf){
        answer = bf;} // b ⊕ a가 a ⊕ b보다 크다면 b ⊕ a를 return 합니다.
    
    return answer;
}