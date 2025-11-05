# Simulation Module

## Purpose

- This module is an abstraction of the pyroomacoustic module that we are using for pyroom.
- It defines methods that build on top of the functionality that is present in the pyroomacoustics module. 
- The benefit this provides is that it encapsulates the logic of methods we define in our pyroom from the testing environment, which can happen anywhere now (as long as you are using the same venv). 

## Installation

1. Recommendation: Use a virtual environment, since you may not want to install this custom module in your actual python library. Can create a venv by doing `python -m venv venv`. This creates a venv folder, activate the venv by calling `source venv/bin/activate`.
2. Install simulation module using `pip install -e ./simulation`. 
