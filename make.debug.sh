./make.sh debug

exit 0
if [ $? -eq 0 ]; then
    ./midi-controller debug
fi
