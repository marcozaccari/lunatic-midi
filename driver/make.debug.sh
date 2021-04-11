./make.sh debug

exit $?

if [ $? -eq 0 ]; then
    ./lunatic-driver debug
fi
