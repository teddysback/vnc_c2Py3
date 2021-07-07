@echo on

echo =========
echo = Build =
echo =========

setup.py bdist

echo ===========
echo = Install =
echo ===========

setup.py install


echo ============
echo = FINISHED =
echo ============