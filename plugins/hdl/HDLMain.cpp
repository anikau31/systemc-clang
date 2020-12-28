// clang-format off
#include <regex>
#include <tuple>
#include "SystemCClang.h"
#include "PortBinding.h"
#include "Tree.h"
#include "HDLMain.h"
//#include "TemplateParametersMatcher.h"
#include "HDLType.h"
#include "SensitivityMatcher.h"
#include "clang/Basic/FileManager.h"
#include "llvm/Support/Debug.h"
#include "clang/Basic/Diagnostic.h"
#include "HDLHnode.h"

// clang-format on
/// Different matchers may use different DEBUG_TYPE
#undef DEBUG_TYPE
#define DEBUG_TYPE "HDL"

using namespace std;
using namespace hnode;
using namespace systemc_clang;
using namespace clang;

namespace systemc_hdl {

  std::unique_ptr<clang::tooling::FrontendActionFactory>
  newFrontendActionFactory(const std::string &top_module) {
    return std::unique_ptr<tooling::FrontendActionFactory>(
	   new HDLFrontendActionFactory(top_module));
  }


  //!
  //! Entry point for HDL generation
  //!
  //! Starting with top level module, process systemc objects
  //! Generate hcode for modules and submodules, including their
  //! ports and variables, port bindings, sensitivity lists
  //! Generate hcode for SC_METHOD body declarations
  //!
  bool HDLMain::postFire() {
    Model *model = getSystemCModel();

    std::error_code ec;
    string outputfn;

    FileID fileID = getSourceManager().getMainFileID();
    const FileEntry *fileentry = getSourceManager().getFileEntryForID(fileID);
    if (!fileentry) {
      outputfn = "HCodeout";
      LLVM_DEBUG(llvm::dbgs()
		 << "Null file entry for tranlation unit for this astcontext\n");
    } else {
      outputfn = fileentry->getName();
      regex r("\\.cpp");
      outputfn = regex_replace(outputfn, r, "_hdl");

      LLVM_DEBUG(llvm::dbgs() << "File name is " << outputfn << "\n");
    }

    llvm::raw_fd_ostream HCodeOut(outputfn + ".txt", ec,
				  llvm::sys::fs::CD_CreateAlways);
    LLVM_DEBUG(llvm::dbgs() << "file " << outputfn
	       << ".txt, create error code is " << ec.value()
	       << "\n");

    LLVM_DEBUG(llvm::dbgs() << "\n SC  HDL plugin\n");

    // typedef std::vector< modulePairType > moduleMapType;
    // typedef std::pair<std::string, ModuleInstance *> modulePairType;


    ModuleInstance *modinstance{model->getRootModuleInstance()};
    if (modinstance == nullptr) {
      LLVM_DEBUG(llvm::dbgs() << "\nRoot instance not found, exiting\n");
      return false;
    }

    // generate module instance for top module and its submodules
    string modname = mod_newn.newname();
    LLVM_DEBUG(llvm::dbgs() << "\ntop level module " << modinstance->getName()
	       << " renamed " << modname << "\n");
    hNodep h_module = new hNode(modname, hNode::hdlopsEnum::hModule);
    //mod_name_map[modinstance->getInstanceDecl()] = {modinstance->getName(),
    mod_name_map[modinstance] = {modinstance->getName(),
				 modname, h_module};
    SCmodule2hcode(modinstance, h_module, HCodeOut);
    // h_module->print(HCodeOut);
  

    LLVM_DEBUG(llvm::dbgs() << "User Types Map\n");

    while (!HDLt.usertypes.empty()) {
      std::unordered_map<string, QualType> usertypestmp = HDLt.usertypes;
      HDLt.usertypes.clear();
      for (auto t : usertypestmp) {
	LLVM_DEBUG(llvm::dbgs()
		   << "User Type --------\n"
		   << t.first << ":" << t.second.getTypePtr() << "\n");
	LLVM_DEBUG(t.second->dump(llvm::dbgs()));
	LLVM_DEBUG(llvm::dbgs() << "---------\n");
	HDLt.addtype(t.first, t.second, getContext())->print(HCodeOut);
      }
    }
    return true;
  }

  void HDLMain::SCmodule2hcode(ModuleInstance *mod, hNodep &h_module,
			       llvm::raw_fd_ostream &HCodeOut) {
    const std::vector<ModuleInstance *> &submodv = mod->getNestedModuleInstances();
    // look at constructor

    // LLVM_DEBUG(llvm::dbgs() << "dumping module constructor stmt\n");

    // LLVM_DEBUG(mod->getConstructorStmt()->dump(llvm::dbgs()));
    LLVM_DEBUG( llvm::dbgs() << "dumping module constructor decl body\n");
    LLVM_DEBUG(mod->getConstructorDecl()->getBody()->dump());
    LLVM_DEBUG( llvm::dbgs() << "end dumping module constructor decl body\n");
    //LLVM_DEBUG(mod->getConstructorDecl()->dump(llvm::dbgs()));
  
    LLVM_DEBUG(llvm::dbgs() << "submodule count is " << submodv.size() << "\n");

    // Ports
    hNodep h_ports =
      new hNode(hNode::hdlopsEnum::hPortsigvarlist);  // list of ports, signals
    SCport2hcode(mod->getIPorts(), hNode::hdlopsEnum::hPortin, h_ports);
    SCport2hcode(mod->getInputStreamPorts(), hNode::hdlopsEnum::hPortin, h_ports);
    SCport2hcode(mod->getOPorts(), hNode::hdlopsEnum::hPortout, h_ports);
    SCport2hcode(mod->getOutputStreamPorts(), hNode::hdlopsEnum::hPortout,
		 h_ports);
    SCport2hcode(mod->getIOPorts(), hNode::hdlopsEnum::hPortio, h_ports);

    // Signals
    SCsig2hcode(mod->getSignals(), hNode::hdlopsEnum::hSigdecl, h_ports);

    h_module->child_list.push_back(h_ports);

    SCport2hcode(mod->getOtherVars(), hNode::hdlopsEnum::hVardecl, h_ports);

    for (const auto &smod : submodv) {
      if (smod->getInstanceInfo().isArrayType()) {
	LLVM_DEBUG(llvm::dbgs() << "Array submodule " << smod->getInstanceName() << "\n");
      }
      else LLVM_DEBUG(llvm::dbgs() << "Non-Array submodule " << smod->getInstanceName() << "\n");
      const std::vector<std::string> &instnames{smod->getInstanceInfo().getInstanceNames()};
      string newmodname = mod_newn.newname();
      for (auto instname: instnames) {
	LLVM_DEBUG(llvm::dbgs() << "Instance " << instname << "\n");
      
	hNodep h_smod =
	  new hNode(instname, hNode::hdlopsEnum::hModdecl);
	h_ports->child_list.push_back(h_smod);
	hNodep h_smodtypinfo = new hNode(hNode::hdlopsEnum::hTypeinfo);

	mod_name_map[smod] = {instname,
			      newmodname, h_smod};
	h_smodtypinfo->child_list.push_back(
					    new hNode(newmodname, hNode::hdlopsEnum::hType));
	h_smod->child_list.push_back(h_smodtypinfo);
      }
    }

    // Processes
    h_top = new hNode(hNode::hdlopsEnum::hProcesses);
    SCproc2hcode(mod->getProcessMap(), h_top);
    if (!h_top->child_list.empty()) h_module->child_list.push_back(h_top);

    {
      // init block 
      clang::DiagnosticsEngine &diag_engine{getContext().getDiagnostics()};
      hNodep hconstructor = new hNode(mod->getInstanceName(), hNode::hdlopsEnum::hModinitblock);
      HDLBody xconstructor(mod->getConstructorDecl()->getBody(), hconstructor, diag_engine);
      LLVM_DEBUG(llvm::dbgs() << "HDL output for module body\n");
      hconstructor->print(llvm::dbgs());
      HDLConstructorHcode hcxxbody;
      h_module->child_list.push_back(hcxxbody.ProcessCXXConstructorHcode(hconstructor));
      //hconstructor->print(HCodeOut);

      // diag_engine scope ends
    }
  
    //  typedef std::pair<std::string, PortBinding *> portBindingPairType;
    //  typedef std::map<std::string, PortBinding *> portBindingMapType;
    //   portBindingMapType getPortBindings();
    // We are doing port bindings in the init block now
    // hNodep h_submodule_pb =
    //   new hNode(mod->getInstanceName()+"_portbindings", hNode::hdlopsEnum::hPortbindings);
    // SCportbindings2hcode(mod, h_submodule_pb);
    // if (!h_submodule_pb->child_list.empty())
    //   h_module->child_list.push_back(h_submodule_pb);
  

    if (allmethodecls.size() > 0) {
      LLVM_DEBUG(llvm::dbgs() << "Module Method/Function Map\n");
      std::unordered_map<string, FunctionDecl *> modmethodecls;
      modmethodecls =
        std::move(allmethodecls);  // procedures/functions found in this module
      LLVM_DEBUG(llvm::dbgs()
		 << "size of allmethodecls is " << allmethodecls.size() << "\n");
      LLVM_DEBUG(llvm::dbgs()
		 << "size of modmethodecls is " << modmethodecls.size() << "\n");
      for (auto m : modmethodecls) {
	LLVM_DEBUG(llvm::dbgs() << "Method --------\n"
		   << m.first << ":" << m.second << "\n");
	// LLVM_DEBUG(m.second->dump(llvm::dbgs()));
	LLVM_DEBUG(llvm::dbgs() << "---------\n");
	clang::DiagnosticsEngine &diag_engine{getContext().getDiagnostics()};
	if (m.second->hasBody()) {
	  hNodep hfunc = new hNode(m.first, hNode::hdlopsEnum::hFunction);
	  QualType qrettype = m.second->getDeclaredReturnType();
	  const clang::Type *rettype = qrettype.getTypePtr();
	  FindTemplateTypes *te = new FindTemplateTypes();
	  te->Enumerate(rettype);
	  HDLType HDLt;
	  HDLt.SCtype2hcode("", te->getTemplateArgTreePtr(), NULL,
			    hNode::hdlopsEnum::hFunctionRetType, hfunc);
	  if (m.second->getNumParams() > 0) {
	    hNodep hparams = new hNode(hNode::hdlopsEnum::hFunctionParams);
	    hfunc->child_list.push_back(hparams);
	    for (int i = 0; i < m.second->getNumParams(); i++) {
	      VarDecl *vardecl = m.second->getParamDecl(i);
	      QualType q = vardecl->getType();
	      const clang::Type *tp = q.getTypePtr();
	      LLVM_DEBUG(llvm::dbgs() << "ProcessParmVarDecl type name is "
			 << q.getAsString() << "\n");
	      FindTemplateTypes *te = new FindTemplateTypes();
	      te->Enumerate(tp);
	      HDLType HDLt;
	      HDLt.SCtype2hcode(vardecl->getName(), te->getTemplateArgTreePtr(),
				NULL, hNode::hdlopsEnum::hVardecl, hparams);
	    }
	    HDLBody xfunction(m.second->getBody(), hfunc, diag_engine);
	  } else {
	    HDLBody xfunction(m.second->getBody(), hfunc, diag_engine);
	  }
	  h_top->child_list.push_back(hfunc);
	  // LLVM_DEBUG(m.second->dump(llvm::dbgs()));
	}
      }
    }

    h_module->print(HCodeOut);
    // now generate submodules
    for (const auto &smod : submodv) {
      //for (int i = 0; i < smod->getInstanceInfo().instance_names.size(); i++) {
      //string modname = mod_name_map[smod->getInstanceDecl()].newn;
      string modname = mod_name_map[smod].newn;
      LLVM_DEBUG(llvm::dbgs() << "generate submodule " << smod->getName() //smod->getInstanceInfo().instance_names[i] //getInstanceName()
		 << " renamed " << modname << "\n");
      hNodep h_submod = new hNode(modname, hNode::hdlopsEnum::hModule);
      SCmodule2hcode(smod, h_submod, HCodeOut);
      // }
    }
  }

  void HDLMain::SCportbindings2hcode(ModuleInstance* mod,
				     //systemc_clang::ModuleInstance::portBindingMapType portbindingmap,
				     hNodep &h_pbs) {
    systemc_clang::ModuleInstance::portBindingMapType portbindingmap{mod->getPortBindings()};
    for (auto const &pb : portbindingmap) {
      PortBinding *binding{get<1>(pb)};
      string port_name{binding->getCallerPortName()};
      LLVM_DEBUG(llvm::dbgs() << "SC port binding found Caller port name  " << port_name
		 << " caller instance name " << binding->getCallerInstanceName()
		 << " <==> callee port name " << binding->getCalleePortName() <<
		 " callee instance name " 
		 << binding->getCalleeInstanceName() << "\n");
      if (binding->getCallerArraySubscripts().size() >0)
	{
	  LLVM_DEBUG(llvm::dbgs() << "Caller Subscript vector length is " <<
		     binding->getCallerArraySubscripts().size() << "\n");
	  for (auto subscriptex: binding->getCallerArraySubscripts()) {
	    LLVM_DEBUG(subscriptex->dump(llvm::dbgs()));
	  }
	}
      if (binding->getCalleeArraySubscripts().size()>0) {
	LLVM_DEBUG(llvm::dbgs() << "Callee Subscript vector length is " <<
		   binding->getCalleeArraySubscripts().size() << "\n");
	for (auto subscriptex: binding->getCalleeArraySubscripts()) {
	  LLVM_DEBUG(subscriptex->dump(llvm::dbgs()));
	}
      }
      
      hNodep hpb = new hNode(binding->getCallerInstanceName(), hNode::hdlopsEnum::hPortbinding);
      // caller module name 
      hNodep hpb_caller = new hNode(port_name, hNode::hdlopsEnum::hVarref);
      if (binding->getCallerPortArraySubscripts().size() >0) {
	hpb_caller->child_list.push_back(new hNode("INDEX", hNode::hdlopsEnum::hLiteral)); //placeholder
      }
      hpb->child_list.push_back(hpb_caller);
      string mapped_name =  binding->getCalleeInstanceName();
                             
      // hpb->child_list.push_back(new hNode(binding->getBoundToName(),
      // hNode::hdlopsEnum::hVarref));
      hNodep hpb_callee = new hNode(mapped_name, hNode::hdlopsEnum::hVarref);
      if (binding->getCalleeArraySubscripts().size() >0) {
	hpb_callee->child_list.push_back(new hNode("INDEX", hNode::hdlopsEnum::hLiteral)); //placeholder
      }
      hpb->child_list.push_back(hpb_callee);
    
      h_pbs->child_list.push_back(hpb);
    }
  }

  void HDLMain::SCport2hcode(ModuleInstance::portMapType pmap, hNode::hdlopsEnum h_op,
			     hNodep &h_info) {
    clang::DiagnosticsEngine &diag_engine{getContext().getDiagnostics()};

    for (ModuleInstance::portMapType::iterator mit = pmap.begin(); mit != pmap.end();
	 mit++) {
      string objname = get<0>(*mit);

      LLVM_DEBUG(llvm::dbgs() << "object name is " << objname << " and h_op is "
		 << h_op << "\n");
      PortDecl *pd = get<1>(*mit);
      Tree<TemplateType> *template_argtp =
        (pd->getTemplateType())->getTemplateArgTreePtr();

      std::vector<llvm::APInt> array_sizes = pd->getArraySizes();
      HDLt.SCtype2hcode(objname, template_argtp, &array_sizes, h_op,
			h_info);  // passing the sigvarlist
      // check for initializer
      VarDecl *vard = pd->getAsVarDecl();
      if (vard) {
	LLVM_DEBUG(llvm::dbgs() << "var decl dump follows\n");
	LLVM_DEBUG(vard->dump(llvm::dbgs()));
	if (vard->hasInit()) {
	  APValue *apval = vard->getEvaluatedValue();
	  if (apval && apval->isInt()) {
	    hNodep h_lit = new hNode((apval->getInt()).toString(10),
				     hNode::hdlopsEnum::hLiteral);
	    hNodep h_varinit = new hNode(hNode::hdlopsEnum::hVarInit);
	    h_varinit->child_list.push_back(h_lit);
	    (h_info->child_list.back())->child_list.push_back(h_varinit);
	  }
	}
      } else {
	FieldDecl *fieldd = pd->getAsFieldDecl();
	if (fieldd) {
	  LLVM_DEBUG(llvm::dbgs() << "field decl dump follows\n");
	  LLVM_DEBUG(fieldd->dump(llvm::dbgs()));
	  Expr* initializer = fieldd->getInClassInitializer();
	  if (initializer != NULL) {
	    LLVM_DEBUG(llvm::dbgs() << "field initializer dump follows\n");
	    LLVM_DEBUG(initializer->dump(llvm::dbgs()));
	    hNodep h_init = new hNode(hNode::hdlopsEnum::hVarInit);
	    HDLBody xmethod(initializer, h_init, diag_engine);
	    (h_info->child_list.back())->child_list.push_back(h_init);
	  }
	}
      }
    }
  }

  void HDLMain::SCsig2hcode(ModuleInstance::signalMapType pmap,
			    hNode::hdlopsEnum h_op, hNodep &h_info) {
    for (ModuleInstance::signalMapType::iterator mit = pmap.begin();
	 mit != pmap.end(); mit++) {
      string objname = get<0>(*mit);

      LLVM_DEBUG(llvm::dbgs() << "object name is " << objname << "\n");

      SignalDecl *pd = get<1>(*mit);

      Tree<TemplateType> *template_argtp =
        (pd->getTemplateTypes())->getTemplateArgTreePtr();

      int arr_size = pd->getArraySizes().size() > 0
	? pd->getArraySizes()[0].getLimitedValue()
	: 0;
      std::vector<llvm::APInt> array_sizes = pd->getArraySizes();
      HDLt.SCtype2hcode(objname, template_argtp, &array_sizes, h_op,
			h_info);  // passing the sigvarlist
    }
  }

  void HDLMain::SCproc2hcode(ModuleInstance::processMapType pm, hNodep &h_top) {
    // typedef std::map<std::string, ProcessDecl *> processMapType;
    // processMapType getProcessMap();
    // ProcessDecl::getEntryFunction() returns EntryFunctionContainer*

    /// Get the diagnostic engine.
    //
    clang::DiagnosticsEngine &diag_engine{getContext().getDiagnostics()};

    const unsigned cxx_record_id1 = diag_engine.getCustomDiagID(
								clang::DiagnosticsEngine::Remark, "non-SC_METHOD '%0' skipped.");

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
	    // typedef std::map<std::string, std::vector<SensitivityTupleType>>
	    // SenseMapType; typedef std::tuple<std::string, ValueDecl *,
	    // MemberExpr *, DeclRefExpr*>SensitivityTupleType;

	    if (sensmapitem.first == "dont_initialize")  // nonsynthesizable
	      continue;

	    LLVM_DEBUG(llvm::dbgs()
		       << "sensmap item " << sensmapitem.first << "\n");
	    size_t found = sensmapitem.first.find("_handle");
	    // LLVM_DEBUG(llvm::dbgs() << "first part is " <<
	    // sensmapitem.first.substr(0, found) << "\n");
	    if ((found == std::string::npos) ||
		(sensmapitem.first.substr(0, found).compare(efc->getName()) !=
		 0)) {
	      // LLVM_DEBUG(llvm::dbgs() << "compare not equal " <<
	      // sensmapitem.first.substr(0, found) << " " << efc->getName() <<
	      // "\n");
	      continue;  // sensitivity item is not for this process
	    }
	    std::vector<EntryFunctionContainer::SensitivityTupleType> sttv =
              sensmapitem.second;
	    EntryFunctionContainer::SensitivityTupleType sensitem0 = sttv[0];
	    string sensitm = get<1>(sensitem0)->getNameAsString();
	    LLVM_DEBUG(llvm::dbgs() << "sens item " << sensitm << "\n");
	    // LLVM_DEBUG(llvm::dbgs() << " declref follows\n");
	    // LLVM_DEBUG(get<3>(sensitem0)->dump(llvm::dbgs()));
	    hNode::hdlopsEnum h_op;
	    if ((sensitm.compare("pos") == 0) || (sensitm.compare("neg") == 0)) {
	      h_op = hNode::hdlopsEnum::hSensedge;
	    } else {
	      h_op = hNode::hdlopsEnum::hSensvar;
	    }
	    hNodep h_firstfield = new hNode(sensitm, h_op);
	    for (int i = 1; i < sttv.size(); i++) {
	      sensitm = get<1>(sttv[i])->getNameAsString();
	      if ((sensitm.compare("pos") == 0) ||
		  (sensitm.compare("neg") == 0)) {
		h_op = hNode::hdlopsEnum::hSensedge;
	      } else {
		h_op = hNode::hdlopsEnum::hSensvar;
	      }
	      h_firstfield->child_list.push_back(new hNode(sensitm, h_op));
	    }
	    h_senslist->child_list.push_back(h_firstfield);
	  }
	  h_process->child_list.push_back(h_senslist);
	}
	CXXMethodDecl *emd = efc->getEntryMethod();
	if (emd->hasBody()) {
	  hNodep h_body = new hNode(hNode::hdlopsEnum::hMethod);
	  HDLBody xmethod(emd, h_body, diag_engine);
	  allmethodecls.insert(xmethod.methodecls.begin(),
			       xmethod.methodecls.end());
	  h_process->child_list.push_back(h_body);
	  h_top->child_list.push_back(h_process);
	} else {
	  LLVM_DEBUG(llvm::dbgs() << "Entry Method is null\n");
	}
      } else {
	clang::DiagnosticBuilder diag_builder{diag_engine.Report(
								 (efc->getEntryMethod())->getLocation(), cxx_record_id1)};
	diag_builder << efc->getName();

	LLVM_DEBUG(llvm::dbgs() << "process " << efc->getName()
		   << " not SC_METHOD, skipping\n");
      }
    }
  }
}
