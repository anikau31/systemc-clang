#include "FindEntryFunctions.h"

using namespace scpar;

FindEntryFunctions::FindEntryFunctions (CXXRecordDecl * d, llvm::raw_ostream & os):_os (os),
_d (d),
_isEntryFunction (false),
_procType (NONE),
_entryCXXRecordDecl (nullptr),
_entryMethodDecl (nullptr),
_foundEntryDeclaration (false), _constructorStmt (nullptr), pass (1)
{

	/// Pass 1:
	/// Find the constructor definition, and the Stmt* that has the code for it.
	/// Set the _constructorStmt pointer.
	//  _os << "\n>>>> PASS 1\n";

	TraverseDecl (_d);
	//  _os << "\n EntryFunctions found: " << _entryFunctions.size() << "\n";
	pass = 2;

	//    _os << "\n>>>> PASS 2\n";
	/// Pass 2:
	/// Get the entry function name from constructor
	TraverseStmt (_constructorStmt);
	pass = 3;

	/// Pass 3:
	/// Find the CXXMethodDecl* to the entry function
	TraverseDecl (_d);
	pass = 4;

}

FindEntryFunctions::~FindEntryFunctions() {
  _entryCXXRecordDecl = nullptr;
  _entryMethodDecl = nullptr;
  _constructorStmt = nullptr;
}

bool FindEntryFunctions::VisitMemberExpr (MemberExpr * e)
{
	//_os << "\tVisitMemberExpr";
	switch (pass)
		{
		case 2:
			{

				string
					memberName = e->getMemberDecl ()->getNameAsString ();

				// _os <<"\n member name : " <<memberName;

				//_os << "####: MemberExpr -- " << memberName << "\n";
				if (memberName == "create_method_process")
					{
						_procType = METHOD;
					}
				else if (memberName == "create_thread_process")
					{
						_procType = THREAD;
					}
				else if (memberName == "create_cthread_process")
					{
						_procType = CTHREAD;
					}

				break;
			}
		default:
			break;
		}

	return true;
}

bool FindEntryFunctions::VisitStringLiteral (StringLiteral * s)
{
	//_os << "\t\tVisitStringLiteral\n";
	switch (pass)
		{
		case 2:
			{
				//_os << "\nVisitStringLiteral\n";
				_entryName = s->getString ();


				/// FIXME: Where do we erase these?
				/// Create the object to handle multiple entry functions.

				//  EntryFunctionType* ef = new EntryFunctionType();

				ef = new EntryFunctionContainer ();
				//ef->setConstructorStmt (_constructorStmt);
				ef->setName (_entryName);
				ef->setProcessType (_procType);

				if (_procType != 0)
					{
						_entryFunctions.push_back (ef);
					}
/*    ef->_constructorStmt = _constructorStmt;
    ef->_entryName = _entryName;
    ef->_procType = _procType;
 */


				break;
			}
		case 3:
			{
				break;
			}
		default:
			break;
		}
	return true;
}

bool FindEntryFunctions::VisitCXXMethodDecl (CXXMethodDecl * md)
{
	// _os << "\nVisitCXXMethodDecl: " << pass << ", " << md->getNameAsString() << "\n";  
	_otherFunctions.push_back (md);
	switch (pass)
		{
		case 1:
			{
				//    _os << "\n\nPass 1 of VisitCXXMethodDecl\n\n";
				/// Check if method is a constructor
				if (CXXConstructorDecl * cd = dyn_cast < CXXConstructorDecl > (md))
					{
						const FunctionDecl *
							fd = NULL;

						cd->getBody (fd);

						if (cd->hasBody ())
							{
								_constructorStmt = cd->getBody ();
							}
					}
				break;
			}
		case 2:
			{
				//    _os << "\n\nPass 2 of VisitCXXMethodDecl\n\n";
				break;
			}
		case 3:
			{
				/// Check if name is the same as _entryFunction.
				//_os <<"\n md name : " <<md->getNameAsString();
				for (size_t i = 0; i < _entryFunctions.size (); i++)
					{
						if (md->getNameAsString () == _entryFunctions.at (i)->getName ())
							{
								EntryFunctionContainer *
									tmp = _entryFunctions.at (i);

								_entryMethodDecl = md;
								if (_entryMethodDecl != NULL)
									{
										tmp->setEntryMethod (_entryMethodDecl);
										//ef->_entryMethodDecl = _entryMethodDecl;
									}
							}
					}
				//    _os << "\n case 3: " << md->getNameAsString() << ", " << _entryName << "\n";
				break;
			}
		}
	return true;
}

CXXRecordDecl *
FindEntryFunctions::getEntryCXXRecordDecl (
)
{
	assert (!(_entryCXXRecordDecl == NULL));
	return _entryCXXRecordDecl;
}

CXXMethodDecl *
FindEntryFunctions::getEntryMethodDecl (
)
{
	assert (!(_entryMethodDecl == NULL));
	return _entryMethodDecl;
}

void
  FindEntryFunctions::dump (
)
{
	_os << "\n ============== FindEntryFunction ===============\n";
	_os << "\n:> Print Entry Function informtion for : " <<
		_d->getNameAsString () << "\n";
	for (size_t i = 0; i < _entryFunctions.size (); i++)
		{
			EntryFunctionContainer * ef = _entryFunctions[i];

			_os << "\n:> Entry function name: " << ef->getName () <<
				", process type: ";
			switch (ef->getProcessType ())
				{
				case THREAD:
					_os << " SC_THREAD\n";
					break;
				case METHOD:
					_os << " SC_METHOD\n";
					break;
				case CTHREAD:
					_os << " SC_CTHREAD\n";
					break;
				default:
					_os << " NONE\n";
					break;
				}
			_os << ":> CXXMethodDecl addr: " << ef->getEntryMethod () << "\n";
			//ef->_entryMethodDecl->dump();
		}
	_os << "\n ============== END FindEntryFunction ===============\n";
	_os << "\n";
}

FindEntryFunctions::entryFunctionVectorType *
	FindEntryFunctions::getEntryFunctions ()
{
	return new entryFunctionVectorType (_entryFunctions);
}

vector < CXXMethodDecl * >FindEntryFunctions::getOtherFunctions ()
{
	return _otherFunctions;
}

string FindEntryFunctions::getEntryName ()
{
	return _entryName;
}
