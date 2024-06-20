Routing Paths Lengths
=====================

Currently, routing paths lengths are not saved and exported by the NENoCSi framework. However, unless modified, a simulation of the cortical microcircuit model of NEST always uses the same random number generator seed ([Potjans\_2014/sim\_params.py](https://github.com/nest/nest-simulator/blob/master/pynest/examples/Potjans_2014/sim_params.py)). According to NEST's guide on parallel simulation, this always results in the same biological network being generated, as long as the same number of threads is used ([NEST parallel computing](https://nest-simulator.readthedocs.io/en/v3.1/guides/parallel_computing.html)). Thus, in case of deterministic XY routing, histograms of routing paths lengths can be assembled by a Python script from the YAML files used for data exchange between NEST and NENoCSi. Examples of such YAML files for the cortical microcircuit model are available within the dataset at [Zenodo](https://doi.org/10.5281/zenodo.10159252).

To re-produce the histograms on routing paths lengths within 6 &#x00D7; 6 mesh and torus networks for the cortical microcircuit model scaled to 10%, do the following:

1. Download the three script files from this subfolder to a convenient working directory.
2. Download the ZIP-file available at [Zenodo](https://doi.org/10.5281/zenodo.10159252).
3. Extract the subfolder `NoC-Sim-Input` to a convenient location.
4. Create the subfolders `data` and `img` in the working directory.
5. Execute `det_routing_NENoCSi.sh` with the path to the parent directory of `NoC-Sim-Input` as argument.

By following these steps, two PDF images will be stored in the `img` subfolder created in step 3.
