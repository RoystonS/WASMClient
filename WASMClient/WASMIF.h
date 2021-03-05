#pragma once
#include <windows.h>
#include <stdio.h>
#include <map>
#include <unordered_map>
#include <vector>
#include "SimConnect.h"
#include "ClientDataArea.h"
#include "WASM.h"
#include "Logger.h"
#include "CDAIdBank.h"

using namespace ClientDataAreaMSFS;
using namespace CPlusPlusLogging;
using namespace CDAIdBankMSFS;

using namespace std;

enum WASM_EVENT_ID {
	EVENT_SET_LVAR=1,		// map to StartEventNo + 1
	EVENT_SET_HVAR,			// map to StartEventNo + 2
	EVENT_UPDATE_CDAS,		// map to StartEventNo + 3
	EVENT_LIST_LVARS,		// map to StartEventNo + 4
	EVENT_RELOAD,			// map to StartEventNo + 5
	EVENT_SET_LVARNEG,		// map to StartEventNo + 6
	EVENT_CONFIG_RECEIVED=9,
	EVENT_VALUES_RECEIVED=10, // Only one needed
	EVENT_LVARS_RECEIVED=11, // Allow for MAX_NO_LVAR_CDAS (4)
	EVENT_HVARS_RECEIVED=15, // Allow for MAX_NO_HVAR_CDAS (4)
};

/**
struct _ClientDataArea {
	int id;
	string name;
	int size;
	int noItems;
	int definitionId;
	CDAType type;
} ClientDataArea;
**/
class WASMIF
{
	public:
		static class WASMIF* GetInstance(HWND hWnd, int startEventNo = EVENT_START_NO, void (*loggerFunction)(const char* logString) = nullptr);
		static class WASMIF* GetInstance(HWND hWnd, void (*loggerFunction)(const char* logString));
		bool start();
		void end();
		void createAircraftLvarFile();
		void reload();
		void setLvarUpdateFrequency(int freq);
		int getLvarUpdateFrequency();
		void setLogLevel(LogLevel logLevel);
		double getLvar(int lvarID);
		double getLvar(const char * lvarName);
		void setLvar(unsigned short id, double value);
		void setLvar(unsigned short id, short value);
		void setLvar(unsigned short id, const char *value);
		void setLvar(unsigned short id, unsigned short value);
		void setHvar(int id);
		void listLvars(); // Just print to log for now
		void listHvars(); // Just print to log for now
		void getLvarList(unordered_map<int, string >& returnMap);
		void getHvarList(unordered_map<int, string >& returnMap);
		void executeCalclatorCode(const char *code);

	public:
		static void CALLBACK MyDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);
		static VOID CALLBACK StaticConfigTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
		static VOID CALLBACK StaticRequestDataTimer(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

	protected:
		WASMIF();
		~WASMIF();
	private:
		static DWORD WINAPI StaticSimConnectThreadStart(void* Param);
		void DispatchProc(SIMCONNECT_RECV* pData, DWORD cbData);
		VOID CALLBACK ConfigTimer(HWND hWnd, UINT uMsg, DWORD dwTime);
		VOID CALLBACK RequestDataTimer(HWND hWnd, UINT uMsg, DWORD dwTime);
		volatile  HANDLE hThread = NULL;
		DWORD WINAPI SimConnectStart();
		void SimConnectEnd();
		const char* getEventString(int eventNo);
		void setLvar(DWORD param);

	private:
		static WASMIF* m_Instance;
		HANDLE  hSimConnect;
		HWND hWnd;
		int quit, noLvarCDAs, noHvarCDAs, startEventNo, lvarUpdateFrequency;
		UINT_PTR configTimer;
		UINT_PTR requestTimer;
		BOOL configReceived;
		ClientDataArea* lvar_cdas[MAX_NO_LVAR_CDAS];
		ClientDataArea* hvar_cdas[MAX_NO_HVAR_CDAS];
		ClientDataArea* value_cda;
		static int nextDefinitionID;
		static Logger* pLogger;
		vector<string> lvarNames;
		vector<double> lvarValues;
		vector<string> hvarNames;
		CDAIdBank* cdaIdBank;
		CRITICAL_SECTION        lvarMutex;
};

