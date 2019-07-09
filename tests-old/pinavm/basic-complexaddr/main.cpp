/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

/* Plateforme SystemC utilisant le protocole TLM BASIC.

   Cette version est en un seul fichier pour faciliter la
   compréhension. Une version multi-fichiers (donc plus propre) est
   également disponible.
*/

/*
    Testing complexe addresses resolution
 */

#include "basic.h"
#include "bus.h"

// FIXME: move this code in a bytecode library
extern "C" void tweto_call_process_method(sc_core::SC_ENTRY_FUNC vfct, sc_core::sc_process_host *host);
void tweto_call_process_method(sc_core::SC_ENTRY_FUNC vfct, sc_core::sc_process_host *host) {
    (host->*vfct)();
}
extern "C" void tweto_call_write_method(basic::target_module_base *module, 
                                        basic::addr_t a, basic::data_t d);
void tweto_call_write_method(basic::target_module_base *module, 
                             basic::addr_t a, basic::data_t d) {
    module->write(a, d);
}
extern "C" void tweto_call_read_method(basic::target_module_base *module, 
                                       basic::addr_t a, basic::data_t d);
void tweto_call_read_method(basic::target_module_base *module, 
                            basic::addr_t a, basic::data_t d) {
    module->read(a, d);
}

using namespace std;
using namespace sc_core;

struct initiator : sc_module {
        basic::initiator_socket<initiator> socket;
        void thread(void) {
            
        // Address = 0x64
        basic::addr_t addr1 = 8/2;
		basic::addr_t addr2 = 32+16;
        basic::addr_t addr3 = addr1*4 + addr2;
            
		for (basic::data_t val = 1; val <= 10; val++) {
			val++;
                        socket.write(addr3, val);
                }
        }
        SC_CTOR(initiator) {
                SC_THREAD(thread);
        }
};

struct target : sc_module, basic::target_module_base {
        basic::target_socket<target> socket;
        tlm::tlm_response_status write(const basic::addr_t &a,
                                       const basic::data_t &d) {
                cout << "j'ai reçu : " << d << endl;
                return tlm::TLM_OK_RESPONSE;
        }
        tlm::tlm_response_status read (const basic::addr_t &a,
                                       /* */ basic::data_t &d) {
                SC_REPORT_ERROR("TLM", "non implémenté");
                abort();
        }
        SC_CTOR(target) {/* */}
};

int sc_main (int argc, char ** argv) {
        /*
         +---------+    +-------------+    +--------+
         |        +++  +-+           +++  +++       |
         | Alice  | +--+ |  Router   | +--+ |  Bob  |
         |        +++  +-+           +++  +++       |
         +---------+    +-------------+    +--------+
         */

        initiator a("Alice");
        target b("Bob");
        Bus router("Router");

        /* Bob is mapped at addresses [0, 100[, i.e. ... */
        router.map(b.socket, 0, 100);

        /* connect components to the bus */
        a.socket.bind(router.target);
        router.initiator.bind(b.socket);

        /* and start simulation */
        sc_start();
        return 0;
}
