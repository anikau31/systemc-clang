#include <systemc.h>
#include <sysc/utils/verif.h>

#define MEM_SIZE 512

SC_MODULE(Memory) {
public:
	typedef enum Function_e {
		FUNC_NONE,
		FUNC_READ,
		FUNC_WRITE
	} Function;
	typedef enum RETSignal_e {
		RSIG_NONE,
		RSIG_READ_FIN,
		RSIG_WRITE_FIN,
		RSIG_ERROR
	} RETSignal;
// 	sc_in<bool> Port_CLK;
	sc_in<Function> Port_Func;
	sc_in<int> Port_Addr;
	sc_inout<int> Port_Data;
	sc_out<RETSignal> Port_DoneSig;
	SC_CTOR(Memory) {
		SC_METHOD(execute);
// 		sensitive << Port_CLK;
		m_clkCnt = 0;
		m_curAddr = 0;
		m_curData = 0;
		m_curFunc = Memory::FUNC_NONE;
		m_data = new int[MEM_SIZE];
		m_writesCnt = 0;
		m_readsCnt = 0;
		m_errorsCnt = 0;
		m_errorCode = 0;
	}
	~Memory() {
		delete [] m_data;
	}
private:
	int m_clkCnt;
	int m_curAddr;
	int m_curData;
	Function m_curFunc;

	int* m_data;
	int m_errorCode;
	int m_writesCnt;
	int m_readsCnt;
	int m_errorsCnt;
	RETSignal myread() {
		if(m_errorCode == 0) {
			Port_Data.write(m_data[m_curAddr]);
			m_readsCnt++;
			return RSIG_READ_FIN;
		} else {
			m_errorsCnt++;
			return RSIG_ERROR;
		}
	}
	RETSignal mywrite() {
		if(m_errorCode == 0) {
			m_data[m_curAddr] = m_curData;
			m_writesCnt++;
			return RSIG_WRITE_FIN;
		} else {
			m_errorsCnt++;
			return RSIG_ERROR;
		}
	}
	void execute() {
		if(m_curFunc != Memory::FUNC_NONE) {
			m_clkCnt++;
			if(m_clkCnt == 100) {
				RETSignal retSig = Memory::RSIG_ERROR;
				switch(m_curFunc) {
				case Memory::FUNC_READ : { retSig = myread(); break; }
				case Memory::FUNC_WRITE : { retSig = mywrite(); break; }
				}
				Port_DoneSig.write(retSig);
				m_clkCnt = 0;
				m_curFunc = Memory::FUNC_NONE;
			}
			return;
		}
		if(Port_Func.read() == Memory::FUNC_NONE) {
			return;
		}
		m_curFunc = Port_Func.read();

		m_curAddr = Port_Addr.read();
		m_curData = Port_Data.read();
		Port_DoneSig.write(Memory::RSIG_NONE);
	}
};



SC_MODULE(CPU) {
public:
//	sc_in<bool> Port_CLK;
	sc_in<Memory::RETSignal> Port_MemDone;
	sc_out<Memory::Function> Port_MemFunc;
	sc_out<int> Port_MemAddr;
	sc_inout<int> Port_MemData;
	SC_CTOR(CPU) {
		SC_METHOD(execCycle);
//		sensitive << Port_CLK;
		dont_initialize();
		SC_METHOD(memDone);
		sensitive << Port_MemDone;
		dont_initialize();
		m_waitMem = false;
	}
private:
	bool m_waitMem;
	Memory::Function getrndfunc() {
		int rndnum=(rand() % 10);

		if(rndnum < 5)
			return Memory::FUNC_READ;
		else
			return Memory::FUNC_WRITE;
	}
	int getRndAddress() {
		return (rand() % MEM_SIZE);
	}
	int getRndData() {
		return rand();
	}
	void execCycle() {
		if(m_waitMem) {
			return;
		}
		int addr = getRndAddress();
		Memory::Function f = getrndfunc();
		Port_MemFunc.write(f);
		Port_MemAddr.write(addr);
		sc_verif::ASSERT(true);
		if(f == Memory::FUNC_WRITE)
			Port_MemData.write(getRndData());
		m_waitMem = true;
	}
	void memDone() {
		if(Port_MemDone.read() == Memory::RSIG_NONE) {
			return;
		}
		m_waitMem = false;
		Port_MemFunc.write(Memory::FUNC_NONE);
	}
};

int sc_main(int argc, char* argv[])
{
	cout << "\n\nCreating Modules............";
/* Instantiate Modules */
	Memory mem("main_memory");
	CPU cpu("cpu");
/* Signals */
	sc_signal<Memory::Function> sigMemFunc("sigMF");
	sc_signal<Memory::RETSignal> sigMemDone("sigMD");
	sc_signal<int> sigMemAddr("sigMA");
	sc_signal<int> sigMemData("sigMData");
/* The clock that will drive the CPU and Memory*/
//	sc_clock clk("essai_clk");
	cout << "DONE\nConnecting Modules' Ports...";
/* Connecting module ports with signals */
	mem.Port_Func(sigMemFunc);
	mem.Port_Addr(sigMemAddr);
	mem.Port_Data(sigMemData);
	mem.Port_DoneSig(sigMemDone);
	cpu.Port_MemFunc(sigMemFunc);
	cpu.Port_MemAddr(sigMemAddr);
	cpu.Port_MemData(sigMemData);
	cpu.Port_MemDone(sigMemDone);
// 	mem.Port_CLK(clk);
// 	cpu.Port_CLK(clk);
	cout << "DONE\n" << endl
	     << "\n\nRunning (press CTRL+C to exit)... ";
/* Start Simulation */
	sc_start();
	return 0;
}
