#pragma once
#define VER_INFO _T("VSAlgorithm Task ver 1.0.1 save image on thread")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*		버전 정보			변경 날짜			작성자			변경내용												참고

		Ver 1.0.0			2020-02-19			신창진S			MURA_NUGI // MURA_EDGE_NUGI 분류 코드 추가				AviInslpection.cpp 내 함수 bool AviInspection::DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara)
																버전 관리 시작

		Ver 1.0.1           2020-04-06			최광일S         Round In 라인 과검 개선 완료							주석 choikwangil 04.06

		Ver 1.0.2			2020-04-07			최광일S			Black hole 중복좌표 제거시 Curx Draw Image에			주석 choikwangil 04.07
																남는 버그 개선

			//					//					//			고객사 조문 요청 Black mura 와 G64 패턴 명점 중복 시						//
																에도 Black mura 판정 추가 (함수 새로 만듬)				DeleteOverlapDefect_Black_Mura_and_Judge()						
		
			//					//					//			R,G,B,Black 패턴 Point 와 White spot 중복 시			DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point
																White spot 제거 함수 ->  Black 패턴 제거 (조문 요청)
			
			//				2020-04.20				//			White spot spec 분할 작업 MURA_WHITE_E_SPOT-> spec2로   주석 choi 04.16 
																TEST

																Test 파라미터 GVArea_per 추가							주석 choi 04.20

			//				2020-05.01				//			White spot Mura3 알고리즘 upgrade						주석 choi 05.01
			//					//					//			White spot & Bright Point 중복시 WP제거로 원복               //
																샤오만 요청	
			
			//				2020-05.07				//			DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point		주석 choi 05.07
																함수 주석 white spot 과 Bright Point의 구분을 위한
																함수 였지만 GVArea_Per로 구분 가능 할듯 하여 임시 주석

			//					//					//			DeleteOverlapDefect_Black_Mura_and_Judge 함수			주석 choi 05.07
																Black hole 영역 offset 줌

		Ver 1.0.3			2020-05-14				//			B7 GoldenCell (MuraNormal) Merge 완료               

		Ver 1.0.3			2020-05-20				//			NEW WHITE SPOT JUDGE 추가 
		 
		   //               2020-06-04              //          중복 좌표 제거 mura-mura , mura-line 조문 요청 변경      주석 choi 06.04

		   //					//					//			배경 mask 수정 G패턴 값 따로 설정 필요해서				 주석 choi 06.04		

		   //					//					//			mura 알고리즘 조금 수정	Edge 과검 제거를 위해 
																Edge Active 나누고 spec 나눔 기능 추가					 주석 choi 06.04
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


*/