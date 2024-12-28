#include <string>
#include <vector>
#include <math.h>
// 원의 방정식: (x - a)^2 + (y - b)^2 = r^2 임을 응용
// 전제조건은 원점을 기반으로 한다고 했으므로 x^2 + y^2 = r^2가 됨
// 피타고라스 정리를 이용하면 점의 개수를 오차없이 구할 수 있음
using namespace std;

long long solution(int r1, int r2) {
    
    long long answer = 0;
    
    long long inside = pow(r1, 2);
    long long outside = pow(r2, 2);
    // 내부에 있는 선, 경계에 있는 각 원 위의 점의 경계를 계산하는 유일한 경우의 수임
    for(int i = 1; i <= r2; i++) // 1 ≤ r1 < r2 ≤ 1,000,000 이므로 i는 1부터 시작함
    {
        //x^2 + y^2 = r^2를 활용하면 y = sqrt(r^2 - x^2)
        double y1 = sqrt(inside - pow(i, 2)); // 내부 원의 내부y값
        double y2 = sqrt(outside - pow(i, 2)); // 외부 원의 내부 y값
        
        answer += ((long long)y2 - (long long)ceil(y1) + 1); // 내부 원의 내부 y 값을 빼줌
        // ceil은 y1보다  크거나 같은 가장 작은 정수를 반환하기에 2.3의 경우 3을 반환함
        // ceil을 사용하지 않으면 내부 원 경계의 점을 계산해서 y2와 겹치는 점이 생김
    }
    
    answer *= 4;
    
    return answer;
}