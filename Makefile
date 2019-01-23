configure:
	cmake -DCMAKE_INSTALL_PREFIX=./cmake-build-debug ./

develop:
	cmake --build ./cmake-build-debug --target softengine -- -j 4

run:
	cd ./cmake-build-debug/; ./softengine