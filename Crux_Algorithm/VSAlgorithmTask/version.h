#pragma once
#define VER_INFO _T("VSAlgorithm Task ver 1.0.1 save image on thread")

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*		���� ����			���� ��¥			�ۼ���			���泻��												����

		Ver 1.0.0			2020-02-19			��â��S			MURA_NUGI // MURA_EDGE_NUGI �з� �ڵ� �߰�				AviInslpection.cpp �� �Լ� bool AviInspection::DeleteOverlapDefect(ResultPanelData& resultPanelData, double* dAlignPara)
																���� ���� ����

		Ver 1.0.1           2020-04-06			�ֱ���S         Round In ���� ���� ���� �Ϸ�							�ּ� choikwangil 04.06

		Ver 1.0.2			2020-04-07			�ֱ���S			Black hole �ߺ���ǥ ���Ž� Curx Draw Image��			�ּ� choikwangil 04.07
																���� ���� ����

			//					//					//			���� ���� ��û Black mura �� G64 ���� ���� �ߺ� ��						//
																���� Black mura ���� �߰� (�Լ� ���� ����)				DeleteOverlapDefect_Black_Mura_and_Judge()						
		
			//					//					//			R,G,B,Black ���� Point �� White spot �ߺ� ��			DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point
																White spot ���� �Լ� ->  Black ���� ���� (���� ��û)
			
			//				2020-04.20				//			White spot spec ���� �۾� MURA_WHITE_E_SPOT-> spec2��   �ּ� choi 04.16 
																TEST

																Test �Ķ���� GVArea_per �߰�							�ּ� choi 04.20

			//				2020-05.01				//			White spot Mura3 �˰��� upgrade						�ּ� choi 05.01
			//					//					//			White spot & Bright Point �ߺ��� WP���ŷ� ����               //
																������ ��û	
			
			//				2020-05.07				//			DeleteOverlapDefect_White_Spot_Mura_RGBBlk_Point		�ּ� choi 05.07
																�Լ� �ּ� white spot �� Bright Point�� ������ ����
																�Լ� ������ GVArea_Per�� ���� ���� �ҵ� �Ͽ� �ӽ� �ּ�

			//					//					//			DeleteOverlapDefect_Black_Mura_and_Judge �Լ�			�ּ� choi 05.07
																Black hole ���� offset ��

		Ver 1.0.3			2020-05-14				//			B7 GoldenCell (MuraNormal) Merge �Ϸ�               

		Ver 1.0.3			2020-05-20				//			NEW WHITE SPOT JUDGE �߰� 
		 
		   //               2020-06-04              //          �ߺ� ��ǥ ���� mura-mura , mura-line ���� ��û ����      �ּ� choi 06.04

		   //					//					//			��� mask ���� G���� �� ���� ���� �ʿ��ؼ�				 �ּ� choi 06.04		

		   //					//					//			mura �˰��� ���� ����	Edge ���� ���Ÿ� ���� 
																Edge Active ������ spec ���� ��� �߰�					 �ּ� choi 06.04
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


*/