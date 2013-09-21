#include "Utility.h"

using namespace scpar;

void Utility::tabit(raw_ostream& os, int tabn) {
  for (int i = 0; i < tabn; i++) {
    os << " ";
  }
}

string Utility::strip(string s, string sub) {
  // sub has "struct "
  size_t pos = s.find(sub);
  if (pos == string::npos) {
    return s;
  }
  
  return s.erase(pos, sub.length()); 
}

Utility::Utility() {

}

