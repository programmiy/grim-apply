#include <string>
#include <vector>

using namespace std;

int solution(int n, int k) {
    int lambPrice = 12000;
    int drinkPrice = 2000;
    int serviceDrink =  n / 10; //  10개당 음료수 한개 무료, 무료인 음료수의 개수 저장 변수
    int totalCost = (lambPrice * n) + ((k - serviceDrink) * drinkPrice); // 음료수 중 무료 음료수를 가격에서 배제 후 계산
    return totalCost;
}