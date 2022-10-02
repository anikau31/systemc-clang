// clang-format off
#include <regex>
#include <tuple>
#include <unordered_map>
#include "SystemCClang.h"
#include "PortBinding.h"
#include "ArrayTypeUtils.h"
#include "Tree.h"
#include "HDLMain.h"
//#include "TemplateParametersMatcher.h"
//#include "SensitivityMatcher.h"
#include "clang/Basic/FileManager.h"
#include "llvm/Support/Debug.h"
#include "clang/Basic/Diagnostic.h"

#include "HDLHnode.h"
#include "HDLThread.h"

#include <iostream>

#include "APIntUtils.h"
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

    /// File name passed from command line argument.
    //

    LLVM_DEBUG(llvm::dbgs() << "HDL-FILE-OUTPUT: " << hdl_file_out_ << "\n"; );

    FileID fileID = getSourceManager().getMainFileID();
    const FileEntry *fileentry = getSourceManager().getFileEntryForID(fileID);
    if (hdl_file_out_ == "") {
      if (!fileentry) {
	outputfn = "HCodeout";
	LLVM_DEBUG(llvm::dbgs()
		   << "Null file entry for tranlation unit for this astcontext\n");
      } else {
	outputfn = fileentry->getName().str();
	regex r("\\.cpp");
	outputfn = regex_replace(outputfn, r, "_hdl");

	LLVM_DEBUG(llvm::dbgs() << "File name is " << outputfn << "\n");
      }
    } else {
      outputfn = hdl_file_out_;
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

    //string modname = modinstance->getName()+mod_newn.newname(); // include original name for readability

    hNodep h_module = new hNode(hNode::hdlopsEnum::hModule);
    
    // need to provide an old name for module instance decl
    // since it's not derived from clang NamedDecl and doesn't have
    // a str() class to dereference llvm:StringRef
    
    mod_name_map.add_entry(modinstance, modinstance->getName(), h_module);
    LLVM_DEBUG(llvm::dbgs() << "\ntop level module " << modinstance->getName()
	       << " renamed " << h_module->getname() << "\n");

    //mod_name_map[modinstance] = {modinstance->getName(),
    //				 modname, h_module};
    
    SCmodule2hcode(modinstance, h_module, HCodeOut);
    // h_module->print(HCodeOut);

    LLVM_DEBUG(llvm::dbgs() << "User Types Map\n");

    while (!HDLt.usertype_info.usertypes.empty()) {
     HDLType::usertype_map_t usertypestmp = HDLt.usertype_info.usertypes;
     HDLType::userrectype_map_t userrecmaptmp = HDLt.usertype_info.userrectypes;
      HDLt.usertype_info.usertypes.clear();
      HDLt.usertype_info.userrectypes.clear();
      for (auto t : usertypestmp) {
	LLVM_DEBUG(llvm::dbgs()
		   << "User Type --------\n"
		   << t.first << ":" << t.second.getTypePtr() << t.second.getAsString() <<"\n");
	LLVM_DEBUG(t.second->dump(llvm::dbgs(), getContext()));
	LLVM_DEBUG(llvm::dbgs() << "---------\n");
	HDLt.addtype(t.first, t.second, getContext())->print(HCodeOut);
      }
    }
    return true;
  }

  void HDLMain::SCmodule2hcode(ModuleInstance *mod, hNodep &h_module,
			       llvm::raw_fd_ostream &HCodeOut) {
    const std::vector<ModuleInstance *> &submodv = mod->getNestedModuleInstances();
    const std::vector<ModuleInstance *> &basemods = mod->getBaseInstances();

    //!
    //! map of overridden method decl and overriding method decl
    //! warning: only supporting single inheritance
    //!
    overridden_method_map_t overridden_method_map;

    LLVM_DEBUG( llvm::dbgs() << "Processing module " << mod->getName() << " instance " << mod->getInstanceName() << "\n");
    LLVM_DEBUG( llvm::dbgs() << "dumping base instances \n");
    LLVM_DEBUG(mod->dump_base_instances(llvm::dbgs()));
    LLVM_DEBUG( llvm::dbgs() << "end base instances \n");

    const CXXRecordDecl *cdecl{mod->getModuleClassDecl()};
    LLVM_DEBUG( llvm::dbgs() << "Methods in this module\n");
    for (const auto &method : cdecl->methods()) {
      if (isValidMethod(method)) {
	if (method->isVirtual()) {
	  LLVM_DEBUG( llvm::dbgs() <<  "Virtual ");
	}
	LLVM_DEBUG(llvm::dbgs() << "Method name is " << method->getParent()->getNameAsString() << "::" << method->getNameAsString()
		   << "\n");
	QualType qtype{method->getThisType()};
	LLVM_DEBUG(qtype.getTypePtr()->dump());
	LLVM_DEBUG(llvm::dbgs() << "\n");
	if (method->getBody() != NULL) {
	  LLVM_DEBUG(llvm::dbgs() << "Body of method non-null\n");
	  //LLVM_DEBUG(method->getBody()->dump());
	}
	else LLVM_DEBUG(llvm::dbgs() << "Empty method body\n");
      
	for (const auto &ometh : method->overridden_methods()) {
	  LLVM_DEBUG(llvm::dbgs() << " overridden method " << ometh->getParent()->getNameAsString() << "::" << ometh->getNameAsString() << "\n");
	  if (ometh->hasBody()) {
	    overridden_method_map[ometh] = method; 
	  }
	  else LLVM_DEBUG(llvm::dbgs() << "Empty overridden method body\n");
	}
      }
    }

    LLVM_DEBUG(llvm::dbgs() <<"Overridden method map\n");
    for (auto ov: overridden_method_map) {
      LLVM_DEBUG(llvm::dbgs() << "Overridden method\n");
      LLVM_DEBUG(ov.first->dump(llvm::dbgs()));
      LLVM_DEBUG(llvm::dbgs() << "Overriding method\n");
      LLVM_DEBUG(ov.second->dump(llvm::dbgs()));
    }
    LLVM_DEBUG(llvm::dbgs() <<"end Overridden method map\n");
    LLVM_DEBUG( llvm::dbgs() << "End Methods in this module\n\n");
      
    // look at constructor

    // LLVM_DEBUG(llvm::dbgs() << "dumping module constructor stmt\n");

    // LLVM_DEBUG(mod->getConstructorStmt()->dump(llvm::dbgs()));
    //LLVM_DEBUG( llvm::dbgs() << "dumping module constructor decl body\n");
    //LLVM_DEBUG(mod->getConstructorDecl()->getBody()->dump());
    //LLVM_DEBUG( llvm::dbgs() << "end dumping module constructor decl body\n");
    //LLVM_DEBUG(mod->getConstructorDecl()->dump(llvm::dbgs()));

    LLVM_DEBUG(llvm::dbgs() << "submodule count is " << submodv.size() << "\n");
    
    hdecl_name_map_t mod_vname_map("_scclang_global_");
    xbodyp = new HDLBody(main_diag_engine, getContext(), mod_vname_map, allmethodecls, overridden_method_map);
    
    module_vars.clear();
    threadresetmap.clear();
    
    // Ports
    hNodep h_ports =
      new hNode(hNode::hdlopsEnum::hPortsigvarlist);  // list of ports, signals
    h_module->child_list.push_back(h_ports);

    // generate port, sig, var for module inheritance chain
    ModuleInstance *mod_i = mod;
    for (int i = 0; i <= basemods.size(); i++) {
      SCport2hcode(mod_i->getIPorts(), hNode::hdlopsEnum::hPortin, h_ports, mod_vname_map);
      SCport2hcode(mod_i->getInputStreamPorts(), hNode::hdlopsEnum::hPortin, h_ports, mod_vname_map);
      SCport2hcode(mod_i->getOPorts(), hNode::hdlopsEnum::hPortout, h_ports, mod_vname_map);
      SCport2hcode(mod_i->getOutputStreamPorts(), hNode::hdlopsEnum::hPortout,
		   h_ports, mod_vname_map);
      SCport2hcode(mod_i->getIOPorts(), hNode::hdlopsEnum::hPortio, h_ports, mod_vname_map);
      
      // Signals
      SCsig2hcode(mod_i->getSignals(), hNode::hdlopsEnum::hSigdecl, h_ports, mod_vname_map);
      
      SCport2hcode(mod_i->getOtherVars(), hNode::hdlopsEnum::hVardecl, h_ports, mod_vname_map);
      if (i == basemods.size()) break;
      mod_i = basemods[i];
    }

    // add the submodule declarations
   
    for (const auto &smod : submodv) {
      std::vector<std::string> instnames;
      if (smod->getInstanceInfo().isArrayType()) {
	LLVM_DEBUG(llvm::dbgs() << "Array submodule " << smod->getInstanceInfo().getVarName() << "\n");
      }
      else {
	LLVM_DEBUG(llvm::dbgs() << "Non-Array submodule " << smod->getInstanceInfo().getVarName() << "\n");
      }

      // we generate instance names based on the array indices so that the names match
      // names used in the portbindings for each instance, which are generated in
      // for loops (see HDLHNode.cpp code to unroll portbindings).
      
      GenerateInstanceNames(smod, instnames);
      bool frsttime = true;
      for (auto instname: instnames) {
	LLVM_DEBUG(llvm::dbgs() << "Instance " << instname << "\n");

	hNodep h_smod =
	  new hNode(instname, hNode::hdlopsEnum::hModdecl);
	h_ports->child_list.push_back(h_smod);
	hNodep h_smodtypinfo = new hNode(hNode::hdlopsEnum::hTypeinfo);
	if (frsttime) { // only enter the first one into the map
	  mod_name_map.add_entry(smod, smod->getName(), h_smod);
	  h_smod->set(instname); // override name inserted by map service
	  frsttime = false;
	}
	h_smodtypinfo->child_list.push_back(
					    new hNode(mod_name_map.find_entry_newn(smod), hNode::hdlopsEnum::hType));
	h_smod->child_list.push_back(h_smodtypinfo);
      }
    }

    // init block
    mod_i = mod;
    hNodep h_modinitblockhead = new hNode( hNode::hdlopsEnum::hNoop); // hold list of module constructors
    hNodep h_constructor;
    hNodep h_allsenslists = new hNode( hNode::hdlopsEnum::hNoop);
    for (int i = 0; i <= basemods.size(); i++) {
      if (mod_i->getConstructorDecl() ==NULL) continue; // null constructor
      h_constructor = new hNode(mod_i->getInstanceInfo().getVarName()+ (mod_i->getInstanceInfo().isArrayType()? "_0" :""),
				hNode::hdlopsEnum::hModinitblock);
    
      xbodyp->Run(mod_i->getConstructorDecl()->getBody(), h_constructor,rmodinit);
      LLVM_DEBUG(llvm::dbgs() << "HDL output for module constructor body\n");
      LLVM_DEBUG(h_constructor->print(llvm::dbgs()));
      HDLConstructorHcode hcxxbody;
      hNodep modinithp = hcxxbody.ProcessCXXConstructorHcode(h_constructor);
      if (modinithp->child_list.size() != 0) { // if there was an initblock
	h_modinitblockhead->child_list.push_back(modinithp);
	// need to add these nodes to h_senshead
	std::vector<hNodep> slvec;
	hcxxbody.GetSensLists(slvec);
	h_allsenslists->child_list.insert(h_allsenslists->child_list.end(), slvec.begin(), slvec.end());
      }
      
      //h_constructor->print(HCodeOut);
      if (i == basemods.size()) break;
      mod_i = basemods[i];
    }

    //LLVM_DEBUG(llvm::dbgs() << "Module sensitivity lists follow\n");
    //LLVM_DEBUG(h_allsenslists->print(llvm::dbgs()));
    //LLVM_DEBUG(llvm::dbgs() << "Module sensitivity lists end\n");

    // build map of thread name to reset var name for this module
    MakeResetMap(threadresetmap, h_allsenslists);

    LLVM_DEBUG(llvm::dbgs() << "Module vname map size is " << mod_vname_map.size() << " \n");

    // Processes
    hNodep h_processes = new hNode(hNode::hdlopsEnum::hProcesses);
    mod_i = mod;
    for (int i = 0; i <= basemods.size(); i++) {
      // send portsigvarlist (h_ports) to proc code gen so thread vars get promoted to module level
      SCproc2hcode(mod_i->getProcessMap(), h_processes, h_ports, mod_vname_map, overridden_method_map, threadresetmap);
      if (i == basemods.size()) break;
      mod_i = basemods[i];
    }
    // add all the processes (including those in the inheritance chain) to the module
    if (!h_processes->child_list.empty()) h_module->child_list.push_back(h_processes);

    // add extra sig and var decls to shadow those referenced in threads
    for (auto const &var: mod_vname_map) {
      if (var.second.referenced) {
	hNodep hvp = new hNode("_main_"+var.second.h_vardeclp->getname(), var.second.h_vardeclp->getopc());
	hvp->child_list = var.second.h_vardeclp->child_list;
	h_ports->append(hvp);
      }
    }
  
    
    // now add init block
    if (h_modinitblockhead->size()>0) {
      h_module->append(h_modinitblockhead->child_list[0]);
      //h_module->child_list.insert(h_module->child_list.end(), h_modinitblockhead->child_list.begin(), h_modinitblockhead->child_list.end());
      hNodep hfirstblock = h_modinitblockhead->child_list[0];
      for (int i = 1; i< h_modinitblockhead->size(); i++) { // in case of multiple modinit blocks due to inheritance
	// join all their child_lists under the first mod_int
	hfirstblock->child_list.insert(hfirstblock->child_list.end(),
				       h_modinitblockhead->child_list[i]->child_list.begin(),
				       h_modinitblockhead->child_list[i]->child_list.end());
	
      }
    }

    // Functions
    // Initially these are functions that were referenced in the module's sc_methods/threads
    // Function calls within functions get added to all methodecls.
    
    std::set<Decl *> generated_functions;
    while (allmethodecls.size() > 0) {
      LLVM_DEBUG(llvm::dbgs() << "Module Method/Function Map\n");
      //std::unordered_multimap<string, FunctionDecl *> modmethodecls;
      hfunc_name_map_t modmethodecls;
      modmethodecls =
        std::move(allmethodecls);  // procedures/functions found in this module
      LLVM_DEBUG(llvm::dbgs()
		 << "size of allmethodecls is " << allmethodecls.size() << "\n");
      LLVM_DEBUG(llvm::dbgs()
		 << "size of modmethodecls is " << modmethodecls.size() << "\n");
      LLVM_DEBUG(modmethodecls.print(llvm::dbgs()));
      LLVM_DEBUG(HDLt.print(llvm::dbgs()));
      for (auto const &m : modmethodecls) {
	LLVM_DEBUG(llvm::dbgs() << "Method --------\n"
		   << m.second.newn << "\n");
	LLVM_DEBUG(m.first->dump(llvm::dbgs()));
	LLVM_DEBUG(llvm::dbgs() << "---------\n");
	//clang::DiagnosticsEngine &diag_engine{getContext().getDiagnostics()};
	if (m.first->hasBody()) {
	  if (generated_functions.count(m.first) > 0) continue; // already generated this one
	  generated_functions.insert(m.first);
	  hNodep hfunc = new hNode(m.second.newn, hNode::hdlopsEnum::hFunction);
	  QualType qrettype = m.first->getReturnType(); // m.first->getDeclaredReturnType();
	  const clang::Type *rettype = qrettype.getTypePtr();
	  FindTemplateTypes *te = new FindTemplateTypes();
	  te->Enumerate(rettype);
	  HDLType HDLt2;
	  // what about returning an array type? this isn't handled 
	  HDLt2.SCtype2hcode("", te->getTemplateArgTreePtr(), NULL,
			    hNode::hdlopsEnum::hFunctionRetType, hfunc);
	  CXXMethodDecl * thismethod = dyn_cast<CXXMethodDecl>(m.first);
	  bool isUserDefinedMethod = (thismethod != NULL) && (modmethodecls.methodobjtypemap.count(thismethod));
	  if (thismethod != NULL) {
	    LLVM_DEBUG(llvm::dbgs() << thismethod->getParent()->getQualifiedNameAsString() << " " << m.second.newn << " is a Method\n");
	  }
	  else LLVM_DEBUG(llvm::dbgs() << m.second.newn << " is a Function\n");
	  if ((m.first->getNumParams() > 0) || (thismethod != NULL)) {
	    hNodep hparams = new hNode(hNode::hdlopsEnum::hFunctionParams);
	    hNodep hparam_assign_list = new hNode(hNode::hdlopsEnum::hCStmt);
	    hfunc->child_list.push_back(hparams);

	    if (isUserDefinedMethod) { // user defined non scmodule method
	      hNodep hthisparam = new hNode("hthis", hNode::hdlopsEnum::hFunctionParamIO);
	      hNodep hthistype = new hNode(hNode::hdlopsEnum::hTypeinfo);
	      const clang::Type * tp = modmethodecls.methodobjtypemap[thismethod];
	      if (tp == NULL) {
		LLVM_DEBUG(llvm::dbgs() <<"Couldn't find methodobjtypemap entry for "  << thismethod << "\n");
	      }
	      else {
		if (HDLt.usertype_info.userrectypes.count(tp)) {
		  LLVM_DEBUG(llvm::dbgs() << "Found methodobjtypemap entry for " << thismethod << " and userrectypes gives " << HDLt.usertype_info.userrectypes[tp] << "\n");
		}
		else {
		  LLVM_DEBUG(llvm::dbgs() << "Couldn't find userrectypes entry for " << tp << "\n");
		  LLVM_DEBUG(HDLt.print(llvm::dbgs()));
		  }
		}
	      hthistype->append(new hNode(HDLt.usertype_info.userrectypes[tp],
					  hNode::hdlopsEnum::hType));
	      hthisparam->append(hthistype);
	      hparams->append(hthisparam);
	    }
	    for (int i = 0; i < m.first->getNumParams(); i++) {
	      ParmVarDecl *vardecl = m.first->getParamDecl(i);
	      QualType q = vardecl->getType();
	      const clang::Type *tp = q.getTypePtr();
	      LLVM_DEBUG(llvm::dbgs() << "ProcessParmVarDecl type name is "
			 << q.getAsString() << "\n");
	      FindTemplateTypes *te = new FindTemplateTypes();
	      te->Enumerate(tp);
	      HDLType HDLt1;
	      std::vector<llvm::APInt> array_sizes = sc_ast_matchers::utils::array_type::getConstantArraySizes(vardecl);
	      hNode::hdlopsEnum paramtype;
	      // special case if sc_min, max, abs, treat parameters as input
	      // unfortunately simulation library makes them I/O
	      //if (mutil.is_sc_macro(m.first)) paramtype = hNode::hdlopsEnum::hFunctionParamI;
	      
        // ============= CHECK ==============
	      bool t1 = mutil.isSCByFunctionDecl(m.first);
	      bool t2 = mutil.isSCMacro(m.second.oldn);

	      if (t1 != t2) {
		llvm::dbgs() << "@@@@ isSCMacro does not match.  t1 = " << t1 << ", t2 = " << t2 << "  " << m.second.oldn << "\n";
		assert(0 && "isSCMacro does not match");
	      }
	      // ============= END CHECK ==============
	      //
	      if (mutil.isSCMacro(m.second.oldn)) {
		paramtype = hNode::hdlopsEnum::hFunctionParamI;
	      }
	      else if ((vardecl->getType()->isReferenceType()) && !(vardecl->getType().getNonReferenceType().isConstQualified()))
		paramtype = hNode::hdlopsEnum::hFunctionParamIO;
	      else { // handle actual parameter
		
		paramtype = hNode::hdlopsEnum::hFunctionParamI;
		// create an entry in mod_vname_map for this parameter's local variable
		string objname = vardecl->getName().str()+"_actual";

		HDLt1.SCtype2hcode(objname, te->getTemplateArgTreePtr(),
				&array_sizes, hNode::hdlopsEnum::hVardecl, h_ports);
		mod_vname_map.add_entry(vardecl, objname, h_ports->child_list.back());
		hNodep hparam_assign = new hNode("=", hNode::hdlopsEnum::hBinop);
		hNodep hv = new hNode(mod_vname_map.find_entry_newn(vardecl), hNode::hdlopsEnum::hVarref);
		hparam_assign->append(hv);
		hv = new hNode(vardecl->getName().str(), hNode::hdlopsEnum::hVarref);
		hparam_assign->append(hv);
		hparam_assign_list->append(hparam_assign);
	      }
	      
	      HDLt1.SCtype2hcode(vardecl->getName().str(), te->getTemplateArgTreePtr(),
				&array_sizes, paramtype, hparams);
	    }
	    
	    if (hparam_assign_list->child_list.size()>0) { // there were some actual parameters
	      hNodep htmpf = new hNode( hNode::hdlopsEnum::hCStmt);
	      if (isUserDefinedMethod) {
		xbodyp->Run(m.first->getBody(), htmpf, ruserdefclass, &HDLt); // suppress output of unqualified name
	      }
	      else {
		xbodyp->Run(m.first->getBody(), htmpf,rnomode);
	      }
	      
	      hNodep hfunccstmt = htmpf->child_list.back();  // htmpf is list of vardecls followed by function body in a cstmt
	      hfunccstmt->child_list.insert(hfunccstmt->child_list.begin(), hparam_assign_list->child_list.begin(), hparam_assign_list->child_list.end());
	      
	      hfunc->child_list.insert(hfunc->child_list.end(), htmpf->child_list.begin(), htmpf->child_list.end());

	    }
	    else {
	      if (isUserDefinedMethod) {
		xbodyp->Run(m.first->getBody(), hfunc, ruserdefclass, &HDLt); // suppress output of unqualified name
	      }
	      else {
		xbodyp->Run(m.first->getBody(), hfunc,rnomode);
	      }
	    }
	  }
	  // If this function invoked other functions, add them to the list to be generated
	  allmethodecls.insertall(xbodyp->methodecls);
	  h_processes->child_list.push_back(hfunc);
	  // LLVM_DEBUG(m.second->dump(llvm::dbgs()));
	} // end non-null body
      }
    }

    h_module->print(HCodeOut);
    // now generate submodules
    delete xbodyp; // release this hdlbody
    
    for (const auto &smod : submodv) {

      string modname = mod_name_map.find_entry_newn(smod);
      LLVM_DEBUG(llvm::dbgs() << "generate submodule " << smod->getName() 
		 << " renamed " << modname << "\n");
      hNodep h_submod = new hNode(modname, hNode::hdlopsEnum::hModule);
      SCmodule2hcode(smod, h_submod, HCodeOut);
      // }
    }
  }

  void HDLMain::GenerateInstanceNames(ModuleInstance *smod, std::vector<std::string> &instnames) {
    string basevarname = smod->getInstanceInfo().getVarName();
    std::vector<llvm::APInt> arraysizes  = smod->getInstanceInfo().getArraySizes(); 
    //instnames = smod->getInstanceInfo().getInstanceNames();
    int ndim = smod->getInstanceInfo().getArrayDimension();

    if (ndim==0) {
      instnames.push_back(basevarname);
      return;
    }
    
    // convert the annoying APInt datatype
    int array_dim[ndim];
    for (int i = 0; i<ndim; i++) {
      array_dim[i] = arraysizes[i].getSExtValue();
    }

    // in order of likelihood
    // only handle up to 3D (front end restriction)
    if (ndim==1) {
      for (int i = 0; i < array_dim[0]; i++) {
	string varname = basevarname;
	varname.append("_" + to_string(i));
	instnames.push_back(varname);
      }
      return;
    }

    if (ndim == 2) {
      for (int i = 0; i < array_dim[0]; i++)
	for (int j = 0; j < array_dim[1]; j++) {
	string varname = basevarname;
	varname.append("_" + to_string(i)+"_" + to_string(j));
	instnames.push_back(varname);
	}
      return;
    }
    
    for (int i = 0; i <= array_dim[0]; i++) 
      for (int j = 0; j < array_dim[1]; j++) 
	for (int k = 0; k < array_dim[2]; k++) {
	  string varname = basevarname;
	  varname.append("_" + to_string(1)+"_" + to_string(j-1)+"_" + to_string(k-1));
	  instnames.push_back(varname);
	}
  }

  bool HDLMain::isValidMethod(CXXMethodDecl *method) {
    if ((method->getNameAsString() != (method->getParent()->getNameAsString() ))  && // constructor
	(method->getNameAsString() != "~"+ (method->getParent()->getNameAsString() )) && // destructor
	  (method->getBody() !=NULL)) // get rid of methods with empty body
      return true;
    else return false;
  }
  
  void HDLMain::SCport2hcode(ModuleInstance::portMapType pmap, hNode::hdlopsEnum h_op,
			     hNodep &h_info, hdecl_name_map_t &mod_vname_map) {
    //clang::DiagnosticsEngine &diag_engine{getContext().getDiagnostics()};

    const unsigned cxx_record_id1 = main_diag_engine.getCustomDiagID(clang::DiagnosticsEngine::Remark, "Pointer type not synthesized, '%0' skipped.");
    for (ModuleInstance::portMapType::iterator mit = pmap.begin(); mit != pmap.end();
	 mit++) {
      string objname = get<0>(*mit);

      LLVM_DEBUG(llvm::dbgs() << "object name is " << objname << " and h_op is "
		 << h_op << "\n");

      PortDecl *pd = get<1>(*mit);
      if (pd->isPointerType()) {
        NamedDecl * decl = pd->getAsVarDecl();
	if (decl == NULL) decl = pd->getAsFieldDecl();
	if (decl !=NULL) {
	  clang::DiagnosticBuilder diag_builder{main_diag_engine.Report(decl->getLocation(), cxx_record_id1)};
	  diag_builder << decl->getName();
	  return;
	}
	return;
      }
	  
      Tree<TemplateType> *template_argtp =
        (pd->getTemplateType())->getTemplateArgTreePtr();

      std::vector<llvm::APInt> array_sizes = pd->getArraySizes();

      HDLt.SCtype2hcode(objname, template_argtp, &array_sizes, h_op,
			h_info);  // passing the sigvarlist

      // if this is a duplicate name due to inheritance
      // create a new name and add it to the module level vname map
      // this map will be passed to all calls to HDLBody to merge into
      // its vname_map

      NamedDecl * portdecl = pd->getAsVarDecl();
      if (!portdecl)
	portdecl = pd->getAsFieldDecl();
      if (module_vars.count(objname)) {
	LLVM_DEBUG(llvm::dbgs() << "duplicate object " << objname << "\n");
	if (portdecl) mod_vname_map.add_entry(portdecl, objname, h_info->child_list.back());
      }
      else {
	module_vars.insert(objname);
	// don't make new names for ports, will break logic in the modinit hcode processing
	if ((h_op == hNode::hdlopsEnum::hVardecl) && (portdecl)) mod_vname_map.add_entry(portdecl, objname, h_info->child_list.back());
      }

      // check for initializer
      if (h_op == hNode::hdlopsEnum::hVardecl) {
	VarDecl *vard = pd->getAsVarDecl();
	if (vard) {
	  LLVM_DEBUG(llvm::dbgs() << "var decl dump follows\n");
	  LLVM_DEBUG(vard->dump(llvm::dbgs()));
	  if (vard->hasInit()) {
	    APValue *apval = vard->getEvaluatedValue();
	    if (apval && apval->isInt()) {
	      hNodep h_lit = new hNode((systemc_clang::utils::apint::toString(apval->getInt())),
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
	      LLVM_DEBUG(initializer->dump(llvm::dbgs(), getContext()));
	      hNodep h_init = new hNode(hNode::hdlopsEnum::hVarInit);	   
	      if (const CXXConstructExpr *ce = dyn_cast<CXXConstructExpr>(initializer->IgnoreUnlessSpelledInSource())) {
		if (ce->isListInitialization()) {
		  for (const auto arg : ce->arguments()) {
		    const Expr *ex{arg->IgnoreUnlessSpelledInSource()};
		    
		    if (auto il = dyn_cast<IntegerLiteral>(ex)) {
		      llvm::dbgs()  << ">> IntegerLiteral value is " << il->getValue() << "\n";
		      h_init->append(new hNode(systemc_clang::utils::apint::toString(il->getValue()), hNode::hdlopsEnum::hLiteral));
		    }
		    
		    if (auto booll = dyn_cast<CXXBoolLiteralExpr>(ex)) {
		      bool val =  booll->getValue();
		      llvm::dbgs() << ">> CXXBoolLiteralExpr value is "  << val << "\n";
		      h_init->append(new hNode(to_string(val), hNode::hdlopsEnum::hLiteral));
		      (h_info->child_list.back())->child_list.push_back(h_init);
		    }
		  }
		}
	      }
	      else {
		xbodyp->Run(initializer, h_init, rnomode);
		(h_info->child_list.back())->child_list.push_back(h_init);
	      }
	    }
	  }
	}
      }
    }
  }

  void HDLMain::SCsig2hcode(ModuleInstance::signalMapType pmap,
			    hNode::hdlopsEnum h_op, hNodep &h_info, hdecl_name_map_t &mod_vname_map) {

    const unsigned cxx_record_id1 = main_diag_engine.getCustomDiagID(clang::DiagnosticsEngine::Remark, "Pointer type not synthesized, '%0' skipped.");
    for (ModuleInstance::signalMapType::iterator mit = pmap.begin();
	 mit != pmap.end(); mit++) {
      string objname = get<0>(*mit);

      // Unfortunately due to portdecl and sigdecl having incompatible data structures,
      // the same code has to be repeated in both.
      LLVM_DEBUG(llvm::dbgs() << "object name is " << objname << "\n");

      SignalDecl *pd = get<1>(*mit);

      if (pd->isPointerType()) {
        NamedDecl * decl = pd->getAsVarDecl();
	if (decl == NULL) decl = pd->getAsFieldDecl();
	if (decl !=NULL) {
	  clang::DiagnosticBuilder diag_builder{main_diag_engine.Report(decl->getLocation(), cxx_record_id1)};
	  diag_builder << decl->getName();
	  return;
	}
	return;
      }
      
      Tree<TemplateType> *template_argtp =
        (pd->getTemplateTypes())->getTemplateArgTreePtr();

      int arr_size = pd->getArraySizes().size() > 0
	? pd->getArraySizes()[0].getLimitedValue()
	: 0;
      std::vector<llvm::APInt> array_sizes = pd->getArraySizes();
      HDLt.SCtype2hcode(objname, template_argtp, &array_sizes, h_op,
			h_info);  // passing the sigvarlist

      // if this is a duplicate name due to inheritance
      // create a new name and add it to the module level vname map
      // this map will be passed to all calls to HDLBody to merge into
      // its vname_map

      NamedDecl * portdecl = pd->getAsVarDecl();
      if (!portdecl)
	portdecl = pd->getAsFieldDecl();
      // ValueDecl * vd = (ValueDecl *)portdecl;
      // LLVM_DEBUG(llvm::dbgs() << "Sig type is " << vd->getType().getAsString() << "\n");
      if (module_vars.count(objname)) {
	LLVM_DEBUG(llvm::dbgs() << "duplicate object " << objname << "\n");
	if (portdecl)
	  mod_vname_map.add_entry(portdecl, objname, h_info->child_list.back());
	//string newn = mod_newn.newname();
	//objname+="_var"+newn;
      }
      else {
	module_vars.insert(objname);
	if (portdecl) mod_vname_map.add_entry(portdecl, objname, h_info->child_list.back());
      }
    }
  }

  void HDLMain::SCproc2hcode(ModuleInstance::processMapType pm, hNodep &h_top, hNodep &h_port,
			     hdecl_name_map_t &mod_vname_map, overridden_method_map_t &overridden_method_map, resetvar_map_t &threadresetmap) {
    // typedef std::map<std::string, ProcessDecl *> processMapType;
    // processMapType getProcessMap();
    // ProcessDecl::getEntryFunction() returns EntryFunctionContainer*

    /// Get the diagnostic engine.
    //
    //clang::DiagnosticsEngine &diag_engine{getContext().getDiagnostics()};

    const unsigned cxx_record_id1 = main_diag_engine.getCustomDiagID(
								     clang::DiagnosticsEngine::Remark, "non-SC_METHOD/THREAD '%0' skipped.");

    for (auto const &pm_entry : pm) {
      ProcessDecl *pd{get<1>(pm_entry)};
      EntryFunctionContainer *efc{pd->getEntryFunction()};
      if (efc->getProcessType() == PROCESS_TYPE::METHOD) {
	hNodep h_process = new hNode(efc->getName(), hNode::hdlopsEnum::hProcess);
	LLVM_DEBUG(llvm::dbgs() << "process " << efc->getName() << "\n");
	CXXMethodDecl *emd = efc->getEntryMethod();
	if (emd->hasBody()) {
	  hNodep h_body = new hNode(efc->getName(), hNode::hdlopsEnum::hMethod);
	  LLVM_DEBUG(llvm::dbgs() << "HDLMain allmethodecls_ size is " << allmethodecls.size() << "\n");
	  //HDLBody xmethod(emd, h_body, main_diag_engine, getContext(), mod_vname_map);
	  xbodyp->Run(emd->getBody(), h_body, rmethod);
	  allmethodecls.insertall(xbodyp->methodecls);
	  h_process->child_list.push_back(h_body);
	  h_top->child_list.push_back(h_process);
	} else {
	  LLVM_DEBUG(llvm::dbgs() << "Entry Method is null\n");
	}
      } else {
	if ((efc->getProcessType() == PROCESS_TYPE::THREAD) ||
	    (efc->getProcessType() == PROCESS_TYPE::CTHREAD)) {
	  hNodep h_thread = new hNode(efc->getName(), hNode::hdlopsEnum::hProcess);
	  LLVM_DEBUG(llvm::dbgs() << "thread " << efc->getName() << "\n");
	  CXXMethodDecl *emd = efc->getEntryMethod();
	  if (emd->hasBody()) {

	    auto got = threadresetmap.find(efc->getName());
	    // should be an error if there isn't a reset var for this thread
           clang::DiagnosticBuilder diag_builder{main_diag_engine.Report(
                (efc->getEntryMethod())->getLocation(),
                main_diag_engine.getCustomDiagID(
                  clang::DiagnosticsEngine::Remark, "Reset not found in SC_[C]THREAD."))};
           diag_builder << "\n";
	    auto h_resetvarinfo = (got == threadresetmap.end() ? NULL : got->second);

	    // params includes portsigvarlist so thread local vars get promoted to module level
	    // have to pass efc to get the reset info

	    HDLThread xthread(efc, h_thread, h_port, main_diag_engine, getContext(), mod_vname_map, allmethodecls, overridden_method_map, h_resetvarinfo );
	    allmethodecls.insertall(xthread.methodecls);
	    //h_thread->child_list.push_back(h_body);
	    h_top->child_list.push_back(h_thread);
	  } else {
	    LLVM_DEBUG(llvm::dbgs() << "Entry Thread is null\n");
	  }
	}
	else {
	  clang::DiagnosticBuilder diag_builder{main_diag_engine.Report((efc->getEntryMethod())->getLocation(), cxx_record_id1)};
	  diag_builder << efc->getName();

	  LLVM_DEBUG(llvm::dbgs() << "process " << efc->getName()
		     << " not SC_METHOD, THREAD, or CTHREAD, skipping\n");
	}
      }
    }
  }

  void HDLMain::MakeResetMap( resetvar_map_t &threadresetmap, hNodep h_allsenslists)
  {
    // top node is a noop, then child list has the sensitivity lists:
    //hNoop  NONAME [
    //hSenslist mc_proc [
    //hSensvar NONAME [
    //hVarref s_fp##valid NOLIST
    //hNoop always NOLIST
    // ]
    // hSensvar NONAME [
    //   hVarref s_fp##data NOLIST
    //   hNoop always NOLIST
    // ]
    // or
    //hSenslist break_in_for_wait0 [
    //   hSensvar ASYNC [
    //     hVarref arst NOLIST
    //     hLiteral 0 NOLIST
    //   ]
    // ]
    
    if (h_allsenslists != NULL) {	  
      for (hNodep h_onesenslist : h_allsenslists->child_list) {
	string threadname = h_onesenslist->getname();
	for ( hNodep h_sensvar : h_onesenslist->child_list) {
	  if (h_sensvar->getname() == "NONAME") continue; // non-reset var has null hSensvar name
	  threadresetmap[threadname] = h_sensvar;  // only one reset per thread
	  break;
	}
      }
    }
  }
  
  // this is obsolete. It has been supeseded by HDLHnode.cpp
  // due to possibility of for-loops enclosing port bindings
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
	    LLVM_DEBUG(subscriptex->dump(llvm::dbgs(), getContext()));
	  }
	}
      if (binding->getCalleeArraySubscripts().size()>0) {
	LLVM_DEBUG(llvm::dbgs() << "Callee Subscript vector length is " <<
		   binding->getCalleeArraySubscripts().size() << "\n");
	for (auto subscriptex: binding->getCalleeArraySubscripts()) {
	  LLVM_DEBUG(subscriptex->dump(llvm::dbgs(), getContext()));
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
  
}  
