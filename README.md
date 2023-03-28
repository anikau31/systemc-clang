# SCCL: FCCM AE 2023

SCCL: A SystemC translator to RTL
==========================================

(systemc-clang)[https://github.com/anikau31/systemc-clang] parses SystemC 2.3.3. It parses RTL constructs and some TLM 2.0 constructs. There is a HDL synthesis plugin that generates Verilog from SystemC RTL descriptions. 

Requirements
------------

*  `llvm/clang <https://releases.llvm.org/download.html>`_ (version 13.0.0)
*  `SystemC <http://systemc.org>`_ version 2.3.3. 
*  c++17 is required. 
*  docker 23.0.1

Docker use
-----------

- Clone the FCCM AE SCCL repository. TODO: Update the repository link.
```
git clone https://github.com/rseac/systemc-clang.git
```

- Change into the directory
```
cd systemc-clang;
```

- Ensure that docker is present. We use version 23.0.1.
```
docker --version
```

- Build the docker container.
```
docker build -t fccm-ae
```

- Run the docker container in interactive mode
```
docker run -it fccm-ae
```

- Once you are inside the container, you can execute all the case studies through SCCL by invoking the `step1.sh`

```
cd /systemc-clang/;
sh step1.sh;
```

You will notice that the generated Verilog files will be within the respective directories for the case studies.
