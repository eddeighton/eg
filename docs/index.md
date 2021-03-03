---
layout: default
title: EG documentation
---

# {{ page.title }}

Welcome to the EG programming language documentation.

EG is a C++ extension language designed for creative programming and experimental gameplay development.

The EG language extends C++ by adding the ability to write .eg files.  Generally an eg program requires a host C++ program.  The current version supports a "basic" host and "cinder" host.  In the future it is hoped to support many hosts including all popular game engines.

A simple command line tool eg.exe is provided which facilitates creating, building and running eg projects.  An eg project must have a .eg file in its folder which tells the eg driver what host and packages to build.  Hosts and packages can load the eg program database and perform arbitrary code generation reflecting on the eg action tree.

When writing an EG source file you may no longer define C++ structures, classes or functions.  Instead you can only define actions.  Actions are the core concept of doing anything in EG.  One might say eg is an action oriented language.

