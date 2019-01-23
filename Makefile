configure:
	mkdir -p ./cmake-build-debug
	cd ./cmake-build-debug; cmake ../

develop:
	cmake --build ./cmake-build-debug --target softengine -- -j 4

run:
	cd ./cmake-build-debug/; ./softengine