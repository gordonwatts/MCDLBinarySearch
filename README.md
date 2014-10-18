MCDLBinarySearch
================

Using Pythia8, and a basic Hidden Valley model, a few short programs to explore parameter space in a few different ways.

1. binary-search the parameter space to get spread the decay lengths out accross a region of 5cm to 15 m.
2. Dump what particles the HV vally pion decays to
3. Look at how the arrival time is affected by generation parameters (e.g. beta).

Building
========

Check out, and open in VS2012 or VS2013. Hit build. It will pull in from nuget correct versions of Pythia8 and ROOT.

Usage
=====

There are two powershell scripts in the root directory. The first one, run1masses.ps1, will run all the masses used by a HV
search in ATLAS for Run 1. The other powershell script, printSearchResults will print out final result for each log file it finds.

These scripts are amazingly simple, as is the binary search algorithm implemented by the code!
