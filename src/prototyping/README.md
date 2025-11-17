# Prototyping Environment

## Simulation Module

### Overall Structure

The `simulation` module in this folder encapsulates all of the POC logic for audio360. 
- It works as an abstraction of the pyroomacoustic module we are using to simulate the POC environment. 
- It also defines methods that implement various features and requirements we have defined in our requirements documentation. 

Inside this module, there are different python files that include the logic of different components in our POC simulation:
1. room.py: This file contains methods that are used to create the pyroom environment that the simulation will run in. 
2. visualization.py: Responsible for generating illustrations that helps visualize what is going on. 
3. signals.py: Generates signals from the source for the microphones in the simulation to capture. 
4. database.py: Loads and saves various audio files that need to be processed for the simulation. Also contains scripts that updates readme files. 
5. doa_analysis.py: Encapsulates the logic for analyzing directionality using microphone array. 
6. classification.py: Encapsulates the logic for classifying sounds using audio source. Depends on the `.pkl` file in the bin of this directory. 
7. __init__.py: Defines which methods the simulation module is exposing to be used when importing it in other python scripts. 

### Installation

1. Recommendation: Use a virtual environment, since you may not want to install this custom module in your actual python library. Can create a venv by doing `python -m venv venv`. This creates a venv folder, activate the venv by calling `source venv/bin/activate`.
2. Install simulation module using `pip install -e ./simulation`. 

## Example: Running the simulation

An example of how the simulation module can be used to run a simulation is provided in `run_simulation.py`. 

Note: In order to run this, you need to ensure that the simulation module has been installed in the python venv library using the installation guide above. 

You can run this file by using the command `python <path_to_run_simulation.py>`