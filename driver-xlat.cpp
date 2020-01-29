#include "plugins/xlat/Xlat.h"
#include "PluginAction.h"
using namespace scpar;
using namespace std;

#include "plugins/xlat/Xlat.h"

int main(int argc, const char **argv) {
  cout << "Plugin XLAT\n";
    PluginAction<Xlat> pa(argc, argv);
  return 0;
}
