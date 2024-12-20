#include <string>
#include <vector>

using namespace std;

int solution(vector<int> ingredient) {
    int packed_hb = 0; // 포장을 마쳐서 팔릴 준비가 된 햄버거 개수 저장 변수
    vector<int> packaging_stack; // 포장작업 구조화 스택 생성
    vector<int> assemble_hb = {1, 2, 3, 1}; // 햄버거 포장 구성 스택 

    for (int item : ingredient) // 재료를 하나씩 꺼내서 반복
    {
        packaging_stack.push_back(item); // 포장작업 스택에 재료 추가
        // for문을 계속하기 위한 전제조건
        // 1. 포장 작업할 재료 잔고가 4개 이상 있어야 함 1, 2, 3, 1 4개
        // 2. 포장작업 스택의 마지막 원소 4개가 햄버거 구성 스택과 일치해야 함
        if (packaging_stack.size() >= 4 && 
            equal(packaging_stack.end() - 4, packaging_stack.end(), assemble_hb.begin())) // 전제조건을 모두 반영한 뚱뚱한 조건문, python의 슬라이싱[-4:]과 비슷한 기능
        {
            packed_hb++; // 포장완료한 햄버거 개수 증가
            packaging_stack.erase(packaging_stack.end() - 4, packaging_stack.end()); // 포장작업을 마친 재료는 제거됨
        }
    }

    int answer = packed_hb;
    return answer;
}