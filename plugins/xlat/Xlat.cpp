#include <regex>
#include <tuple>
#include "SystemCClang.h"
#include "PortBinding.h"
#include "Tree.h"
#include "Xlat.h"
//#include "TemplateParametersMatcher.h"
#include "XlatType.h"
#include "SensitivityMatcher.h"
#include "clang/Basic/FileManager.h"
#include "llvm/Support/Debug.h"

/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "Xlat"

using namespace std;
using namespace hnode;
using namespace systemc_clang;

bool Xlat::postFire() {
  Model *model = getSystemCModel();
  
  std::error_code ec;
  string outputfn;

  FileID fileID = getSourceManager().getMainFileID();
  const FileEntry *fileentry = getSourceManager().getFileEntryForID(fileID);
  if (!fileentry) {
    outputfn = "xlatout";
    LLVM_DEBUG(llvm::dbgs() << "Null file entry for tranlation unit for this astcontext\n");
  } else {
    outputfn = fileentry->getName();
    regex r("\\.cpp");
    outputfn = regex_replace(outputfn, r, "_hdl");

    LLVM_DEBUG(llvm::dbgs() << "File name is " << outputfn << "\n");
  }

  llvm::raw_fd_ostream xlatout(outputfn + ".txt", ec,
                               llvm::sys::fs::CD_CreateAlways);
  LLVM_DEBUG(llvm::dbgs() << "file " << outputfn << ".txt, create error code is " << ec.value() << "\n");
      
  LLVM_DEBUG(llvm::dbgs() << "\n SC  Xlat plugin\n");

  // typedef std::vector< modulePairType > moduleMapType;
  // typedef std::pair<std::string, ModuleDecl *> modulePairType;

  Model::moduleMapType modules = model->getModuleDecl();
  if (modules.size() <= 0) return true;
  Model::modulePairType modpair = modules[0]; // assume first one is top module
  ModuleDecl * mod = modpair.second;

  vector<ModuleDecl *> instanceVec =model->getModuleInstanceMap()[mod];
  if (instanceVec.size()<=0) return true;

  for (auto modinstance: instanceVec) { // generate module def for each instance
    string modname = mod_newn.newname();
    LLVM_DEBUG(llvm::dbgs() << "\ntop level module " << modinstance->getName() << " renamed " << modname<< "\n");
    hNodep h_module = new hNode(modname, hNode::hdlopsEnum::hModule);
    mod_name_map[modinstance->getInstanceDecl()] = {modinstance->getName(), modname, h_module};
    xlatmodule(modinstance, h_module, xlatout);
    //h_module->print(xlatout);
  }
  
   LLVM_DEBUG(llvm::dbgs() << "Global Method Map\n");
  for (auto m : allmethodecls) {
    LLVM_DEBUG(llvm::dbgs() << "Method --------\n" << m.first << ":" << m.second << "\n");
    LLVM_DEBUG(m.second->dump(llvm::dbgs()));
    LLVM_DEBUG(llvm::dbgs() << "---------\n");
  }

  LLVM_DEBUG(llvm::dbgs() << "User Types Map\n");

  while (!xlatt.usertypes.empty()) {
    std::unordered_map<string, QualType> usertypestmp = xlatt.usertypes;
    xlatt.usertypes.clear();
    for (auto t : usertypestmp) {
      LLVM_DEBUG(llvm::dbgs() << "User Type --------\n" << t.first << ":" << t.second.getTypePtr() << "\n");
      LLVM_DEBUG(t.second->dump(llvm::dbgs()));
      LLVM_DEBUG(llvm::dbgs() << "---------\n");
      xlatt.addtype(t.first, t.second, getContext())->print(xlatout);
    }
  }
  return true;
}

void Xlat::xlatmodule(ModuleDecl *mod, hNodep &h_module, llvm::raw_fd_ostream &xlatout ) {
  const std::vector<ModuleDecl*> &submodv = mod->getNestedModuleDecl();
  // look at constructor

  LLVM_DEBUG(llvm::dbgs() << "dumping module constructor stmt\n");

  LLVM_DEBUG(mod->getConstructorStmt()->dump(llvm::dbgs()));        
  //os_ << "dumping module constructor decl\n";                                                            
  LLVM_DEBUG(mod->getConstructorDecl()->dump(llvm::dbgs()));

  LLVM_DEBUG(llvm::dbgs() << "submodule count is " << submodv.size() << "\n");
  typedef std::pair<std::string, systemc_clang::ModuleDecl::portBindingMapType> submodportbindings_t;
  std::vector<submodportbindings_t> submodportbindings;
  for (auto& smod:submodv) {
    LLVM_DEBUG(llvm::dbgs() << "get submodule portbindings" << smod->getInstanceName() << "\n");
    if ((smod->getPortBindings()).size()>0) {
      submodportbindings.push_back(submodportbindings_t(smod->getInstanceName(), smod->getPortBindings()));
    }
  }
          // Ports
  hNodep h_ports = new hNode(hNode::hdlopsEnum::hPortsigvarlist);  // list of ports, signals
  xlatport(mod->getIPorts(), hNode::hdlopsEnum::hPortin,
	   h_ports);
  xlatport(mod->getInputStreamPorts(), hNode::hdlopsEnum::hPortin,
	   h_ports);
  xlatport(mod->getOPorts(), hNode::hdlopsEnum::hPortout,
	   h_ports);
  xlatport(mod->getOutputStreamPorts(), hNode::hdlopsEnum::hPortout,
	   h_ports);
  xlatport(mod->getIOPorts(), hNode::hdlopsEnum::hPortio,
	   h_ports);

  // Signals
  xlatsig(mod->getSignals(), hNode::hdlopsEnum::hSigdecl,
	  h_ports);

  h_module->child_list.push_back(h_ports);
      
  
  xlatport(mod->getOtherVars(), hNode::hdlopsEnum::hVardecl, h_ports);
  //xlatport(mod->getSubmodules(), hNode::hdlopsEnum::hModdecl, h_ports);
  for (const auto &smod:submodv) {
    hNodep h_smod = new hNode(smod->getInstanceName(), hNode::hdlopsEnum::hModdecl);
    h_ports->child_list.push_back(h_smod);
    hNodep h_smodtypinfo = new hNode(hNode::hdlopsEnum::hTypeinfo);
    string newmodname = mod_newn.newname();
    mod_name_map[smod->getInstanceDecl()] = {smod->getInstanceName(), newmodname, h_smod};
    h_smodtypinfo->child_list.push_back(new hNode(newmodname, hNode::hdlopsEnum::hType));
    h_smod->child_list.push_back(h_smodtypinfo);
    }
 
      // Processes
  h_top = new hNode(hNode::hdlopsEnum::hProcesses);

  xlatproc(mod->getProcessMap(), h_top, llvm::dbgs());

  if (!h_top->child_list.empty()) h_module->child_list.push_back(h_top);

  //  typedef std::pair<std::string, PortBinding *> portBindingPairType;
  //  typedef std::map<std::string, PortBinding *> portBindingMapType;
  //   portBindingMapType getPortBindings();

  for (std::pair<std::string, systemc_clang::ModuleDecl::portBindingMapType> pbm: submodportbindings) {
    hNodep h_submodule_pb = new hNode(pbm.first, hNode::hdlopsEnum::hPortbindings);
    xlatportbindings(pbm.second, h_submodule_pb);
    if (!h_submodule_pb->child_list.empty())
      h_module->child_list.push_back(h_submodule_pb);
  }
  h_module->print(xlatout);
  // now generate submodules
  for (const auto &smod:submodv) {
    string modname =  mod_name_map[smod->getInstanceDecl()].newn;
    LLVM_DEBUG(llvm::dbgs() << "generate submodule " << smod->getInstanceName() << " renamed " << modname<< "\n");
    hNodep h_submod = new hNode(modname, hNode::hdlopsEnum::hModule);
    xlatmodule(smod, h_submod, xlatout);
  }
}

void Xlat::xlatportbindings(systemc_clang::ModuleDecl::portBindingMapType portbindingmap, hNodep &h_pbs){
  for (auto const &pb : portbindingmap) {
    string port_name{get<0>(pb)};
    PortBinding *binding{get<1>(pb)};
    LLVM_DEBUG(llvm::dbgs() << "xlat port binding found " << port_name << "<==> " << binding->getBoundToName() << " " << binding->getBoundToParameterVarName() << "\n");
    hNodep hpb = new hNode(hNode::hdlopsEnum::hPortbinding);
    hpb->child_list.push_back(new hNode(port_name, hNode::hdlopsEnum::hVarref));
    string mapped_name = binding->getBoundToParameterVarName().empty()? binding->getBoundToName() :
      binding->getBoundToParameterVarName()+"##"+binding->getBoundToName();
    //hpb->child_list.push_back(new hNode(binding->getBoundToName(), hNode::hdlopsEnum::hVarref));
    hpb->child_list.push_back(new hNode(mapped_name, hNode::hdlopsEnum::hVarref));
    h_pbs->child_list.push_back(hpb);
  }
}

void Xlat::xlatport(ModuleDecl::portMapType pmap, hNode::hdlopsEnum h_op,
                    hNodep &h_info) {

  for (ModuleDecl::portMapType::iterator mit = pmap.begin(); mit != pmap.end();
       mit++) {

    string objname = get<0>(*mit);


    LLVM_DEBUG(llvm::dbgs() << "object name is " << objname << " and h_op is " << h_op << "\n");
    PortDecl *pd = get<1>(*mit);
    Tree<TemplateType> *template_argtp = (pd->getTemplateType())->getTemplateArgTreePtr();

    xlatt.xlattype(objname, template_argtp, h_op, h_info);  // passing the sigvarlist
    // check for initializer
    VarDecl * vard = pd->getAsVarDecl();
    if (vard) {
      LLVM_DEBUG(llvm::dbgs() << "var decl dump follows\n");
      LLVM_DEBUG(vard->dump(llvm::dbgs()));
      if (vard->hasInit()) {
	APValue *apval = vard->getEvaluatedValue();
	if (apval && apval->isInt()) {
	  hNodep h_lit = new hNode((apval->getInt()).toString(10), hNode::hdlopsEnum::hLiteral);
	  hNodep h_varinit = new hNode(hNode::hdlopsEnum::hVarInit);
	  h_varinit->child_list.push_back(h_lit);
	  (h_info->child_list.back())->child_list.push_back(h_varinit);
	}
      }
    }
    else {
      FieldDecl *fieldd = pd->getAsFieldDecl();
      if (fieldd) {
	LLVM_DEBUG(llvm::dbgs() << "field decl dump follows\n");
	LLVM_DEBUG(fieldd->dump(llvm::dbgs()));
	Expr * initializer = fieldd->getInClassInitializer();
	if (initializer!=NULL) {
	  LLVM_DEBUG(llvm::dbgs() << "field initializer dump follows\n");
	  LLVM_DEBUG(initializer->dump(llvm::dbgs()));
	  hNodep h_init = new hNode(hNode::hdlopsEnum::hVarInit);
	  XlatMethod xmethod(initializer, h_init, llvm::dbgs());
	  (h_info->child_list.back())->child_list.push_back(h_init);
	}
	
      }
    }
  }
}

void Xlat::xlatsig(ModuleDecl::signalMapType pmap, hNode::hdlopsEnum h_op,
                   hNodep &h_info) {

  for (ModuleDecl::signalMapType::iterator mit = pmap.begin();
       mit != pmap.end(); mit++) {

    string objname = get<0>(*mit);

    LLVM_DEBUG(llvm::dbgs() << "object name is " << objname << "\n");

    SignalDecl *pd = get<1>(*mit);

    Tree<TemplateType> *template_argtp = (pd->getTemplateTypes())->getTemplateArgTreePtr();
    //  if type is structured, it will be flattened into multiple declarations
    // each with a unique name and Typeinfo followed by Type.
    xlatt.xlattype(objname, template_argtp, h_op, h_info);  // passing the sigvarlist
   
  }
}

void Xlat::xlatproc(ModuleDecl::processMapType pm, hNodep &h_top,
                    llvm::raw_ostream &os) {

  // typedef std::map<std::string, ProcessDecl *> processMapType;
  // processMapType getProcessMap();
  // ProcessDecl::getEntryFunction() returns EntryFunctionContainer*

  for (auto const &pm_entry : pm) {
    ProcessDecl *pd{get<1>(pm_entry)};
    EntryFunctionContainer *efc{pd->getEntryFunction()};
    if (efc->getProcessType() == PROCESS_TYPE::METHOD) {
      hNodep h_process = new hNode(efc->getName(), hNode::hdlopsEnum::hProcess);
      LLVM_DEBUG(llvm::dbgs() << "process " << efc->getName() << "\n");
      // Sensitivity list
      EntryFunctionContainer::SenseMapType sensmap = efc->getSenseMap();
      if (!sensmap.empty()) {
	hNodep h_senslist = new hNode(hNode::hdlopsEnum::hSenslist);
	for (auto sensmapitem : sensmap) {
	  //typedef std::map<std::string, std::vector<SensitivityTupleType>> SenseMapType;
	  //typedef std::tuple<std::string, ValueDecl *, MemberExpr *, DeclRefExpr*>SensitivityTupleType;

	  if (sensmapitem.first == "dont_initialize") // nonsynthesizable
	    continue;
	  
	  LLVM_DEBUG(llvm::dbgs() << "sensmap item " << sensmapitem.first <<"\n");
	  size_t found = sensmapitem.first.find("_handle");
	  //LLVM_DEBUG(llvm::dbgs() << "first part is " << sensmapitem.first.substr(0, found) << "\n");
	  if ((found==std::string::npos) || (sensmapitem.first.substr(0, found).compare(efc->getName())!=0)) {
	    //LLVM_DEBUG(llvm::dbgs() << "compare not equal " << sensmapitem.first.substr(0, found) << " " << efc->getName() << "\n");
	    continue;  // sensitivity item is not for this process
	  }
	  std::vector<EntryFunctionContainer::SensitivityTupleType> sttv = sensmapitem.second;
	  EntryFunctionContainer::SensitivityTupleType sensitem0 = sttv[0];
	  string sensitm = get<1>(sensitem0)->getNameAsString();
	  LLVM_DEBUG(llvm::dbgs() << "sens item " << sensitm << "\n");
	  //LLVM_DEBUG(llvm::dbgs() << " declref follows\n");
	  //LLVM_DEBUG(get<3>(sensitem0)->dump(llvm::dbgs()));
	  hNode::hdlopsEnum h_op;
	  if ((sensitm.compare("pos")==0) || (sensitm.compare("neg")==0)) {
	    h_op = hNode::hdlopsEnum::hSensedge;
	  }
	  else {
	    h_op = hNode::hdlopsEnum::hSensvar;
	  }
	  hNodep h_firstfield = new hNode(sensitm, h_op);
	  for (int i = 1; i < sttv.size(); i++) {
	    sensitm = get<1>(sttv[i])->getNameAsString();
	    if ((sensitm.compare("pos")==0) || (sensitm.compare("neg")==0)) {
	      h_op = hNode::hdlopsEnum::hSensedge;
	    }
	    else {
	      h_op = hNode::hdlopsEnum::hSensvar;
	    }
	    h_firstfield->child_list.push_back(new hNode(sensitm,
							 h_op));
	  }
	  h_senslist->child_list.push_back(h_firstfield);
	}
	h_process->child_list.push_back(h_senslist);
      }
      CXXMethodDecl *emd = efc->getEntryMethod();
      hNodep h_body = new hNode(hNode::hdlopsEnum::hMethod);
      XlatMethod xmethod(emd, h_body, llvm::dbgs());  //, xlatout);
      LLVM_DEBUG(llvm::dbgs() << "Method Map:\n");
      for (auto m : xmethod.methodecls) {
	LLVM_DEBUG(llvm::dbgs() << m.first << ":" << m.second <<"\n");
	//LLVM_DEBUG(m.second->dump(llvm::dbgs()));
      }
      allmethodecls.insert(xmethod.methodecls.begin(), xmethod.methodecls.end());

      h_process->child_list.push_back(h_body);
      h_top->child_list.push_back(h_process);
    } else
      LLVM_DEBUG(llvm::dbgs() << "process " << efc->getName() << " not SC_METHOD, skipping\n");
  }
}

