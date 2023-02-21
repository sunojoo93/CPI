/*
2019.04.08 add by JHJ
성도 판다 대형
Phase1, Phase2, Phase3
AVL : 21(PC9), 22, 23, 24, 25 (PH1)  /  41, 42 (PH2)  /  43, 44, 45(PH3)(PC9)
ABI : 21(PC9), 22, 23, 24     (PH1)  /  41, 42 (PH2)  /  43, 44    (PH3)(PC9)

- 버전관리 Phase3 (AVL) 3.0.0 기준으로 시작
*/

#define STR_CURRENT_VERSION _T("L 3.0.7")

// 2019.04.08 -  JHJ  - 3.0.0 : UI 관련 리스트 컨트롤 MAX 갯수 수정, PC Conenct 딜레이 추가, 버전추가, 통합 버전
// 2019.04.15 -  JHJ  - 3.0.1 : B, C Grade TimeOverlap 기능 추가
// 2019.04.15 -  JHJ  - 3.0.2 : 디펙별 Cim 보고 Ng Flag 분류 기능 추가
// 2019.04.16 -  JHJ  - 3.0.3 : ADDR_PROCESS_MEASURE_VALUE_ZERO 측정 알람 수정
// 2019.04.18 -  JHJ  - 3.0.4 : nFlagCount 로그 추가
// 2019.04.19 -  JHJ  - 3.0.5 : Sub Grade CF, TFT Broken / CF, TFT Broken Value 충돌 구분 수정
// 2019.06.15 - ryu   - 1.0.6 : B163D알람 추가 , CIM 통신 끊김
//2019.06.21 - ryu   - 1.0.6 : EQ, Stage 데이터 추가 , 3word 변ㄴ경 