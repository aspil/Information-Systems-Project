# Information-Systems-Project

### Compilation
Compilation of the app is done by using `make` or `make app`.
### Execution
Run with `./app <data_path> <csv_file_path> [-s <n>]`

- **data_path** : directory with json files. If there are subdirectories with more json files they are visited recursively
- **csv_file_path** : full path to the csv file containing the product relations
- **[-s <n\>]** : optional,specifies the size of the hash table that is used. If not specified, the program counts the number of the json files and uses that number as the hash table's size.

Examples:
- `./app Datasets Datasets/sigmod_large_labelled_dataset.csv`
- `./app Datasets Datasets/sigmod_medium_labelled_dataset.csv -s 137`
- `./app Datasets/camera_specs/2013_camera_specs Datasets/sigmod_large_labelled_dataset.csv -s 1000`

### Testing
Testing is performed on the three data structures and the app. To do so, do one of the following:
- `make test_app`
- `make test_vector`
- `make test_map`
- `make test_list`

For any of those, run`./test`

Before compiling another test file, make sure to clean using `make clean`.

### Contributors
- [Αλέξιος Σπηλιωτόπουλος](https://github.com/aspil) 1115201700147
- [Φανούριος-Στυλιανός Ψαθόπουλος](https://github.com/FanouriosStylianosPsathopoulos) 1115201400320
