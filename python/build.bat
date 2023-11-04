cd LatexrateC
gcc -shared -o LatexrateC.dll -m64 LatexrateC.c
cd ..
python setup.py sdist
pip install dist/LatexrateC-0.0.1.tar.gz
