# a1000 bare driver test

more information:

	`./make_$(arch).sh help`

# Use docker clone

* The first way:

	`git clone git@10.10.13.252:a800/baremetal_verification.git`

	`cd baremetal_verification`

For first clone:

	`git submodule update --init --recursive`

Otherwise:

	`git submodule update --remote`

* The second way:

	`git clone --recurse-submodules git@10.10.13.252:a800/baremetal_verification.git`

