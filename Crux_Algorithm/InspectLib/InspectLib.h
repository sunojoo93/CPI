// ���� ifdef ����� DLL���� ���������ϴ� �۾��� ���� �� �ִ� ��ũ�θ� ����� 
// ǥ�� ����Դϴ�. �� DLL�� ��� �ִ� ������ ��� ����ٿ� ���ǵ� _EXPORTS ��ȣ��
// �����ϵǸ�, �ٸ� ������Ʈ������ �� ��ȣ�� ������ �� �����ϴ�.
// �̷��� �ϸ� �ҽ� ���Ͽ� �� ������ ��� �ִ� �ٸ� ��� ������Ʈ������ 
// INSPECTLIB_API �Լ��� DLL���� �������� ������ ����, �� DLL��
// �� DLL�� �ش� ��ũ�η� ���ǵ� ��ȣ�� ���������� ������ ���ϴ�.
#ifdef INSPECTLIB_EXPORTS
#define INSPECTLIB_API __declspec(dllexport)
#else
#define INSPECTLIB_API __declspec(dllimport)
#endif

// �� Ŭ������ InspectLib.dll���� ������ ���Դϴ�.
class INSPECTLIB_API CInspectLib {
public:
	CInspectLib(void);
	// TODO: ���⿡ �޼��带 �߰��մϴ�.
};

extern INSPECTLIB_API int nInspectLib;

INSPECTLIB_API int fnInspectLib(void);
