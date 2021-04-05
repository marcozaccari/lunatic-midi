./make.sh debug

exit 0
if [ $? -eq 0 ]; then
    ./lunatic-driver debug
fi
