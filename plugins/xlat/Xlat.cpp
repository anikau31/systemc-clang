#include <regex>
#include <tuple>
#include "SystemCClang.h"
#include "Xlat.h"
#include "clang/Basic/FileManager.h"

using namespace std;

bool Xlat::postFire() {
  Model *model = getSystemCModel();
  Model::moduleMapType modules = model->getModuleDecl();

  std::error_code ec;
  string outputfn;

  FileID fileID = getSourceManager().getMainFileID();
  const FileEntry *fileentry = getSourceManager().getFileEntryForID(fileID);
  if (!fileentry) {
    outputfn = "xlatout";
    os_ << "Null file entry for tranlation unit for this astcontext\n";
  } else {
    outputfn = fileentry->getName();
    regex r("\\.cpp");
    outputfn = regex_replace(outputfn, r, "_hdl");

    os_ << "File name is " << outputfn << "\n";
  }

  llvm::raw_fd_ostream xlatout(outputfn + ".txt", ec,
                               llvm::sys::fs::CD_CreateAlways);
  os_ << "file " << outputfn << ".txt, create error code is " << ec.value()
      << "\n";
  os_ << "\n SC  Xlat plugin\n";

  for (Model::moduleMapType::iterator mit = modules.begin();
       mit != modules.end(); mit++) {
    // Second is the ModuleDecl type.

    vector<ModuleDecl *> instanceVec =
        model->getModuleInstanceMap()[mit->second];
    for (size_t i = 0; i < instanceVec.size(); i++) {
      hNodep h_module = new hNode(false);
      os_ << "\nmodule " << mit->first << "\n";
      string modname = mit->first + "_" + to_string(i);
      hNodep h_modname = new hNode(modname, hNode::hdlopsEnum::hModule);

      // Ports
      hNodep h_ports = new hNode(false);  // list of ports, signals
      xlatport(instanceVec.at(i)->getIPorts(), hNode::hdlopsEnum::hPortin,
               h_ports);
      xlatport(instanceVec.at(i)->getOPorts(), hNode::hdlopsEnum::hPortout,
               h_ports);
      xlatport(instanceVec.at(i)->getIOPorts(), hNode::hdlopsEnum::hPortio,
               h_ports);

      // Other Variables
      xlatport(instanceVec.at(i)->getOtherVars(), hNode::hdlopsEnum::hVardecl,
               h_ports);

      // Signals
      xlatsig(instanceVec.at(i)->getSignals(), hNode::hdlopsEnum::hSigdecl,
              h_ports);

      h_module->child_list.push_back(h_modname);
      h_module->child_list.push_back(h_ports);

      h_top = new hNode(false);

      // Processes
      xlatproc(instanceVec.at(i)->getEntryFunctionContainer(), h_top, os_);

      h_module->child_list.push_back(h_top);
      h_module->print(xlatout);
      delete h_module;
    }
  }
  return true;
}

void Xlat::xlatport(ModuleDecl::portMapType pmap, hNode::hdlopsEnum h_op,
                    hNodep &h_info) {
  static const std::set<std::string> sctypes = {"sc_in", "sc_out", "sc_inout"};
  for (ModuleDecl::portMapType::iterator mit = pmap.begin(); mit != pmap.end();
       mit++) {
    h_info->child_list.push_back(new hNode(get<0>(*mit), h_op));
    os_ << "object name is " << get<0>(*mit) << "\n";
    PortDecl *pd = get<1>(*mit);
    hNodep h_typeinfo = new hNode(false);
    xlattype(pd->getTemplateType(), h_typeinfo);  // sctypes, xlatout);//, os_);
    h_info->child_list.push_back(h_typeinfo);
  }
}

void Xlat::xlatsig(ModuleDecl::signalMapType pmap, hNode::hdlopsEnum h_op,
                   hNodep &h_info) {
  static const std::set<std::string> sctypes = {"sc_signal"};
  for (ModuleDecl::signalMapType::iterator mit = pmap.begin();
       mit != pmap.end(); mit++) {
    //h_info->child_list.push_back(new hNode(mit->first, h_op));
    h_info->child_list.push_back(new hNode(get<0>(*mit), h_op));
    os_ << "object name is " << get<0>(*mit) << "\n";
    // xlatout << string( n, ' ') << typ;
    // xlatout << mit->first;
    //Signal *pd = mit->second;
    Signal *pd = get<1>(*mit);
    hNodep h_typeinfo = new hNode(false);
    xlattype(pd->getTemplateTypes(), h_typeinfo);  // xlatout);//, os_);
    h_info->child_list.push_back(h_typeinfo);
  }
}

void Xlat::xlattype(FindTemplateTypes *tt, hNodep &h_typeinfo) {
  //tt->printTemplateArguments(os_);

  scpar::FindTemplateTypes::type_vector_t ttargs =
      tt->getTemplateArgumentsType();
  os_ << "number of type args is " << ttargs.size() << "\n";
  for (auto const &targ : ttargs) {
    h_typeinfo->child_list.push_back(
        new hNode(targ.getTypeName(), hNode::hdlopsEnum::hType));
    (targ.getTypePtr())->dump(os_);
  }
}

// void Xlat::xlatproc(scpar::processMapType pmap, hNodep &h_top,
// llvm::raw_ostream &os)
void Xlat::xlatproc(scpar::vector<EntryFunctionContainer *> efv, hNodep &h_top,
                    llvm::raw_ostream &os) {
  for (auto efc : efv) {
    if (efc->getProcessType() == PROCESS_TYPE::METHOD) {
      hNodep h_process = new hNode(false);
      // [process name, process body]
      h_process->child_list.push_back(
          new hNode(efc->getName(), hNode::hdlopsEnum::hProcess));
      os_ << "process " << efc->getName() << "\n";
      // Sensitivity list
      hNodep h_senslist = new hNode(false);
      for (auto sensmap : efc->getSenseMap()) {

	hNodep h_senspair = new hNode(false); // [ (sensvar name) (edge expr) ]
	hNodep h_sensitem = new hNode(sensmap.first, hNode::hdlopsEnum::hSensvar);
	
  // HP: There is a change here. 
  // Sensitivity map returns as its second argument a tuple.  
  // The tuple has two parameters: the edge (pos/neg) and the second is the MemberExpr*.
  // 
  //
	h_senspair->child_list.push_back(h_sensitem);

	string edgeval = get<0>(sensmap.second);

	if (edgeval == "") edgeval = "always";
	hNodep h_edge = new hNode(edgeval, hNode::hdlopsEnum::hSensedge);
	h_senspair->child_list.push_back(h_edge);
	get<1>(sensmap.second)->dump(os_);
	//h_senslist->child_list.push_back(h_sensitem);
	h_senslist->child_list.push_back(h_senspair);
	os_ << "sensitivity item " << sensmap.first << "\n";
      }
      h_process->child_list.push_back(h_senslist);
      CXXMethodDecl *emd = efc->getEntryMethod();
      hNodep h_body = new hNode(false);
      XlatMethod xmethod(emd, h_body, os_);  //, xlatout);
      h_process->child_list.push_back(h_body);
      h_top->child_list.push_back(h_process);
    } else
      os_ << "process " << efc->getName() << " not SC_METHOD, skipping\n";
  }
}
