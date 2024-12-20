#include <string>
#include <vector>

using namespace std;

int solution(vector<vector<int>> board, int k) {
    int answer = 0;
    // 0 ≤ k < board의 길이 + board[i]의 길이 이므로 k가 board의 길이보다 크거나 같을 경우는 없음
    for (int i = 0; i < board.size(); i++) { // i의 크기가 k보다 커질 경우가 존재 <= 가 아닌 <
        for (int j = 0; j <= k - i && j < board[i].size(); j++) { // board의 크기가 4x3이고 k = 5인 경우 방지
            answer += board[i][j]; // 가능한 항의 값은 즉각 최종 값에 더해줌
        }
        //j의 반복 조건을 j <= k - i로 설정했기 때문에, 
        // i + j가 항상 k 이하가 됨, 따라서 추가적인 if 조건 없이도 answer에 더할 수 있음
    }
    return answer;
}