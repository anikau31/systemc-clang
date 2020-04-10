#include <regex>
#include <tuple>
#include "SystemCClang.h"
#include "Xlat.h"
#include "clang/Basic/FileManager.h"

using namespace std;
using namespace hnode;
using namespace scpar;

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
      os_ << "\nmodule " << mit->first << "\n";
      string modname = mit->first + "_" + to_string(i);
      hNodep h_module = new hNode(modname, hNode::hdlopsEnum::hModule);
      //hNodep h_modname = new hNode(modname, hNode::hdlopsEnum::hModule);

      // Ports
      hNodep h_ports = new hNode(hNode::hdlopsEnum::hPortsigvarlist);  // list of ports, signals
      xlatport(instanceVec.at(i)->getIPorts(), hNode::hdlopsEnum::hPortin,
               h_ports);
      xlatport(instanceVec.at(i)->getInputStreamPorts(), hNode::hdlopsEnum::hPortin,
               h_ports);
      xlatport(instanceVec.at(i)->getOPorts(), hNode::hdlopsEnum::hPortout,
               h_ports);
      xlatport(instanceVec.at(i)->getOutputStreamPorts(), hNode::hdlopsEnum::hPortout,
               h_ports);
      xlatport(instanceVec.at(i)->getIOPorts(), hNode::hdlopsEnum::hPortio,
               h_ports);

      // Other Variables
      xlatport(instanceVec.at(i)->getOtherVars(), hNode::hdlopsEnum::hVardecl,
               h_ports);

      // Signals
      xlatsig(instanceVec.at(i)->getSignals(), hNode::hdlopsEnum::hSigdecl,
              h_ports);

      //h_module->child_list.push_back(h_modname);
      h_module->child_list.push_back(h_ports);

      h_top = new hNode(hNode::hdlopsEnum::hProcesses);
      // Processes
      xlatproc(instanceVec.at(i)->getEntryFunctionContainer(), h_top, os_);

      if (!h_top->child_list.empty()) h_module->child_list.push_back(h_top);
      h_module->print(xlatout);
      delete h_top; //h_module;
    }
  }
  os_ << "Global Method Map\n";
  for (auto m : allmethodecls) {
    os_ << "Method --------\n" << m.first << ":" << m.second << "\n";
    m.second->dump(os_);
    os_ << "---------\n";
  }
  return true;
}

void Xlat::xlatport(ModuleDecl::portMapType pmap, hNode::hdlopsEnum h_op,
                    hNodep &h_info) {

  for (ModuleDecl::portMapType::iterator mit = pmap.begin(); mit != pmap.end();
       mit++) {

    string objname = get<0>(*mit);

    os_ << "object name is " << objname << " and h_op is " << h_op << "\n";

    PortDecl *pd = get<1>(*mit);
    Tree<TemplateType> *template_argtp = (pd->getTemplateType())->getTemplateArgTreePtr();
    const Type * typep = (template_argtp->getRoot())->getDataPtr()->getTypePtr();
    xlattype(objname, template_argtp, typep, h_op, h_info);  // passing the sigvarlist
  }
}

void Xlat::xlatsig(ModuleDecl::signalMapType pmap, hNode::hdlopsEnum h_op,
                   hNodep &h_info) {

  for (ModuleDecl::signalMapType::iterator mit = pmap.begin();
       mit != pmap.end(); mit++) {

    string objname = get<0>(*mit);

    os_ << "object name is " << objname << "\n";

    Signal *pd = get<1>(*mit);
    Tree<TemplateType> *template_argtp = (pd->getTemplateTypes())->getTemplateArgTreePtr();
    TreeNode<TemplateType> * rootp = template_argtp->getRoot();
    if (rootp) {
      const Type * typep = rootp->getDataPtr()->getTypePtr();
      xlattype(objname, template_argtp, typep, h_op, h_info);  // passing the sigvarlist
    }
    else os_ << "object's template type root pointer is null, skipping\n";
  }
}

void Xlat::makehpsv(string prefix, string typname, hNode::hdlopsEnum h_op, hNodep &h_info, bool needtypeinfo) {
  // create hNode for a Port|Signal|Var declaration with resolved type name
  os_ << "in makehpsv prefix, type, op " << prefix << " " << typname << " " << h_op << "\n";
  hNodep hmainp = new hNode(prefix, h_op);
  h_info->child_list.push_back(hmainp); // returning [opPort|Sig|Var prefix [Typeinfo [Type typname]]]

  hNodep htypep = new hNode(typname, hNode::hdlopsEnum::hType);
  if (needtypeinfo) {
    hNodep h_typeinfop = new hNode(hNode::hdlopsEnum::hTypeinfo);
    h_typeinfop->child_list.push_back(htypep);
    hmainp->child_list.push_back(h_typeinfop);
  }
  else {
    hmainp->child_list.push_back(htypep);
  }
}
  
void Xlat::xlattype(string prefix,  Tree<TemplateType> *template_argtp, const Type *typep,  hNode::hdlopsEnum h_op, hNodep &h_info) {

  //llvm::outs()  << "xlattype dump of templatetree args follows\n";
  //template_argtp->dump();

  if (const BuiltinType *bt = dyn_cast<BuiltinType>(typep)) {
    string typname = (bt->desugar()).getAsString();
    os_ << "found built in type " << typname << "\n";
    makehpsv(prefix, typname, h_op, h_info);
    return;
  }

  
  // QualType 	getCanonicalTypeInternal () const -- from Type
  // const Type * 	getUnqualifiedDesugaredType () const  -- from Type

  // sc_types: sc_int<>, sc_uint<>, sc_bigint<>, sc_biguint<>, sc_bv<>, sc_logic

  
  // now process composite types

  if (const RecordType * rectype = dyn_cast<RecordType>(typep)) {
    string tmps = rectype->desugar().getAsString();
    os_ << "record type found, name is " << tmps << "\n";
    if (lutil.isSCType(tmps) || lutil.isSCBuiltinType(tmps)) {  // primitive sc type
      lutil.make_ident(tmps);
      makehpsv(prefix,tmps, h_op, h_info);
    }
    else { // not primitive sc type, need to traverse
      os_ << " nonprimitive type " << tmps << "\n";
      for (auto const &fld: rectype->getDecl()->fields()) {
	os_ << "field of record type \n";
	fld ->dump(os_);
	os_ << "found name " << fld->getName() << "\n";
	xlattype(prefix+'_'+fld->getNameAsString(), template_argtp, fld->getType().getTypePtr(), h_op, h_info);
      }
    } // end non primitive sc type
  } // end record type
}

void Xlat::xlatproc(scpar::vector<EntryFunctionContainer *> efv, hNodep &h_top,
                    llvm::raw_ostream &os) {
  for (auto efc : efv) {
    if (efc->getProcessType() == PROCESS_TYPE::METHOD) {
      hNodep h_process = new hNode(efc->getName(), hNode::hdlopsEnum::hProcess);
      os_ << "process " << efc->getName() << "\n";
      // Sensitivity list
      hNodep h_senslist = new hNode(hNode::hdlopsEnum::hSenslist);
      for (auto sensmap : efc->getSenseMap()) {
	if (sensmap.first == "dont_initialize") // nonsynthesizable
	  continue;
	hNodep h_senspair = new hNode(sensmap.first, hNode::hdlopsEnum::hSensvar); // [ sensvar name (edge expr) ]
	//hNodep h_sensitem = new hNode(sensmap.first, hNode::hdlopsEnum::hSensvar);
	
  // HP: There is a change here. 
  // Sensitivity map returns as its second argument a tuple.  
  // The tuple has two parameters: the edge (pos/neg) and the second is the MemberExpr*.
  // 
  //
	//h_senspair->child_list.push_back(h_sensitem);

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
      hNodep h_body; // = new hNode(hNode::hdlopsEnum::hCStmt);
      XlatMethod xmethod(emd, h_body, os_);  //, xlatout);
      os_ << "Method Map:\n";
      for (auto m : xmethod.methodecls) {
	os_ << m.first << ":" << m.second <<"\n";
	//m.second->dump(os_);
      }
      allmethodecls.insert(xmethod.methodecls.begin(), xmethod.methodecls.end());

      h_process->child_list.push_back(h_body);
      h_top->child_list.push_back(h_process);
    } else
      os_ << "process " << efc->getName() << " not SC_METHOD, skipping\n";
  }
}
