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
    hNodep hportp = new hNode(objname, h_op);
    h_info->child_list.push_back(hportp);
    os_ << "object name is " << objname << "\n";
    PortDecl *pd = get<1>(*mit);
    hNodep h_typeinfo = new hNode(hNode::hdlopsEnum::hTypeinfo);
    xlattype(objname, pd->getTemplateType(), h_typeinfo);
    //xlattype(objname, pd->getFieldDecl(), h_typeinfo);
    hportp->child_list.push_back(h_typeinfo);
  }
}

void Xlat::xlatsig(ModuleDecl::signalMapType pmap, hNode::hdlopsEnum h_op,
                   hNodep &h_info) {

  for (ModuleDecl::signalMapType::iterator mit = pmap.begin();
       mit != pmap.end(); mit++) {
    string objname = get<0>(*mit);
    hNodep hsigp = new hNode(objname, h_op);
    h_info->child_list.push_back(hsigp);
    os_ << "object name is " << objname << "\n";

    Signal *pd = get<1>(*mit);
    hNodep h_typeinfo = new hNode(hNode::hdlopsEnum::hTypeinfo);
    xlattype(objname, pd->getTemplateTypes(), h_typeinfo);
    //xlattype(objname, pd->getASTNode(), h_typeinfo);
    hsigp->child_list.push_back(h_typeinfo);
  }
}

/*
for (auto const &port : test_module_inst->getIPorts()) {
      std::string name{get<0>(port)};
      PortDecl *pd{get<1>(port)};
      FindTemplateTypes *template_type{pd->getTemplateType()};
      Tree<TemplateType> *template_args{template_type->getTemplateArgTreePtr()};

      // Print out each argument individually using the iterators.
      //

      // Note: template_args must be dereferenced.
      for (auto const &node : *template_args) {
        auto type_data{node->getDataPtr()};
        llvm::outs() << "\n@> name: " << name
                     << ", type name: " << type_data->getTypeName() << " ";

        // Access the parent of the current node.
        // If the node is a pointer to itself, then the node itself is the
        // parent. Otherwise, it points to the parent node.
        auto parent_node{node->getParent()};
        if (parent_node == node) {
          llvm::outs() << "\n@> parent (itself) type name: " << parent_node->getDataPtr()->getTypeName() << "\n";
        } else {
          // It is a different parent.
          llvm::outs() << "\n@> parent (different) type name: " << parent_node->getDataPtr()->getTypeName() << "\n";
        }

        // Access the children for each parent.
        // We use the template_args to access it.

        auto children{ template_args->getChildren(parent_node)};
        for (auto const &kid: children) {
          llvm::outs() << "@> child type name: " << kid->getDataPtr()->getTypeName() << "\n";
        }
      }
      llvm::outs() << "\n"; 
*/
void Xlat:: xlatprocesstype(string prefix, Tree<TemplateType> * treep, TreeNode<TemplateType> * nodep, hNodep &h_typeinfo) {
  TreeNode<TemplateType> * parentp = nodep->getParent();
  os_ << "xlatprocesstype pointers are " << treep << " " << nodep << "\n";
  // check if sc_in, sc_out, sc_inout, sc_signal
  // if so check template parameter to see if primitive -- bool or int, uint of a certain width
  if (parentp == nodep) {
    os_ << "parent (itself) type name: " << parentp->getDataPtr()->getTypeName() << "\n";
  }									    
    else {
          // It is a different parent.
      
      os_ << "parent (different) type name: " << parentp->getDataPtr()->getTypeName() << "\n";
      os_ << "parent type follows\n";
      parentp->getDataPtr()->getTypePtr()->dump(os_);
      const RecordType * rectype = dyn_cast<RecordType>(parentp->getDataPtr()->getTypePtr());
      if (rectype && !lutil.isSCType(parentp->getDataPtr()->getTypeName())) {
	for (auto const &fld: rectype->getDecl()->fields()) {
	  os_ << "field of record type \n";
	  fld ->dump(os_);
	}
      }
	     
      os_ << "this node type name: " << nodep->getDataPtr()->getTypeName() << "\n";

    }
	
  auto children{ treep->getChildren(nodep)};
  for (auto const &child: children) {
    xlatprocesstype(prefix, treep, child, h_typeinfo);
    
  }
}

void Xlat::xlattype(string prefix, FindTemplateTypes *tt, hNodep &h_typeinfo) {
  //tt->printTemplateArguments(os_);

  Tree<TemplateType> *template_argtp = tt->getTemplateArgTreePtr();

  //llvm::outs()  << "xlattype dump of templateargs follows\n";
  //template_args->dump();
  if (template_argtp->size() == 1) {
    string tmps = ((template_argtp->getRoot())->getDataPtr())->getTypeName();  
    os_ << "primitive type " << tmps << "\n";
    lutil.make_ident(tmps);
    h_typeinfo->child_list.push_back(
       new hNode(tmps, hNode::hdlopsEnum::hType));
    return;
  }

  // now process composite types
  if (template_argtp->getRoot()) {
    string tmps = ((template_argtp->getRoot())->getDataPtr())->getTypeName();
    os_ << " nonprimitive type " << tmps << "\n";
    if (!lutil.isSCType(tmps)) {
	auto children{ template_argtp->getChildren(template_argtp->getRoot())};
	for (auto const &child: children) {
	  xlatprocesstype(prefix, template_argtp, child, h_typeinfo);
	}
    }
  }
  
  for (auto const &node : *template_argtp) {
    const TemplateType * type_data{node->getDataPtr()}; 
    string tmps =  type_data->getTypeName();
    if (tmps.empty()) os_ << "xlattype typename is empty\n";
    else os_ << "xlattype typename is " << tmps << "\n";
    auto parent_node{node->getParent()};
    if (parent_node != node) { //has children
    }
    lutil.make_ident(tmps);
    h_typeinfo->child_list.push_back(
       new hNode(tmps, hNode::hdlopsEnum::hType));
  }
}

void Xlat::xlattype(string prefix, FieldDecl *fieldd, hNodep &h_typeinfo) {
  os_ << "PortVarSigtype " << prefix << " is\n";
  if (fieldd) {
    fieldd->dump(os_);
    QualType q = fieldd->getType();
    if (!lutil.isSCType(q.getAsString())) {
      os_ <<"non primitive type " << q.getAsString() << "\n";
    }
  }
  else os_ << "EMPTY FieldDecl pointer\n";
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
