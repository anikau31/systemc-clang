#set bind-root false
# match sc_modules
# m cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")))

# match ports
# sc_in 
# m fieldDecl(hasType(cxxRecordDecl(hasName("sc_in"))))
# sc_inout
# m fieldDecl(hasType(cxxRecordDecl(hasName("sc_inout"))))
# sc_out
# m fieldDecl(hasType(cxxRecordDecl(hasName("sc_out"))))
# sc_in_clk
# TODO

# Get all fields within the module
# The condition for node matchers is an "AND".
# forEach goes through all the matches of that fieldDecl.
set output diag
# m cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")), forEach(fieldDecl(hasType(cxxRecordDecl(hasName("sc_in")))).bind("sc_in"))).bind("sc_module")

# Match sc_in, sc_out, sc_inout, sc_signal, and bind them accordingly.
# m cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")), forEach(eachOf(fieldDecl(hasType(cxxRecordDecl(hasName("sc_in")))).bind("sc_in"), fieldDecl(hasType(cxxRecordDecl(hasName("sc_out")))).bind("sc_out"), fieldDecl(hasType(cxxRecordDecl(hasName("sc_inout")))).bind("sc_inout"), fieldDecl(hasType(cxxRecordDecl(hasName("sc_signal")))).bind("sc_signal")))).bind("sc_module")

# Trying to match all other fieldDecl that are not ports
# 1. derives from sc_module
# 2. is not sc_event_queue
# m cxxRecordDecl(isDerivedFrom(hasName("::sc_core::sc_module")), unless(isDerivedFrom(matchesName("sc_event_queue"))), forEach(eachOf(fieldDecl(hasType(cxxRecordDecl(hasName("sc_in")))).bind("sc_in"), fieldDecl(hasType(cxxRecordDecl(hasName("sc_out")))).bind("sc_out"), fieldDecl(hasType(cxxRecordDecl(hasName("sc_inout")))).bind("sc_inout"), fieldDecl(hasType(cxxRecordDecl(hasName("sc_signal")))).bind("sc_signal")))).bind("sc_module")

#
# Match all sc_module field varialbes that are not systemc types 
#
# 1. derives from sc_module
# 2. is not sc_event_queue
# 3. for every field declaration, match only those that don't begin with "sc_"

#m cxxRecordDecl(isExpansionInMainFile(), isDerivedFrom(hasName("::sc_core::sc_module")), unless(isDerivedFrom(matchesName("sc_event_queue"))),  forEach(fieldDecl(unless(hasType(cxxRecordDecl(matchesName("sc*"))))).bind("non_sc")))

#
# Match sc_in_clk
#
# Notice that we are using the namedDecl here because sc_in_clk is actually 
# a typedef to sc_in<bool>.
#
#m cxxRecordDecl(isDerivedFrom(hasName("sc_module")), forEach(fieldDecl(hasType(namedDecl(hasName("sc_in_clk")))).bind("sc_in_clk")))

# Match entry function
# 1. Find constructor
# 2. Find method with the name
m cxxRecordDecl(isExpansionInMainFile(), isDerivedFrom(hasName("::sc_core::sc_module")), hasDescendant(cxxConstructorDecl().bind("constructor")))
